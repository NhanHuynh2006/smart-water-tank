// ============================================================
//  config.h  -  DAY LA FILE DUY NHAT BAN CAN SUA
//  Sua wifi, dia chi broker va cac nguong o day roi nap lai.
// ============================================================
#pragma once

// ---------- Wi-Fi ----------
#define WIFI_SSID       "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD   "MAT_KHAU_WIFI"

// ---------- MQTT broker ----------
#define MQTT_HOST       "192.168.1.10"   // IP may chay mosquitto
#define MQTT_PORT       1883
#define MQTT_USER       "device1"
#define MQTT_PASS       "device_pass"

// ---------- Dinh danh thiet bi ----------
#define SITE_ID         "lab1"
#define DEVICE_ID       "esp32-01"

// ---------- Chan GPIO ----------
#define PIN_TRIG        5     // ultrasonic trigger
#define PIN_ECHO        18    // ultrasonic echo (qua chia ap 10k/20k)
#define PIN_FLOW        4     // xung cam bien luu luong DAU VAO (bom -> bon)
#define PIN_FLOW_OUT    19    // xung cam bien luu luong DAU RA  (bon -> tieu thu)
#define PIN_RELAY       26    // relay dieu khien bom
#define PIN_CURRENT     34    // ACS712 (qua chia ap 10k/10k), chan chi vao
#define PIN_FLOAT_MAX   27    // phao muc cao, INPUT_PULLUP
#define PIN_FLOAT_SRC   14    // phao bon nguon, INPUT_PULLUP
#define PIN_LED_OK      2
#define PIN_LED_FAULT   25
#define PIN_BTN_RESET   33    // nut xoa loi, INPUT_PULLUP

#define RELAY_ACTIVE_LOW  1   // dat 0 neu module relay kich muc cao

// ---------- Cuc tinh hai phao ----------
// Ca hai chan deu bat keo len noi bo, nen chan HO doc ra muc CAO.
// Dat 1 = tiep diem DONG (chan bi keo xuong dat) co nghia la DA KICH HOAT.
// Dat 0 = tiep diem HO (chan o muc cao) co nghia la DA KICH HOAT.
//
// DO THAT TREN MACH NGAY 21/09: ca hai chan doc ra MUC THAP khi bon CHUA day.
// Vay muc thap la trang thai NGHI, va phao muc cao phai dat 0.
// Cach nay con an toan hon: dut day thi chan len muc cao, thanh "bon day",
// va bom bi chan — hong theo huong an toan.
//
// KIEM CHUNG LAI bang: pio run -e test_floats -t upload -t monitor
// Nhan tay phao muc cao len, dong chu phai doi thanh "DA KICH HOAT".
#define FLOAT_MAX_ACTIVE_LOW  0
#define FLOAT_SRC_ACTIVE_LOW  1

// ---------- Hinh hoc bon ----------
#define TANK_SENSOR_TO_BOTTOM_CM   55.0f
#define TANK_MAX_LEVEL_CM          25.0f
#define TANK_AREA_CM2             400.0f

// ---------- Hieu chuan (lay tu thi nghiem E1 va E2) ----------
#define LEVEL_CAL_A     1.0f
#define LEVEL_CAL_B     0.0f
// He so K = so xung moi giay tren moi lit/phut. KHAC NHAU THEO LOAI CAM BIEN:
//    YF-S201  (ren 1/2", 1-30 L/phut) : 7.5
//    YF-S401  (ren 1/4", 0.3-6 L/phut): 98.0
//    YF-B10   (ren 1/2", 1-25 L/phut) : 6.6
// Dat sai he so nay thi so doc lech dung bang ti so hai he so.
// Gia tri duoi day PHAI duoc kiem chung lai bang thi nghiem E2.
#define FLOW_K_FACTOR       98.0f   // cam bien DAU VAO
#define FLOW_OUT_K_FACTOR   98.0f   // cam bien DAU RA, doi neu hai con khac loai

