// Buoc 9: chan luu luong dang bi CAI GI dieu khien?
//
// Do tan so o BA kieu cau hinh chan. Ket qua phan biet duoc ba truong hop:
//
//   Cam bien noi dung, dung yen : 0 Hz o CA BA kieu.
//                                 (ngo ra hut xuong kieu cuc thu ho, khi
//                                  khong quay thi no giu yen mot muc)
//   Day THA NOI                 : tan so DOI NHIEU theo tung kieu.
//   Co tin hieu that dang danh  : tan so GIONG NHAU o ca ba kieu.
//
// Ro le giu NGAT suot, chuong trinh tu dung han.
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

volatile uint32_t cnt = 0;
void IRAM_ATTR isr() { cnt++; }

void relayOff() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
}

float hz(int pin, int mode, uint32_t ms) {
  pinMode(pin, mode);
  delay(60);
  cnt = 0;
  attachInterrupt(digitalPinToInterrupt(pin), isr, FALLING);
  delay(ms);
  detachInterrupt(digitalPinToInterrupt(pin));
  return cnt * 1000.0f / ms;
}

int pctHigh(int pin, int mode) {
  pinMode(pin, mode); delay(60);
  int h = 0;
  for (int i = 0; i < 200; i++) { if (digitalRead(pin) == HIGH) h++; delayMicroseconds(500); }
  return h / 2;
}

void khaosat(const char* nhan, int pin) {
  float hUp   = hz(pin, INPUT_PULLUP,   1500);
  float hNone = hz(pin, INPUT,          1500);
  float hDown = hz(pin, INPUT_PULLDOWN, 1500);
  int   pUp   = pctHigh(pin, INPUT_PULLUP);
  int   pDown = pctHigh(pin, INPUT_PULLDOWN);

  Serial.printf("\n%s\n", nhan);
  Serial.printf("  keo len 3,3 V : %8.0f Hz   (%3d%% thoi gian o muc cao)\n", hUp,   pUp);
  Serial.printf("  khong keo     : %8.0f Hz\n", hNone);
  Serial.printf("  keo xuong dat : %8.0f Hz   (%3d%% thoi gian o muc cao)\n", hDown, pDown);

  float mx = max(hUp, max(hNone, hDown));
  if (mx < 5) {
    Serial.println("  => IM LANG o ca ba kieu. Day duoc giu chat. DUNG.");
  } else {
    float mn = min(hUp, min(hNone, hDown));
    if (mn > 0 && mx / mn < 1.5f)
      Serial.printf("  => Tan so GIONG NHAU o ca ba kieu (%.0f Hz). Co nguon that dang danh vao chan.\n", mx);
    else
      Serial.println("  => Tan so DOI THEO tung kieu. Day KHONG duoc giu chat: THA NOI.");
    if (mx > FLOW_MAX_PLAUSIBLE_HZ)
      Serial.printf("  => %.0f Hz vuot xa gioi han vat ly %.0f Hz cua cam bien. Day la NHIEU.\n",
                    mx, (float)FLOW_MAX_PLAUSIBLE_HZ);
  }
  pinMode(pin, INPUT_PULLUP);
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();
  Serial.begin(115200);
  delay(400);
  WiFi.mode(WIFI_OFF); delay(300);

  Serial.println();
  Serial.println("=== CHAN LUU LUONG DANG BI CAI GI DIEU KHIEN? ===");
  Serial.println("Wi-Fi tat, bom ngat, khong co nuoc chay.");
  Serial.printf("Gioi han vat ly cua cam bien: %.0f Hz\n", (float)FLOW_MAX_PLAUSIBLE_HZ);

  khaosat("dau vao   GPIO 4", PIN_FLOW);
  khaosat("dau ra    GPIO 19", PIN_FLOW_OUT);

  // Doi chieu: mot chan chac chan KHONG noi gi, de biet the nao la tha noi
  Serial.println();
  Serial.println("--- DOI CHIEU: GPIO 23 (khong noi gi trong so do) ---");
  khaosat("chan trong GPIO 23", 23);

  relayOff();
  Serial.println();
  Serial.println("So sanh hai chan luu luong voi chan trong GPIO 23:");
  Serial.println("giong nhau -> chua noi day. Khac han -> da noi, va la nhieu tu nguon khac.");
  Serial.println("XONG. Chuong trinh dung han.");
}

void loop() { relayOff(); delay(1000); }
