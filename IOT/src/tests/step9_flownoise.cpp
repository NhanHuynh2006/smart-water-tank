// Buoc 9: chan cam bien luu luong CO DUOC NOI DAY khong?
//
// Phep thu: keo chan len 3,3 V roi keo xuong dat, va xem chan co ngoan ngoan
// di theo khong.
//   - Day THA NOI          : chan di theo ca hai chieu  -> CHUA NOI DAY
//   - Cam bien NOI DUNG    : chan bi cam bien giu chat mot ben, khong di theo
// Ro le giu NGAT suot, chuong trinh tu dung han.
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

volatile uint32_t cIn = 0, cOut = 0;
void IRAM_ATTR isrIn()  { cIn++; }
void IRAM_ATTR isrOut() { cOut++; }

void relayOff() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
}

// Doc 200 lan trong 100 ms, dem bao nhieu lan o muc cao
int highRatio(int pin) {
  int h = 0;
  for (int i = 0; i < 200; i++) { if (digitalRead(pin) == HIGH) h++; delayMicroseconds(500); }
  return h / 2;   // phan tram
}

void probe(const char* nhan, int pin) {
  pinMode(pin, INPUT_PULLUP);   delay(50); int up   = highRatio(pin);
  pinMode(pin, INPUT_PULLDOWN); delay(50); int down = highRatio(pin);
  pinMode(pin, INPUT_PULLUP);

  const char* ketluan;
  if (up > 90 && down < 10)       ketluan = "THA NOI — CHUA NOI DAY (chan di theo ca hai chieu)";
  else if (up > 90 && down > 90)  ketluan = "bi giu o MUC CAO — co nguon ngoai giu, co ve da noi";
  else if (up < 10 && down < 10)  ketluan = "bi giu o MUC THAP — cam bien dang keo xuong, da noi";
  else                            ketluan = "DAO LIEN TUC — co tin hieu hoac nhieu manh";
  Serial.printf("%-20s keo len: %3d%% cao | keo xuong: %3d%% cao  ->  %s\n",
                nhan, up, down, ketluan);
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();
  Serial.begin(115200);
  delay(400);
  WiFi.mode(WIFI_OFF); delay(300);

  Serial.println();
  Serial.println("=== CHAN LUU LUONG DA NOI DAY CHUA? (Wi-Fi tat) ===");
  probe("dau vao  GPIO 4", PIN_FLOW);
  probe("dau ra   GPIO 19", PIN_FLOW_OUT);

  Serial.println();
  Serial.println("=== DEM XUNG HAI CHAN CUNG LUC, 3 giay, Wi-Fi tat ===");
#if FLOW_PIN_PULLUP
  pinMode(PIN_FLOW, INPUT_PULLUP); pinMode(PIN_FLOW_OUT, INPUT_PULLUP);
#else
  pinMode(PIN_FLOW, INPUT);        pinMode(PIN_FLOW_OUT, INPUT);
#endif
  cIn = cOut = 0;
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW),     isrIn,  FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT), isrOut, FALLING);
  delay(3000);
  detachInterrupt(digitalPinToInterrupt(PIN_FLOW));
  detachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT));
  Serial.printf("dau vao : %8lu xung (%7.0f Hz)\n", (unsigned long)cIn,  cIn  / 3.0f);
  Serial.printf("dau ra  : %8lu xung (%7.0f Hz)\n", (unsigned long)cOut, cOut / 3.0f);

  Serial.println();
  Serial.println("=== DEM LAI VOI Wi-Fi BAT ===");
  WiFi.mode(WIFI_STA); WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) delay(250);
  Serial.printf("Wi-Fi: %s, RSSI %d dBm\n",
                WiFi.status() == WL_CONNECTED ? "da noi" : "KHONG NOI DUOC", WiFi.RSSI());
  cIn = cOut = 0;
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW),     isrIn,  FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT), isrOut, FALLING);
  delay(3000);
  detachInterrupt(digitalPinToInterrupt(PIN_FLOW));
  detachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT));
  Serial.printf("dau vao : %8lu xung (%7.0f Hz)\n", (unsigned long)cIn,  cIn  / 3.0f);
  Serial.printf("dau ra  : %8lu xung (%7.0f Hz)\n", (unsigned long)cOut, cOut / 3.0f);

  relayOff();
  Serial.println();
  Serial.println("Khong co nuoc chay, nen MOI xung dem duoc o tren deu la NHIEU.");
  Serial.println("XONG. Chuong trinh dung han.");
}

void loop() { relayOff(); delay(1000); }
