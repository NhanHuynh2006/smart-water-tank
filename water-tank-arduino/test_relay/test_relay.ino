// Buoc 3: dong cat ro le.
//
// AN TOAN: chuong trinh nay chay SO LAN CO HAN roi DUNG HAN voi ro le NGAT.
// Ban cu lap vo han, va no da gay su co that: de chay 4,9 phut trong luc
// lam viec khac, bom day 3,6 lit vao bon 10 lit va suyt tran.
// Moi chuong trinh kiem thu co dong cat bom deu phai co diem dung.
#include <Arduino.h>
#include "config.h"

#define CYCLES      5      // so lan dong cat roi dung han
#define ON_MS    1500UL
#define OFF_MS   2000UL

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

void setup() {
  // Dua ro le ve NGAT truoc moi thu khac, ke ca truoc Serial.begin.
  pinMode(PIN_RELAY, OUTPUT);
  relayOff();

  Serial.begin(115200);
  delay(400);
  Serial.println();
  Serial.println("=== KIEM THU RO LE ===");
  Serial.printf("Se dong cat %d lan, moi lan bat %lu ms, roi DUNG HAN.\n",
                CYCLES, ON_MS);
  Serial.println("NEU BOM DANG NOI, NO SE CHAY. Rut bom ra neu chua muon.");
  for (int i = 5; i > 0; i--) { Serial.printf("  bat dau sau %d giay...\n", i); delay(1000); }

  for (int i = 1; i <= CYCLES; i++) {
    relayOn();
    Serial.printf("lan %d/%d: ro le DONG  (bom chay)\n", i, CYCLES);
    delay(ON_MS);
    relayOff();
    Serial.printf("lan %d/%d: ro le NGAT\n", i, CYCLES);
    delay(OFF_MS);
  }

  relayOff();
  Serial.println();
  Serial.println("XONG. Ro le da NGAT va chuong trinh dung han.");
  Serial.println("Nghe tieng tach moi lan dong cat la ro le hoat dong.");
}

void loop() {
  relayOff();     // giu ngat, khong lam gi them
  delay(1000);
}
