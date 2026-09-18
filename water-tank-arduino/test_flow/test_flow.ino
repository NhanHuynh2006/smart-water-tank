// Buoc 5: dem xung HAI cam bien luu luong. Dung de do he so K trong thi nghiem E2.
//
// DOC KY TRUOC KHI CHAY:
//   Cot "xung/giay" moi la so do THO, khong phu thuoc he so K.
//   Neu bom chua chay ma xung/giay van khac 0 thi day la NHIEU, khong phai nuoc.
//   Nguyen nhan pho bien nhat: vua dung dien tro keo len noi bo VUA qua chia ap.
//   Keo len noi bo cua ESP32 khoang 45 kOhm, mac song song 20 kOhm xuong dat cho
//   dien ap nghi 1,02 V, roi dung vao vung khong xac dinh giua 0,825 V va 2,475 V,
//   nen chan dao dong theo nhieu va sinh hang nghin xung gia moi giay.
//   Xem FLOW_PIN_PULLUP trong config.h.
//
// Gioi han tren hop ly de doi chieu:
//   YF-S201 toi da 30 L/phut x 7.5  =  225 xung/giay
//   YF-S401 toi da  6 L/phut x 98   =  588 xung/giay
//   Vuot han cac so nay thi gan nhu chac chan la nhieu.
#include <Arduino.h>
#include "config.h"

volatile uint32_t pIn = 0, pOut = 0;
uint32_t lastIn = 0, lastOut = 0, totIn = 0, totOut = 0;

void IRAM_ATTR isrIn()  { pIn++; }
void IRAM_ATTR isrOut() { pOut++; }

void setup() {
  Serial.begin(115200);
  delay(400);

#if FLOW_PIN_PULLUP
  pinMode(PIN_FLOW, INPUT_PULLUP);
#else
  pinMode(PIN_FLOW, INPUT);
#endif
#if FLOW_OUT_PIN_PULLUP
  pinMode(PIN_FLOW_OUT, INPUT_PULLUP);
#else
  pinMode(PIN_FLOW_OUT, INPUT);
#endif

  // Doc muc tinh truoc khi bat ngat: giup phan biet nhieu voi tin hieu that.
  delay(50);
  Serial.println();
  Serial.println("=== KIEM THU HAI CAM BIEN LUU LUONG ===");
  Serial.printf("DAU VAO : GPIO %d, keo len noi bo %s, K = %.1f\n",
                PIN_FLOW, FLOW_PIN_PULLUP ? "BAT" : "tat", (double)FLOW_K_FACTOR);
  Serial.printf("DAU RA  : GPIO %d, keo len noi bo %s, K = %.1f\n",
                PIN_FLOW_OUT, FLOW_OUT_PIN_PULLUP ? "BAT" : "tat", (double)FLOW_OUT_K_FACTOR);
  Serial.printf("Muc tinh luc nghi: vao = %d, ra = %d   (mong doi la 1 khi khong co nuoc)\n",
                digitalRead(PIN_FLOW), digitalRead(PIN_FLOW_OUT));
  Serial.println("Neu muc tinh la 0 hoac doc lai thay nhay loan thi day day hoac cau hinh sai.");
  Serial.println();

  attachInterrupt(digitalPinToInterrupt(PIN_FLOW),     isrIn,  FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT), isrOut, FALLING);
}

void loop() {
  noInterrupts();
  uint32_t cIn = pIn, cOut = pOut;
  interrupts();

  uint32_t dIn  = cIn  - lastIn;   lastIn  = cIn;   totIn  += dIn;
  uint32_t dOut = cOut - lastOut;  lastOut = cOut;  totOut += dOut;

  const char* warnIn  = (dIn  > 700) ? "  <-- NGHI NHIEU" : "";
  const char* warnOut = (dOut > 700) ? "  <-- NGHI NHIEU" : "";

  Serial.printf("VAO  %5lu xung/s | %7.2f L/phut | tong %8lu%s\n",
                dIn,  dIn / (float)FLOW_K_FACTOR, totIn, warnIn);
  Serial.printf("RA   %5lu xung/s | %7.2f L/phut | tong %8lu%s\n",
                dOut, dOut / (float)FLOW_OUT_K_FACTOR, totOut, warnOut);
  Serial.println("---");
  delay(1000);
}
