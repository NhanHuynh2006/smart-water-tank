// Buoc 12: BOM CHAY LAM HONG SIEU AM BANG DIEN, HAY BANG NUOC?
//
// Phat sieu am lien tuc 200 ms mot lan, va dong cat ro le giua chung.
// Doc ket qua theo THOI DIEM hong:
//
//   Hong NGAY trong 1-2 lan phat dau sau khi ro le dong  -> NHIEU DIEN.
//     Nuoc khong the doi 3 cm trong 0,4 giay. Bom that chi day 0,06 cm/s.
//   Hong dan sau vai giay, cang luc cang nang               -> NUOC.
//
// Bom chay 10 giay moi vong. Ket thuc, ro le duoc dua ve NGAT.
#include <Arduino.h>
#include "config.h"

#define PERIOD_MS   200
#define OFF_TICKS    40    // 8 giay
#define ON_TICKS     50    // 10 giay
#define ROUNDS        2

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

float pingCm() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  uint32_t us = pulseIn(PIN_ECHO, HIGH, 30000UL);
  return us ? us * 0.0343f / 2.0f : -1;
}

void phase(const char* nhan, int ticks, bool on) {
  if (on) relayOn(); else relayOff();
  Serial.printf("\n--- %s ---\n", nhan);
  int bad = 0, n = 0;
  float mn = 1e9, mx = -1e9; double sum = 0;
  for (int i = 0; i < ticks; i++) {
    float d = pingCm();
    if (i < 12 || d < 0 || d > TANK_SENSOR_TO_BOTTOM_CM - 0.5f) {
      // in chi tiet 12 lan phat dau, va moi lan doc bat thuong
      Serial.printf("  t=%+5.1fs  ", i * PERIOD_MS / 1000.0f);
      if (d < 0) Serial.println("KHONG CO TIENG DOI");
      else Serial.printf("%6.2f cm  (muc %5.2f cm)%s\n", d,
                         TANK_SENSOR_TO_BOTTOM_CM - d,
                         d > TANK_SENSOR_TO_BOTTOM_CM - 0.5f ? "  <-- BAO CAN" : "");
    }
    if (d < 0) bad++;
    else { n++; sum += d; if (d < mn) mn = d; if (d > mx) mx = d; }
    delay(PERIOD_MS);
  }
  if (n) Serial.printf("  => %d mau · trung binh %.2f cm · dao dong %.2f cm · hong %d\n",
                       n, sum / n, mx - mn, bad);
  else   Serial.printf("  => KHONG doc duoc mau nao, hong %d\n", bad);
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();
  Serial.begin(115200);
  delay(400);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  Serial.println();
  Serial.println("=== BOM LAM HONG SIEU AM BANG DIEN HAY BANG NUOC? ===");
  Serial.printf("cam bien cach day %.1f cm · bom that day %.3f cm/s\n",
                TANK_SENSOR_TO_BOTTOM_CM, 0.06f);
  Serial.println("Trong 0,4 giay nuoc chi kip len 0,024 cm. Bat ky cu nhay nao");
  Serial.println("lon hon the ngay sau khi ro le dong deu la NHIEU DIEN.");
  for (int i = 3; i > 0; i--) { Serial.printf("  bat dau sau %d giay...\n", i); delay(1000); }

  for (int r = 1; r <= ROUNDS; r++) {
    Serial.printf("\n========== VONG %d ==========", r);
    phase("RO LE NGAT", OFF_TICKS, false);
    phase("RO LE DONG — bom chay", ON_TICKS, true);
  }

  relayOff();
  Serial.println();
  Serial.println("=== CACH DOC ===");
  Serial.println("Nhin 12 lan phat dau cua doan RO LE DONG:");
  Serial.println("  hong ngay tu lan 1-2      -> NHIEU DIEN. Ha cau chia ap ECHO");
  Serial.println("                               xuong 1k/2k, va them diot 1N4007.");
  Serial.println("  van dung vai giay roi hong -> NUOC, hoac bot khi trong ong lang.");
  Serial.println("XONG. Chuong trinh dung han, ro le da NGAT.");
}

void loop() { relayOff(); delay(1000); }
