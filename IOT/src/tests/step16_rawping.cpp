// Buoc 16: ghi khoang cach THO cua tung lan phat sieu am, bom tat roi bom chay.
// Dung de phan biet: nhieu do NUOC/HINH HOC (co ca luc bom tat) hay do BOM
// (chi khi bom chay). Moi dong: pha, thoi diem ms, khoang cach cm (-1 = im).
// Bom chay toi da 60 s, tu ngat neu muc tren 75 %.
#include <Arduino.h>
#include "config.h"

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
  uint32_t us = pulseIn(PIN_ECHO, HIGH, 40000UL);
  return us ? us * 0.0343f / 2.0f : -1;
}
void phase(const char* name, uint32_t ms, bool pumpOn, uint32_t period) {
  relay(pumpOn);
  uint32_t t0 = millis(); int high = 0;
  while (millis() - t0 < ms) {
    float d = ping();
    Serial.printf("%s,%lu,%.2f\n", name, (unsigned long)(millis() - t0), d);
    if (pumpOn && d > 0 && (TANK_SENSOR_TO_BOTTOM_CM - d) > 0.75f * TANK_MAX_LEVEL_CM) {
      if (++high >= 3) break;
    } else high = 0;
    delay(period);
  }
  relay(false);
}
void setup() {
  pinMode(PIN_RELAY, OUTPUT); relay(false);
  Serial.begin(115200); delay(500);
  pinMode(PIN_TRIG, OUTPUT); pinMode(PIN_ECHO, INPUT);
  Serial.println("BEGIN");
#if defined(RAW_BACKFLOW)
  // Kiem tra chay nguoc: van xa DONG. Bom tat 60 s (muc phai dung yen), bom
  // 30 s, roi tat 120 s: muc tut sau khi tat bom = nuoc chay nguoc qua bom.
  phase("still", 60000, false, 200);
  phase("on200", 30000, true,  200);
  phase("after", 120000, false, 200);
#elif !defined(RAW_FILL_FIRST)
  phase("off200", 30000, false, 200);
  phase("off60",  15000, false, 60);
  phase("on200",  60000, true,  200);
  phase("after",  20000, false, 200);
#else
  // Bien the: bom len cao truoc, roi do luc nuoc dung yen o muc cao
  phase("on200", RAW_FILL_FIRST, true, 200);
  phase("off200", 40000, false, 200);
  phase("off60",  20000, false, 60);
#endif
  Serial.println("END");
}
void loop() { relay(false); delay(1000); }