// Chan xung cam bien luu luong noi the nao.
//    1 = noi THANG vao GPIO, dung dien tro keo len NOI BO cua ESP32 len 3,3 V.
//        Dung cho cam biến hall ngo ra cuc thu ho, KHONG co chia ap.
//    0 = qua CHIA AP 10k/20k tu 5 V, chan de o che do INPUT thuan.
//        Dung khi mo dun da co san dien tro keo len VCC 5 V.
//
// KHONG BAO GIO dung dong thoi keo len noi bo VA chia ap: dien tro keo len
// cua ESP32 khoang 45 kOhm, mac song song voi 20 kOhm xuong dat cho dien ap
// nghi 3,3 x 20/(45+20) = 1,02 V. Nguong vao cua ESP32 la 0,825 V va 2,475 V,
// nen 1,02 V roi vao VUNG KHONG XAC DINH va chan se dao dong theo nhieu,
// sinh ra hang nghin xung gia moi giay.
#define FLOW_PIN_PULLUP       1   // cho chan PIN_FLOW
#define FLOW_OUT_PIN_PULLUP   1   // cho chan PIN_FLOW_OUT

// Khoang cach toi thieu giua hai xung duoc dem, tinh bang micro giay.
// YF-S401 toi da 6 L/phut x 98 = 588 Hz -> moi xung cach nhau >= 1700 us.
// Dat 1200 us cho tran 833 Hz: con du 40 phan tram bien so voi cam bien
// that, nhung cat bot phan lon nhieu do Wi-Fi da do duoc o 1600-2700 Hz.
#define FLOW_MIN_PULSE_US   1200UL

// ---------- Tham so dieu khien ----------
#define LEVEL_LOW_PCT       30.0f
// Ha tu 80 xuong 70 va tu 95 xuong 85 de mat nuoc dung xa mat cam bien hon.
// Bon cao 25 cm: dung o 70% la nuoc cao 17,5 cm, con cach cam bien 37,5 cm;
// khoa chong tran o 85% la nuoc cao 21,3 cm, con cach cam bien 33,7 cm.
// Truoc day dung o 80% va khoa o 95% thi chi con cach 32,5 va 26,3 cm,
// qua sat vung mu 2 cm cua HC-SR04 va qua sat mieng bon.
#define LEVEL_HIGH_PCT      70.0f
#define LEVEL_OVERFLOW_PCT  85.0f

// Chan an toan doc thang tu khoang cach tho, KHONG qua bo loc nao.
// Cam bien doc gan hon so nay la nuoc da len qua cao: NGAT BOM NGAY.
// Doc lap hoan toan voi levelOk, voi trung vi va voi phao.
#define LEVEL_MIN_DISTANCE_CM  32.0f
#define MIN_ON_MS           10000UL
#define MIN_OFF_MS          20000UL
// Thoi gian bom toi da truoc khi ket luan bat thuong.
// TINH LAI THEO BOM THAT (JT-DC3L, 100 L/gio = 1,67 L/phut o cot nuoc bang 0):
//    the tich bon      = 400 cm2 x 25 cm = 10 L
//    bom tu 30% len 80% = 5 L
//    o luu luong TOI DA  : 5 / 1,67 x 60 = 180 s  <- dung bang gia tri cu
//    o luu luong thuc te : bom co cot nuoc nen cham hon, khoang 300 s
// Gia tri cu 180 s se bao FILL_TIMEOUT ngay trong lan bom binh thuong.
// Dat 600 s = khoang hai lan thoi gian day thuc te. PHAI do lai bang thi
// nghiem E3 roi chinh cho khop bom cua ban.
#define MAX_FILL_MS         240000UL

// ---------- Ba chan an toan KHONG phu thuoc cam bien sieu am ----------
// Bon 10 lit. Bom them qua so nay trong MOT lan bom la chac chan co van de:
// hoac cam bien muc sai, hoac nuoc dang chay di dau do.
#define MAX_FILL_VOLUME_L   12.0f

// Bom chay ma muc nuoc khong nhich len duoc NO_PROGRESS_CM trong
// NO_PROGRESS_MS thi ngat. Bom that day 1,67 L/phut vao tiet dien 400 cm2
// tuc 0,069 cm/s, nen trong 60 s phai len it nhat 4,1 cm. Lay 1,5 cm la
// rong gap gan ba lan, du cho bom yeu hay cot nuoc cao.
#define NO_PROGRESS_MS      60000UL
#define NO_PROGRESS_CM      1.5f

// Tran cuoi cung. Khong dieu kien, khong ngoai le, khong tu phuc hoi.
// Bom khong duoc phep chay lien tuc lau hon so nay du bat ky ly do gi.
#define PUMP_HARD_LIMIT_MS  300000UL

