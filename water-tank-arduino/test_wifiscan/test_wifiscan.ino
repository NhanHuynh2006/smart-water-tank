// Buoc 13: ESP32 nhin thay nhung mang Wi-Fi nao?
// ESP32 CHI bat duoc bang 2,4 GHz. Mang 5 GHz no khong thay, du dien thoai
// va may tinh deu thay binh thuong.
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
void setup() {
  pinMode(PIN_RELAY, OUTPUT);
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
  Serial.begin(115200); delay(400);
  WiFi.mode(WIFI_STA); WiFi.disconnect(); delay(200);
  Serial.println();
  Serial.printf("=== QUET Wi-Fi · dang tim '%s' ===\n", WIFI_SSID);
  int n = WiFi.scanNetworks();
  Serial.printf("thay %d mang (chi bang 2,4 GHz):\n", n);
  bool found = false;
  for (int i = 0; i < n; i++) {
    bool hit = (WiFi.SSID(i) == String(WIFI_SSID));
    if (hit) found = true;
    Serial.printf("  %-28s  %4d dBm  kenh %2d  %s%s\n",
                  WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i),
                  WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "mo" : "co mat khau",
                  hit ? "   <<< MANG CAN TIM" : "");
  }
  Serial.println();
  if (!found) {
    Serial.printf("KHONG THAY '%s'. Ba kha nang:\n", WIFI_SSID);
    Serial.println("  1. Diem phat dang tat");
    Serial.println("  2. Diem phat dang o bang 5 GHz — ESP32 khong bat duoc");
    Serial.println("  3. Ten mang viet khac (phan biet chu hoa chu thuong)");
  } else {
    Serial.println("Thay mang. Neu van khong noi duoc thi la sai mat khau.");
    Serial.println("Thu ket noi 15 giay...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    for (int i = 0; i < 60 && WiFi.status() != WL_CONNECTED; i++) delay(250);
    if (WiFi.status() == WL_CONNECTED)
      Serial.printf("  THANH CONG · IP %s · RSSI %d dBm\n",
                    WiFi.localIP().toString().c_str(), WiFi.RSSI());
    else
      Serial.printf("  THAT BAI · ma %d — gan nhu chac chan la SAI MAT KHAU\n",
                    WiFi.status());
  }
  Serial.println("XONG.");
}
void loop() { delay(1000); }
