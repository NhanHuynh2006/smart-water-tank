// Buoc 7: hai phao — va KIEM CHUNG CUC TINH.
//
// Chuong trinh nay khong bao gio bat bom. Ro le bi giu NGAT suot.
//
// CACH DUNG: nap xong, NHAC TAY phao muc cao len het co.
//   - Neu dong "phao muc cao" doi tu NGHI sang DA KICH HOAT  -> cuc tinh DUNG.
//   - Neu no doi nguoc lai, hoac khong doi gi -> sua FLOAT_MAX_ACTIVE_LOW
//     trong config.h (dang la 0, doi thanh 1, hoac nguoc lai) roi nap lai.
//
// Cuc tinh sai o day la ly do bom khong chiu dung: phan mem tuong bon con
// rong trong khi nuoc da len toi mieng.
#include <Arduino.h>
#include "config.h"

#define RUN_MS   60000UL

void relayOff() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
}

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();                        // bom NGAT truoc moi thu khac

  Serial.begin(115200);
  delay(400);
  pinMode(PIN_FLOAT_MAX, INPUT_PULLUP);
  pinMode(PIN_FLOAT_SRC, INPUT_PULLUP);

  Serial.println();
  Serial.println("=== KIEM THU HAI PHAO — BOM LUON NGAT ===");
  Serial.printf("config: FLOAT_MAX_ACTIVE_LOW = %d, FLOAT_SRC_ACTIVE_LOW = %d\n",
                FLOAT_MAX_ACTIVE_LOW, FLOAT_SRC_ACTIVE_LOW);
  Serial.println("HAY NHAC TAY PHAO MUC CAO LEN va xem dong duoi doi the nao.");
  Serial.println("Chay 60 giay roi dung han.");
  Serial.println();

  uint32_t t0 = millis();
  int lastMax = -1, lastSrc = -1;

  while (millis() - t0 < RUN_MS) {
    relayOff();
    int rawMax = digitalRead(PIN_FLOAT_MAX);
    int rawSrc = digitalRead(PIN_FLOAT_SRC);

#if FLOAT_MAX_ACTIVE_LOW
    bool actMax = (rawMax == LOW);
#else
    bool actMax = (rawMax == HIGH);
#endif
#if FLOAT_SRC_ACTIVE_LOW
    bool actSrc = (rawSrc == LOW);
#else
    bool actSrc = (rawSrc == HIGH);
#endif

    if (rawMax != lastMax || rawSrc != lastSrc) {
      lastMax = rawMax; lastSrc = rawSrc;
      Serial.printf("[%5lus] phao muc cao: chan %s -> %-14s | "
                    "phao bon nguon: chan %s -> %s\n",
                    (millis() - t0) / 1000,
                    rawMax == LOW ? "THAP" : "CAO ",
                    actMax ? "DA KICH HOAT" : "nghi",
                    rawSrc == LOW ? "THAP" : "CAO ",
                    actSrc ? "con nuoc" : "CAN NUOC");
    }
    delay(100);
  }

  relayOff();
  Serial.println();
  Serial.println("=== Y NGHIA ===");
  Serial.println("phao muc cao DA KICH HOAT  -> bom bi chan, khong the chay.");
  Serial.println("phao bon nguon CAN NUOC    -> bom bi chan, tranh chay kho.");
  Serial.println("Neu nhac phao len ma dong chu khong doi dung nhu mo ta,");
  Serial.println("hay doi FLOAT_MAX_ACTIVE_LOW trong config.h roi nap lai.");
  Serial.println("XONG. Chuong trinh dung han, ro le van NGAT.");
}

void loop() { relayOff(); delay(1000); }
