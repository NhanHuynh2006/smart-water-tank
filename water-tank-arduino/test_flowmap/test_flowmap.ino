// Buoc 11: HAI CAM BIEN LUU LUONG CO BI DAU NGUOC KHONG?
//
// Bat bom, roi xem chan nao NHAY LEN. Chan nao thay dong cua bom thi chan do
// la cam bien DAU VAO. Don gian vay thoi.
//
// Cung do luon NEN AO: tan so doc duoc khi bom tat va bon da can, luc do
// khong con gi chay qua ca hai cam bien.
//
// Bom chi chay 12 giay. Ket thuc, ro le duoc dua ve NGAT.
#include <Arduino.h>
#include "config.h"

volatile uint32_t cA = 0, cB = 0;
void IRAM_ATTR isrA() { cA++; }
void IRAM_ATTR isrB() { cB++; }

void relayOff() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
}
void relayOn() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, LOW);
#else
  digitalWrite(PIN_RELAY, HIGH);
#endif
}

void countBoth(uint32_t ms, float* hzA, float* hzB) {
  cA = cB = 0;
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW),     isrA, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT), isrB, FALLING);
  delay(ms);
  detachInterrupt(digitalPinToInterrupt(PIN_FLOW));
  detachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT));
  *hzA = cA * 1000.0f / ms;
  *hzB = cB * 1000.0f / ms;
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();
  Serial.begin(115200);
  delay(400);
#if FLOW_PIN_PULLUP
  pinMode(PIN_FLOW, INPUT_PULLUP);
#else
  pinMode(PIN_FLOW, INPUT);
#endif
#if FLOW_OUT_PIN_PULLUP
  pinMode(PIN_FLOW_OUT, INPUT_PULLUP);
#else
  pinMode(PIN_FLOW_OUT, INPUT);
#endif

  Serial.println();
  Serial.println("=== HAI CAM BIEN LUU LUONG CO BI DAU NGUOC KHONG? ===");
  Serial.printf("config: GPIO %d goi la DAU VAO, GPIO %d goi la DAU RA\n",
                PIN_FLOW, PIN_FLOW_OUT);
  Serial.println("Bom se chay 12 giay. Chan nao NHAY LEN la chan thay dong cua bom.");
  for (int i = 3; i > 0; i--) { Serial.printf("  bat dau sau %d giay...\n", i); delay(1000); }
  Serial.println();

  float a0, b0, a1, b1, a2, b2;

  relayOff(); delay(1500);
  countBoth(6000, &a0, &b0);
  Serial.printf("bom TAT  (truoc) : GPIO %-2d = %7.1f Hz   |   GPIO %-2d = %7.1f Hz\n",
                PIN_FLOW, a0, PIN_FLOW_OUT, b0);

  relayOn(); delay(2000);
  countBoth(10000, &a1, &b1);
  Serial.printf("bom CHAY         : GPIO %-2d = %7.1f Hz   |   GPIO %-2d = %7.1f Hz\n",
                PIN_FLOW, a1, PIN_FLOW_OUT, b1);

  relayOff(); delay(2000);
  countBoth(6000, &a2, &b2);
  Serial.printf("bom TAT  (sau)   : GPIO %-2d = %7.1f Hz   |   GPIO %-2d = %7.1f Hz\n",
                PIN_FLOW, a2, PIN_FLOW_OUT, b2);

  relayOff();

  float baseA = (a0 + a2) / 2, baseB = (b0 + b2) / 2;
  float riseA = a1 - baseA, riseB = b1 - baseB;

  Serial.println();
  Serial.println("=== KET LUAN ===");
  Serial.printf("nen khi bom tat  : GPIO %-2d = %6.1f Hz   |   GPIO %-2d = %6.1f Hz\n",
                PIN_FLOW, baseA, PIN_FLOW_OUT, baseB);
  Serial.printf("muc NHAY LEN     : GPIO %-2d = %6.1f Hz   |   GPIO %-2d = %6.1f Hz\n",
                PIN_FLOW, riseA, PIN_FLOW_OUT, riseB);
  Serial.println();

  if (riseA < 20 && riseB < 20) {
    Serial.println("KHONG CHAN NAO NHAY LEN. Bom khong day duoc nuoc, hoac ca hai");
    Serial.println("cam bien deu chua dau xong. Khong ket luan duoc chuyen nguoc.");
  } else if (riseA > riseB * 2) {
    Serial.printf("GPIO %d thay dong cua bom => no la DAU VAO. Config DUNG.\n", PIN_FLOW);
  } else if (riseB > riseA * 2) {
    Serial.printf("GPIO %d thay dong cua bom => no moi la DAU VAO.\n", PIN_FLOW_OUT);
    Serial.println(">>> HAI CAM BIEN DANG BI DAU NGUOC. Doi PIN_FLOW va PIN_FLOW_OUT. <<<");
  } else {
    Serial.println("CA HAI cung nhay len gan bang nhau — nuoc chay qua ca hai,");
    Serial.println("hoac hai day dang chap vao nhau. Kiem tra lai duong ong va day.");
  }

  Serial.println();
  Serial.printf("NEN AO: chan nao co nen khac 0 khi bom tat va bon can la con nhieu.\n");
  Serial.printf("  He so K dang dung: vao %.1f, ra %.1f xung moi lit/phut\n",
                (float)FLOW_K_FACTOR, (float)FLOW_OUT_K_FACTOR);
  Serial.printf("  Nen quy ra luu luong ao: vao %.2f L/ph, ra %.2f L/ph\n",
                baseA / FLOW_K_FACTOR, baseB / FLOW_OUT_K_FACTOR);
  Serial.println("XONG. Chuong trinh dung han, ro le da NGAT.");
}

void loop() { relayOff(); delay(1000); }
