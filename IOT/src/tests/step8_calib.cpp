// Buoc 8: DO HINH HOC THAT CUA BON, de dat lai nguong cho dung.
//
// Chuong trinh nay KHONG BAO GIO bat bom. Ro le bi giu NGAT suot.
// No chi doc khoang cach sieu am va hai phao, roi in ra so that.
// Chay co gioi han thoi gian roi dung han.
#include <Arduino.h>
#include "config.h"

#define RUN_MS   90000UL
#define PERIOD   500UL

void relayOff() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
}

float pingCm() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  uint32_t us = pulseIn(PIN_ECHO, HIGH, 30000UL);
  if (us == 0) return -1;
  return us * 0.0343f / 2.0f;
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();                       // bom NGAT truoc moi thu khac

  Serial.begin(115200);
  delay(400);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_FLOAT_MAX, INPUT_PULLUP);
  pinMode(PIN_FLOAT_SRC, INPUT_PULLUP);

  Serial.println();
  Serial.println("=== DO HINH HOC BON — BOM LUON NGAT ===");
  Serial.printf("config dang khai bao: day cach cam bien %.1f cm, muc day %.1f cm\n",
                TANK_SENSOR_TO_BOTTOM_CM, TANK_MAX_LEVEL_CM);
  Serial.printf("=> khoang cach hop le phai nam trong [%.1f .. %.1f] cm\n",
                TANK_SENSOR_TO_BOTTOM_CM - TANK_MAX_LEVEL_CM,
                TANK_SENSOR_TO_BOTTOM_CM);
  Serial.println("phao: HO = tiep diem mo, CHAM = tiep diem dong");
  Serial.println("---- d_cm | muc_cm | muc_% | hop_le | phao_max | phao_nguon ----");

  uint32_t t0 = millis();
  float dmin = 1e9, dmax = -1e9, dsum = 0; int n = 0, bad = 0;

  while (millis() - t0 < RUN_MS) {
    relayOff();
    float d = pingCm();
    bool fmax = (digitalRead(PIN_FLOAT_MAX) == LOW);
    bool fsrc = (digitalRead(PIN_FLOAT_SRC) == LOW);

    if (d < 0) {
      bad++;
      Serial.printf("  --.- |   --.- |  --.- | KHONG DOC DUOC | %s | %s\n",
                    fmax ? "CHAM" : "HO", fsrc ? "CHAM" : "HO");
    } else {
      float h  = TANK_SENSOR_TO_BOTTOM_CM - d;
      float pc = (h / TANK_MAX_LEVEL_CM) * 100.0f;
      bool ok  = (h >= -2.0f && h <= TANK_MAX_LEVEL_CM + 5.0f);
      if (d < dmin) dmin = d;
      if (d > dmax) dmax = d;
      dsum += d; n++;
      Serial.printf("%6.1f | %6.1f | %5.1f | %s | %s | %s\n",
                    d, h, pc, ok ? "  hop le  " : "NGOAI DAI!",
                    fmax ? "CHAM" : "HO", fsrc ? "CHAM" : "HO");
    }
    delay(PERIOD);
  }

  relayOff();
  Serial.println();
  Serial.println("=== TONG KET ===");
  if (n > 0) {
    Serial.printf("so mau doc duoc : %d, hong %d (%.0f%%)\n", n, bad, 100.0f*bad/(n+bad));
    Serial.printf("khoang cach     : nho nhat %.1f · trung binh %.1f · lon nhat %.1f cm\n",
                  dmin, dsum/n, dmax);
    Serial.printf("dao dong        : %.1f cm\n", dmax - dmin);
    Serial.printf("=> neu mat nuoc DUNG YEN, hay dat TANK_SENSOR_TO_BOTTOM_CM\n");
    Serial.printf("   = %.1f + chieu cao nuoc hien tai do bang thuoc (cm)\n", dsum/n);
  } else {
    Serial.println("KHONG DOC DUOC MAU NAO. Kiem tra day TRIG/ECHO va nguon 5 V.");
  }
  Serial.println("XONG. Chuong trinh dung han, ro le van NGAT.");
}

void loop() { relayOff(); delay(1000); }
