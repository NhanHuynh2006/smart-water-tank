// Buoc 15: bom nuoc vao va theo doi cam bien tung lan phat.
//
// Bon 1 lit, bom day 0,36 L/phut. Chay 90 giay la them 0,54 lit — chua day
// bon can. Chuong trinh tu ngat va dung han, khong the tran.
#include <Arduino.h>
#include "config.h"

#define PUMP_MS   90000UL
#define PERIOD    400UL

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

float ping() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  uint32_t us = pulseIn(PIN_ECHO, HIGH, 40000UL);
  return us ? us * 0.0343f / 2.0f : -1;
}

int watch(const char* nhan, uint32_t ms, bool pumpOn) {
  if (pumpOn) relayOn(); else relayOff();
  Serial.printf("\n--- %s ---\n", nhan);
  uint32_t t0 = millis();
  int co = 0, tong = 0;
  while (millis() - t0 < ms) {
    float d = ping();
    tong++;
    if (d > 0) {
      co++;
      Serial.printf("  %5.1fs  %6.2f cm   muc %5.2f cm = %5.1f%%\n",
                    (millis() - t0) / 1000.0f, d,
                    TANK_SENSOR_TO_BOTTOM_CM - d,
                    (TANK_SENSOR_TO_BOTTOM_CM - d) / TANK_MAX_LEVEL_CM * 100.0f);
    } else if (tong % 10 == 0) {
      Serial.printf("  %5.1fs  khong co tieng doi (%d/%d lan im)\n",
                    (millis() - t0) / 1000.0f, tong - co, tong);
    }
    delay(PERIOD);
  }
  if (pumpOn) relayOff();
  Serial.printf("  => %d/%d lan co tieng doi\n", co, tong);
  return co;
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT); relayOff();
  Serial.begin(115200); delay(400);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  Serial.println();
  Serial.println("=== BOM NUOC VAO VA THEO DOI CAM BIEN ===");
  Serial.printf("Bom chay toi da %lu giay = %.2f lit vao bon %.1f lit.\n",
                PUMP_MS / 1000, PUMP_MS / 60000.0f * PUMP_FILL_LPM,
                TANK_AREA_CM2 * TANK_MAX_LEVEL_CM / 1000.0f);
  for (int i = 3; i > 0; i--) { Serial.printf("  bat dau sau %d giay...\n", i); delay(1000); }

  int a = watch("BOM TAT, truoc khi bom", 6000, false);
  int b = watch("BOM CHAY", PUMP_MS, true);
  int c = watch("BOM TAT, sau khi bom", 10000, false);

  relayOff();
  Serial.println();
  Serial.println("=== KET LUAN ===");
  if (a == 0 && b == 0 && c == 0) {
    Serial.println("KHONG MOT TIENG DOI NAO trong ca ba doan, ke ca khi da co nuoc.");
    Serial.println("Cam bien khong phat duoc. Day KHONG phai chuyen bon can.");
    Serial.println("Kiem tra: VCC co du 5 V khong · day TRIG o GPIO 5 · diem GIUA");
    Serial.println("cua cau chia ap moi di vao GPIO 18. Neu dung het thi cam bien hong.");
  } else if (a == 0 && (b > 0 || c > 0)) {
    Serial.println("Im khi bon can, co tieng doi khi da co nuoc.");
    Serial.println("Cam bien VAN SONG. Day bon kho tan song di het, chi mat nuoc");
    Serial.println("phang moi doi lai duoc. Khong phai loi.");
  } else {
    Serial.println("Co tieng doi. Cam bien con song.");
  }
  Serial.println("XONG. Chuong trinh dung han, ro le da NGAT.");
}
void loop() { relayOff(); delay(1000); }
