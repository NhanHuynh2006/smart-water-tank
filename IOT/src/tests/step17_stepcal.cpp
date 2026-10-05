// Buoc 17: HIEU CHUAN MUC NUOC THEO BAC, chuan doi chieu la THE TICH BOM VAO.
//
// Dieu kien: VAN XA DONG. Bom tung bac STEP_MS; sau moi bac dung bom SETTLE_MS
// cho mat nuoc lang, roi phat sieu am N_PING lan. Cam bien luu luong dau vao
// (PIN_FLOW) dem xung suot qua trinh => the tich da bom => do cao nuoc da them
// = the tich / tiet dien day. Neu muc do bang sieu am tang dung bang do cao
// nay thi he so goc = 1; sai lech hang so (offset) duoc chot bang MOT lan doc
// thuoc o cuoi.
//
// An toan: dung han neu khoang cach tho < 5,5 cm, hoac da bom qua MAX_L lit,
// hoac qua MAX_STEPS bac.
#include <Arduino.h>
#include "config.h"

#define STEP_MS    20000UL
#define SETTLE_MS  12000UL
#define N_PING     40
#define MAX_STEPS  6
#define MAX_L      0.80f

volatile uint32_t pulses = 0;
void IRAM_ATTR onPulse() { pulses++; }

void relay(bool on) {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, on ? LOW : HIGH);
#else
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
#endif
}
float ping() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  uint32_t us = pulseIn(PIN_ECHO, HIGH, 30000UL);
  return us ? us * 0.0343f / 2.0f : -1;
}
// In ra moi lan doc de may chu tu tinh thong ke; tra ve khoang cach nho nhat
// trong cac lan doc hop le (tieng doi tre chi lam doc xa hon).
float measure(int step) {
  float mn = 999;
  for (int i = 0; i < N_PING; i++) {
    float d = ping();
    Serial.printf("P,%d,%.2f\n", step, d);
    if (d > 0 && d < mn) mn = d;
    delay(150);
  }
  return mn;
}
void setup() {
  pinMode(PIN_RELAY, OUTPUT); relay(false);
  Serial.begin(115200); delay(500);
  pinMode(PIN_TRIG, OUTPUT); pinMode(PIN_ECHO, INPUT);
#if FLOW_PIN_PULLUP
  pinMode(PIN_FLOW, INPUT_PULLUP);
#else
  pinMode(PIN_FLOW, INPUT);
#endif
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW), onPulse, FALLING);
  Serial.println("BEGIN");
  delay(SETTLE_MS);
  measure(0);
  Serial.printf("S,0,0.0000\n");
  for (int s = 1; s <= MAX_STEPS; s++) {
    relay(true); delay(STEP_MS); relay(false);
    delay(SETTLE_MS);
    float litres = pulses / FLOW_K_FACTOR / 60.0f;
    float mn = measure(s);
    Serial.printf("S,%d,%.4f\n", s, litres);
    if (litres > MAX_L || (mn > 0 && mn < 5.5f)) { Serial.println("STOP an toan"); break; }
  }
  relay(false);
  Serial.println("END");
}
void loop() { relay(false); delay(1000); }
