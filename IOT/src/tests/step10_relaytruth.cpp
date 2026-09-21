// Buoc 10: RO LE DANG THEO CUC TINH NAO — do bang dong dien, khong doan.
//
// Lan luot dua chan dieu khien xuong MUC THAP roi len MUC CAO, va do dong
// dien qua ACS712 o tung muc. Muc nao lam bom AN DIEN thi muc do la BAT.
//
// Bom chi chay 6 giay moi phia. Voi luu luong do duoc 0,36 L/phut thi moi
// phia chi them 0,036 lit vao bon 1 lit — khong the gay tran.
// Ket thuc, chuong trinh dua ro le ve phia IT DONG HON, tuc la NGAT.
#include <Arduino.h>
#include "config.h"

#define SETTLE_MS  2000
#define MEAS_MS    4000
#define ROUNDS     2

float readMv() {
  uint32_t acc = 0;
  for (int i = 0; i < 200; i++) acc += analogRead(PIN_CURRENT);
  return (acc / 200.0f) * 3300.0f / 4095.0f;
}

// Do trung binh trong MEAS_MS, tra ve ca do dao dong
float measure(float* spread) {
  uint32_t t0 = millis();
  double sum = 0; int n = 0; float mn = 1e9, mx = -1e9;
  while (millis() - t0 < MEAS_MS) {
    float v = readMv();
    sum += v; n++;
    if (v < mn) mn = v;
    if (v > mx) mx = v;
    delay(20);
  }
  *spread = mx - mn;
  return (float)(sum / n);
}

float pingCm() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  uint32_t us = pulseIn(PIN_ECHO, HIGH, 30000UL);
  return us ? us * 0.0343f / 2.0f : -1;
}

void setup() {
  // Bat dau o phia ma config DANG COI LA NGAT
  pinMode(PIN_RELAY, OUTPUT);
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, HIGH);
#else
  digitalWrite(PIN_RELAY, LOW);
#endif

  Serial.begin(115200);
  delay(400);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_FLOAT_MAX, INPUT_PULLUP);
  pinMode(PIN_FLOAT_MIN, INPUT_PULLUP);

  Serial.println();
  Serial.println("=== RO LE THEO CUC TINH NAO? — do bang dong dien ===");
  Serial.printf("config dang dat RELAY_ACTIVE_LOW = %d\n", RELAY_ACTIVE_LOW);
#if RELAY_ACTIVE_LOW
  Serial.println("  => config tin rang: chan THAP = bom CHAY, chan CAO = bom NGAT");
#else
  Serial.println("  => config tin rang: chan CAO = bom CHAY, chan THAP = bom NGAT");
#endif
  Serial.println("Bom se chay 6 giay moi phia. Bon 1 lit chi nhan them 0,036 lit.");
  for (int i = 3; i > 0; i--) { Serial.printf("  bat dau sau %d giay...\n", i); delay(1000); }
  Serial.println();

  double sumLow = 0, sumHigh = 0, ripLow = 0, ripHigh = 0;

  for (int r = 1; r <= ROUNDS; r++) {
    float sp;

    digitalWrite(PIN_RELAY, LOW);
    delay(SETTLE_MS);
    float mvLow = measure(&sp);
    Serial.printf("vong %d · chan MUC THAP : %7.2f mV  (dao dong %5.2f mV)\n", r, mvLow, sp);
    sumLow += mvLow; ripLow += sp;

    digitalWrite(PIN_RELAY, HIGH);
    delay(SETTLE_MS);
    float mvHigh = measure(&sp);
    Serial.printf("vong %d · chan MUC CAO  : %7.2f mV  (dao dong %5.2f mV)\n", r, mvHigh, sp);
    sumHigh += mvHigh; ripHigh += sp;
  }

  float lo = sumLow / ROUNDS,  hi = sumHigh / ROUNDS;
  float rl = ripLow / ROUNDS,  rh = ripHigh / ROUNDS;
  float diff = fabs(hi - lo);

  Serial.println();
  Serial.println("=== KET LUAN ===");
  Serial.printf("chan THAP : trung binh %7.2f mV · dao dong %6.2f mV\n", lo, rl);
  Serial.printf("chan CAO  : trung binh %7.2f mV · dao dong %6.2f mV\n", hi, rh);
  Serial.printf("lech trung binh %.2f mV · ti so dao dong %.1f lan\n",
                diff, (rl > rh ? rl / max(rh, 0.01f) : rh / max(rl, 0.01f)));
  Serial.println();

  // ACS712 la cam bien HAI CHIEU: ngo ra nam giua dai va lech LEN HOAC XUONG
  // tuy chieu dong. Vay "mV lon hon" KHONG co nghia la "dong lon hon" —
  // dung no de ket luan la sai.
  //
  // Thu do dung la DO DAO DONG. Dong co cho than chay thi dong gon manh do
  // co gop, con luc ngat thi duong dong phang. Ti so dao dong tach hai trang
  // thai ra rat ro va khong phu thuoc chieu dau day qua IP+ va IP-.
  bool pinHighMeansOn = (rh > rl);
  float ripRatio = (rl > rh ? rl / max(rh, 0.01f) : rh / max(rl, 0.01f));

  if (ripRatio < 2.0f && diff < 5.0f) {
    Serial.println("CHENH LECH QUA NHO — khong ket luan duoc.");
    Serial.println("Nghia la bom KHONG AN DIEN o ca hai phia, hoac ACS712 khong doc duoc.");
    Serial.println("Kiem tra: bom da cam chua, tiep diem ro le co dong khong,");
    Serial.println("day bom co di qua dung hai coc IP+ va IP- cua ACS712 khong.");
  } else {
    Serial.printf("Bom AN DIEN khi chan o MUC %s", pinHighMeansOn ? "CAO" : "THAP");
    Serial.printf("  (dao dong %.2f so voi %.2f mV).\n",
                  pinHighMeansOn ? rh : rl, pinHighMeansOn ? rl : rh);
    int dung = pinHighMeansOn ? 0 : 1;
    Serial.printf("=> RELAY_ACTIVE_LOW phai bang %d.  Config dang la %d.  %s\n",
                  dung, RELAY_ACTIVE_LOW,
                  dung == RELAY_ACTIVE_LOW ? "DUNG ROI." : "<<< DANG SAI, PHAI DOI >>>");
  }

  // Dua ve phia it dong hon = NGAT
  digitalWrite(PIN_RELAY, pinHighMeansOn ? LOW : HIGH);
  Serial.println();
  Serial.printf("Da dua chan ve MUC %s de NGAT bom.\n", pinHighMeansOn ? "THAP" : "CAO");

  float d = pingCm();
  Serial.printf("Sieu am doc duoc     : %.1f cm\n", d);
  Serial.printf("Phao muc cao         : chan %s\n", digitalRead(PIN_FLOAT_MAX) == LOW ? "THAP" : "CAO");
  Serial.printf("Phao muc thap        : chan %s\n", digitalRead(PIN_FLOAT_MIN) == LOW ? "THAP" : "CAO");
  Serial.println("XONG. Chuong trinh dung han, ro le da NGAT.");
}

void loop() { delay(1000); }
