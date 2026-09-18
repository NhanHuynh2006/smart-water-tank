// Buoc 6: do cam bien dong ACS712.
//
// Chuong trinh tu dong cat bom 4 giay roi chay bom 4 giay, do ca hai trang thai
// va in ra HIEU giua chung. Hieu do moi la thu quyet dinh, khong phai gia tri
// tuyet doi. Gia tri tuyet doi chi la diem nghi cua cam bien, khoang 1,25 V sau
// chia ap 10k/10k, va no troi theo tung con chip nen khong noi len dieu gi.
//
// LUU Y VE DON VI: ACS712 la cam bien ngo ra DIEN AP. No khong xuat ra ampe.
// Chuong trinh in mV vi day la so do THO tai chan ESP32, dung de hieu chuan.
// Doi sang dong dien bang cong thuc o cuoi moi vong in.
//
// SO LIEU CHO BOM JT-DC3L (3~5 V, 100~200 mA) qua chia ap 10k/10k:
//    ACS712-5A   185 mV/A -> 200 mA cho 18,5 mV tai chan ESP32
//    ACS712-20A  100 mV/A -> 200 mA cho 10,0 mV
//    ACS712-30A   66 mV/A -> 200 mA cho  6,6 mV
// Bom nay rat nho so voi dai do cua ACS712. Chi loai 5A moi dung duoc.
#include <Arduino.h>
#include "config.h"

// Do nhay cua mo dun ban dang dung, mV tren moi ampe. Sua cho dung loai.
#define ACS_SENS_MV_PER_A   185.0f    // 5A = 185 | 20A = 100 | 30A = 66
#define DIVIDER_RATIO       2.0f      // chia ap 10k/10k thi bang 2, khong chia thi 1

void setRelayOff() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif
}
void setRelayOn() {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, LOW);
#else
  digitalWrite(PIN_RELAY, HIGH);
#endif
}

float readMv() {
  uint32_t acc = 0;
  for (int i = 0; i < 200; i++) { acc += analogRead(PIN_CURRENT); delayMicroseconds(200); }
  return (acc / 200.0f) * 3300.0f / 4095.0f;
}

void setup() {
  Serial.begin(115200);
  delay(400);
  analogReadResolution(12);
  pinMode(PIN_RELAY, OUTPUT);
  setRelayOff();
  Serial.println();
  Serial.println("=== KIEM THU CAM BIEN DONG ACS712 ===");
  Serial.printf("Do nhay dang khai bao : %.0f mV/A\n", (double)ACS_SENS_MV_PER_A);
  Serial.printf("He so chia ap         : %.0f\n", (double)DIVIDER_RATIO);
  Serial.printf("Nguong trong config   : %.1f mV\n", (double)CURRENT_ON_MV);
  Serial.println("Chu ky: 4 giay TAT bom, 4 giay CHAY bom. Ctrl+C de dung.");
  Serial.println();
}

void loop() {
  setRelayOff();
  delay(3000);
  float off = readMv();

  setRelayOn();
  delay(3000);
  float on = readMv();
  setRelayOff();

  float diff = fabs(on - off);
  float amps = diff * DIVIDER_RATIO / ACS_SENS_MV_PER_A;

  Serial.printf("bom TAT  : %8.2f mV   (diem nghi cua cam bien)\n", off);
  Serial.printf("bom CHAY : %8.2f mV\n", on);
  Serial.printf("HIEU     : %8.2f mV   -> %.3f A = %.0f mA\n", diff, amps, amps * 1000);

  if (diff < 3.0f) {
    Serial.println("  => HIEU GAN BANG KHONG. Cam bien khong thay dong nao.");
    Serial.println("     Kiem tra ba thu, theo thu tu:");
    Serial.println("     1. Bom co thuc su chay khong, nghe tieng hoac so vao");
    Serial.println("     2. ACS712 co mac NOI TIEP tren day cap bom khong,");
    Serial.println("        dong phai chay xuyen qua IP+ va IP-, khong mac song song");
    Serial.println("     3. Chan OUT cua ACS712 co that su noi toi GPIO 34 khong");
  } else if (diff < CURRENT_ON_MV) {
    Serial.printf("  => Co thay dong nhung HIEU %.1f mV NHO HON nguong %.1f mV.\n",
                  diff, (double)CURRENT_ON_MV);
    Serial.println("     Luat NO_CURRENT se bao loi nham moi lan bom chay.");
    Serial.printf("     Dat CURRENT_ON_MV khoang %.1f mV, tuc mot nua cua hieu.\n", diff / 2);
    Serial.println("     Neu hieu qua nho thi mo dun ACS712 sai dai do cho bom nay.");
  } else {
    Serial.printf("  => DAT. Hieu %.1f mV vuot nguong %.1f mV, bien an toan %.0f phan tram.\n",
                  diff, (double)CURRENT_ON_MV, (diff / CURRENT_ON_MV - 1) * 100);
    Serial.printf("     Nen dat CURRENT_ON_MV = %.1f mV cho can giua.\n", diff / 2);
  }

  if (off < 900 || off > 1700) {
    Serial.printf("  CANH BAO: diem nghi %.0f mV nam ngoai dai mong doi 1100..1400 mV.\n", off);
    Serial.println("     Co chia ap 10k/10k va ACS712 cap 5 V thi phai ra khoang 1250 mV.");
    Serial.println("     Lech nhieu thi hoac thieu chia ap, hoac ACS712 chua duoc cap nguon.");
  }
  Serial.println("---");
}
