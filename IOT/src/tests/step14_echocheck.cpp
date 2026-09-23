// Buoc 14: HC-SR04 con song khong? Dut o dau?
//
// Khong co tieng doi nao co the do bon nguyen nhan. Bai nay tach chung ra.
#include <Arduino.h>
#include "config.h"

void relayOff() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
}

int pctHigh(int pin, int mode) {
  pinMode(pin, mode); delay(60);
  int h = 0;
  for (int i = 0; i < 200; i++) { if (digitalRead(pin) == HIGH) h++; delayMicroseconds(400); }
  return h / 2;
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT); relayOff();
  Serial.begin(115200); delay(400);
  Serial.println();
  Serial.println("=== HC-SR04 CON SONG KHONG? ===");

  // 1. Chan ECHO co bi cai gi giu khong
  int up   = pctHigh(PIN_ECHO, INPUT_PULLUP);
  int down = pctHigh(PIN_ECHO, INPUT_PULLDOWN);
  pinMode(PIN_ECHO, INPUT);
  int none = pctHigh(PIN_ECHO, INPUT);
  Serial.printf("\nCHAN ECHO (GPIO %d)\n", PIN_ECHO);
  Serial.printf("  keo len 3,3 V : %3d%% o muc cao\n", up);
  Serial.printf("  khong keo     : %3d%% o muc cao\n", none);
  Serial.printf("  keo xuong dat : %3d%% o muc cao\n", down);
  if (up > 90 && down < 10)
    Serial.println("  => DI THEO CA HAI CHIEU: chan dang THA NOI.");
  else if (up < 20 && down < 20)
    Serial.println("  => bi giu o MUC THAP. Cam bien co dien va dang nghi — BINH THUONG.");
  else
    Serial.println("  => bi giu o muc cao, hoac dang dao.");

  // 2. Phat thu va do do dai xung ECHO that su tro ve
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  Serial.println("\n20 LAN PHAT, do do rong xung ECHO:");
  int co = 0;
  for (int i = 0; i < 20; i++) {
    digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(4);
    digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);
    uint32_t us = pulseIn(PIN_ECHO, HIGH, 40000UL);
    if (us) { co++; Serial.printf("  lan %2d: %6lu us = %6.2f cm\n", i+1, (unsigned long)us, us*0.0343f/2); }
    delay(100);
  }
  Serial.printf("  => %d/20 lan co xung tro ve\n", co);

  Serial.println("\n=== CACH DOC ===");
  if (co == 0) {
    Serial.println("KHONG MOT XUNG NAO. Kiem tra theo dung thu tu nay:");
    Serial.println("  1. Do dien ap chan VCC cua HC-SR04 so voi GND.");
    Serial.println("     Phai la 5 V. Duoi 4,5 V la cam bien khong du dien de phat.");
    Serial.println("  2. Kiem tra day TRIG co dung chan GPIO 5 khong.");
    Serial.println("  3. Kiem tra cau chia ap tren ECHO: diem GIUA hai dien tro");
    Serial.println("     moi di vao GPIO 18, khong phai dau tren hay dau duoi.");
    Serial.println("  4. Neu ba muc tren deu dung ma van im: cam bien da hong.");
    Serial.println("     Nuoc ban vao mat cam bien la nguyen nhan pho bien nhat.");
  } else {
    Serial.println("Co xung tro ve. Cam bien con song.");
  }
  Serial.println("XONG.");
}
void loop() { relayOff(); delay(1000); }