// ---------- Nguong phat hien su co ----------
#define DRYRUN_MS           6000UL
#define DRYRUN_FLOW_LPM     0.25f
#define NOCURRENT_MS        2000UL
// Nguong nhan biet bom dang co dong, do tai CHAN ESP32 sau chia ap 10k/10k.
// TINH THEO BOM THAT (JT-DC3L, 100~200 mA):
//    ACS712-5A   185 mV/A -> 200 mA cho 37,0 mV -> sau chia ap 18,5 mV
//    ACS712-20A  100 mV/A -> 200 mA cho 20,0 mV -> sau chia ap 10,0 mV
//    ACS712-30A   66 mV/A -> 200 mA cho 13,2 mV -> sau chia ap  6,6 mV
// CHI loai 5A moi vuot nguong nay, va bien an toan chi con 23 phan tram.
// Dung loai 20A hay 30A thi luat NO_CURRENT khong bao gio phat hien duoc.
// Chay test_current de do so that roi dat nguong bang khoang MOT NUA
// hieu giua luc bom chay va luc bom tat.
// Do tren mach that: bom chay cho hieu 19,6 den 22,6 mV so voi diem nghi.
// Dat 10 mV la khoang mot nua, du xa nhieu nen ma van bat duoc bom chay.
#define CURRENT_ON_MV       10.0f

// Toc do bam theo diem nghi khi bom dang tat. 0,02 voi chu ky 200 ms cho
// hang so thoi gian khoang 10 giay: du nhanh de theo kip troi nhiet, du
// cham de khong bi mot xung nhieu keo di.
#define CURRENT_OFFSET_ALPHA  0.02f

// Cho bao lau sau khi cap nguon moi do diem nghi lan dau, tinh bang ms.
#define CURRENT_SETTLE_MS     1500UL
#define LEAK_MS             60000UL
#define LEAK_FLOW_LPM       0.20f
#define SENSOR_TIMEOUT_MS   4000UL
#define SENSOR_RECOVER_MS   5000UL
#define CONFLICT_LEVEL_PCT  70.0f
// Toc do doi muc nuoc toi da coi la co the ve mat vat ly, cm moi giay.
// TINH THEO BOM THAT: 1,67 L/phut / 400 cm2 = 0,069 cm/s khi bom.
// Xa nhanh qua voi mo cung chi khoang 0,2 cm/s.
// Gia tri cu 12 cm/s rong gap 170 lan thuc te nen gan nhu khong loc duoc gi.
// Dat 2 cm/s: van rong gap 10 lan truong hop nhanh nhat, nhung chan duoc
// phan lon xung nhieu cua cam bien sieu am.
#define MAX_LEVEL_RATE_CMS  2.0f

// ---------- Chu ky ----------
#define CONTROL_PERIOD_MS   200UL
// Khoang cach toi thieu giua hai lan phat sieu am. Datasheet HC-SR04 khuyen
// tren 60 ms de tieng vong cua lan truoc kip tat han. Ban cu phat 5 lan lien
// tiep cach nhau 6 ms nen lan sau bat phai tieng vong cua lan truoc.
// Nay moi chu ky dieu khien chi phat MOT lan, va lay trung vi truot 5 mau.
#define LEVEL_MEDIAN_WINDOW 5
// Bien ngoai dai hinh hoc con chap nhan, tinh bang cm. So doc nam ngoai
// [55-25-8 , 55+8] = [22 , 63] cm bi loai truoc khi vao cua so trung vi.
#define LEVEL_GATE_MARGIN_CM   8.0f
// Bao nhieu lan phat hong LIEN TIEP thi coi la mat cam bien va xoa cua so.
// 5 lan x chu ky 200 ms = 1 giay, van con thua truoc SENSOR_TIMEOUT_MS = 4 s.
#define LEVEL_FAIL_STREAK_MAX  5
#define TELEMETRY_PERIOD_MS 1000UL
#define NVS_SAVE_PERIOD_MS  60000UL
#define OFFLINE_BUFFER_SIZE 240

// ---------- Ket noi lai ----------
#define RECONNECT_BASE_MS   1000UL
#define RECONNECT_MAX_MS    30000UL
#define MQTT_KEEPALIVE_S    15
