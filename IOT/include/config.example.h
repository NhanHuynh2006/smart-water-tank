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
// DA DAO NGAY 21/09. Bang chung: che do thu cong, bom TAT, ong xa LUON MO.
//    GPIO 4  doc 0,26-0,36 L/phut   <- co dong chay
//    GPIO 19 doc 0,00                <- khong co dong chay
// Bom dang tat thi duong VAO bat buoc phai bang 0. No khong bang 0, con
// duong duoc goi la RA thi lai bang 0 dung luc nuoc dang xa that.
// Vay GPIO 4 la cam bien DAU RA, va GPIO 19 la cam bien DAU VAO.
#define PIN_FLOW       19     // xung cam bien luu luong DAU VAO (bom -> bon)
#define PIN_FLOW_OUT    4     // xung cam bien luu luong DAU RA  (bon -> tieu thu)
#define PIN_RELAY       26    // relay dieu khien bom
#define PIN_CURRENT     34    // ACS712 (qua chia ap 10k/10k), chan chi vao
#define PIN_FLOAT_MAX   27    // phao muc cao, INPUT_PULLUP
#define PIN_FLOAT_MIN   14    // phao MUC THAP tren bon chua, INPUT_PULLUP
#define PIN_LED_OK      2
#define PIN_LED_FAULT   25
#define PIN_BTN_RESET   33    // nut xoa loi, INPUT_PULLUP

// DO THAT NGAY 21/09: dat 1, firmware dua chan len MUC CAO de NGAT bom,
// nhung bom VAN CHAY. Muc nuoc dang tu 0,2 cm len 11,2 cm trong khi web bao
// "bom tat". Vay module nay KICH MUC CAO, phai dat 0.
//
// Day la ly do that su cua viec bom khong chiu dung: khong phai phan mem
// quen ngat, ma la moi lan phan mem ra lenh NGAT thi phan cung lai BAT.
#define RELAY_ACTIVE_LOW  0

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
// CA HAI PHAO DEU NAM TREN BON CHUA. Bon nguon khong co cam bien nao ca,
// nen phan mem KHONG CACH NAO biet bon nguon con nuoc hay khong, va khong
// duoc phep vin vao do de chan bom. Chay kho duoc bat bang ba luat khac:
// DRY_RUN (co lenh bom ma khong co dong chay), NO_CURRENT (khong co dong
// dien) va NO_PROGRESS (bom chay ma muc khong len).
//
//   phao MUC CAO  (GPIO 27): nuoc cham toi vach cao  -> NGAT bom
//   phao MUC THAP (GPIO 14): nuoc tut duoi vach thap -> CHO PHEP bat bom
//
// DO THAT NGAY 21/09, luc bon gan can:
//   GPIO 27 doc MUC THAP  -> phao muc cao chua cham -> dat ACTIVE_LOW = 0
//   GPIO 14 doc MUC CAO   -> phao muc thap da cham  -> dat ACTIVE_LOW = 0
// Hai phao cung kieu, cung dau day, nen cung mot gia tri la hop ly.
//
// KIEM CHUNG LAI: pio run -e test_floats -t upload -t monitor
#define FLOAT_MAX_ACTIVE_LOW  0
#define FLOAT_MIN_ACTIVE_LOW  0

// ---------- Hinh hoc bon ----------
// DO THAT NGAY 21/09: mat cam bien cach day thung 15,5 cm, day 10 x 10 cm.
//
// Muc nuoc cao nhat KHONG duoc lay bang 15,5. HC-SR04 co vung mu 2 cm va so
// doc duoi 4 cm da khong con dang tin. Chua 5,5 cm cho vung mu va sai so,
// con lai 10 cm la muc lam viec.
//
// The tich lam viec = 100 cm2 x 10 cm = 1 000 cm3 = 1 LIT.
// Bon 1 lit, khong phai 10 lit. Moi nguong thoi gian va the tich ben duoi
// deu duoc tinh lai theo con so nay.
#define TANK_SENSOR_TO_BOTTOM_CM   15.5f
#define TANK_MAX_LEVEL_CM          10.0f
#define TANK_AREA_CM2             100.0f

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

// Tan so xung LON NHAT ma cam bien co the sinh ra ve mat vat ly.
// YF-S401 chay het co 6 L/phut x 98 xung moi lit/phut = 588 Hz.
// YF-S201 chay het co 30 L/phut x 7,5 = 225 Hz.
// Lay 1200 Hz la rong gap doi truong hop nhanh nhat.
//
// CAO HON SO NAY KHONG PHAI LA NUOC. Do thuc te tren mach ngay 21/09: chan
// GPIO 4 dem duoc 17 000 Hz khi da TAT Wi-Fi va khong co giot nuoc nao chay.
// Day tin hieu dang tha noi tren dien tro keo len noi bo 45 kOhm nen bat song.
//
// Phai bat duoc truong hop nay, vi bo loc FLOW_MIN_PULSE_US khong loai bo
// duoc nhieu — no chi lay mau thua ra, bien 17 000 Hz thanh 833 Hz, tuc
// 8,5 L/phut, mot con so trong RAT THAT. Doc con so do vao rang buoc an toan
// con nguy hiem hon la khong co cam bien.
#define FLOW_MAX_PLAUSIBLE_HZ  1200.0f

// ---------- TAT HAI CAM BIEN LUU LUONG ----------
// Dat 0 khi day tin hieu chua dau xong. Do that ngay 21/09, bom da rut,
// khong mot giot nuoc chay:
//    GPIO 4  : 3 445 Hz keo len · 13 403 Hz khong keo · 3 150 Hz keo xuong
//    GPIO 19 : 3 282 Hz keo len · 12 123 Hz khong keo · 2 938 Hz keo xuong
//    GPIO 23 : 0 Hz keo len · 0 Hz keo xuong   <- chan doi chieu, khong noi gi
// Tan so DOI theo tung kieu keo, nghia la khong co nguon nao giu chac day.
// Gioi han vat ly cua YF-S401 la 588 Hz.
//
// Nhieu do lot qua duoc nguong 1200 Hz tung luc, sinh ra 8 lit "da bom" trong
// mot bon 1 lit, va lam no cac bao dong gia LEAK_SUSPECTED va VOLUME_LIMIT.
// Mot cam bien noi doi thi phai TAT han, khong duoc tin mot nua: he van chay
// dung bang cam bien muc, hai phao va cam bien dong dien.
//
// Bat lai thanh 1 sau khi da: cap du 5 V va GND cho ca hai cam bien, va gan
// dien tro keo len 4,7 kOhm tu moi chan tin hieu len 3,3 V.
// Kiem tra bang: pio run -e test_flownoise -t upload -t monitor
// 21/09, sau khi bo TXS0108E khoi hai chan luu luong:
//    GPIO 4  : 3 445 Hz  ->  25 Hz   (keo len 3,3 V)
//    GPIO 19 : 3 282 Hz  ->  51 Hz
//    GPIO 23 : 0 Hz (chan doi chieu, khong noi gi)
// Giam khoang 100 lan, du duoi nguong 1200 Hz nen bat lai duoc.
// VAN CON 25-51 Hz, tuc 0,26 va 0,52 L/phut ao — xem muc canh bao ben duoi.
// TAT LAI ngay 21/09 sau phep thu test_flowmap:
//    bom TAT  : GPIO 4 = 26,4 Hz   ·  GPIO 19 = 25,2 Hz
//    bom CHAY : GPIO 4 = 4755,9 Hz ·  GPIO 19 = 4722,0 Hz
//
// 4750 Hz chia cho he so K 98 la 48 L/phut, trong khi bom that chi day
// 0,36 L/phut. Va hai kenh trung nhau toi 0,7 phan tram — hai cam bien roi
// nhau khong the trung nhau nhu vay. Do la nhieu dong pha do bom bom vao ca
// hai duong tin hieu cung luc, khong phai nuoc.
//
// Chung nao bom con bom 4750 Hz vao day tin hieu thi khong the biet hai cam
// bien co bi dau nguoc hay khong, va cung khong the dung so doc cho viec gi.
#define FLOW_SENSOR_ENABLED   1

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
// Muc cao nhat 10 cm thi khoang cach con 15,5 - 10 = 5,5 cm.
// Dat chan cung o 4,5 cm: gan hon the la nuoc da vuot muc lam viec 1 cm,
// va cung da cham nguong so doc khong con dang tin cua HC-SR04.
#define LEVEL_MIN_DISTANCE_CM   4.5f

// Bao nhieu lan phat LIEN TIEP duoi nguong tren thi moi coi la nuoc that su
// da len qua cao. Mot lan duy nhat khong du: do that cho thay cam bien thinh
// thoang tra ve mot so ngan vo co, va luc bom vua khoi dong thi con hay hon.
// Da gap: bom vua bat 4 giay da bi khoa OVERFLOW trong khi bon dang can kiet.
//
// 3 lan x chu ky 200 ms = 600 ms. Nuoc day nhanh nhat cung chi len 0,278 cm/s,
// tuc 0,17 cm trong 600 ms — khong the vuot qua chan an toan trong khoang do.
#define TOO_CLOSE_STREAK        3
// Bon chi 1 lit. Bom JT-DC3L day 1,67 L/phut, tuc 0,028 L moi giay.
// Giu bom chay toi thieu 10 giay la bom them 0,28 lit, bang 28 phan tram
// bon — du de vot tu nguong dung 70 phan tram len gan tran. Ha xuong 3 giay:
// chi con 8 phan tram, van du dai de tranh ro le dong cat lien hoi.
#define MIN_ON_MS            3000UL
#define MIN_OFF_MS          20000UL
// Thoi gian bom toi da truoc khi ket luan bat thuong.
// TINH LAI THEO BOM THAT (JT-DC3L, 100 L/gio = 1,67 L/phut o cot nuoc bang 0):
//    the tich bon      = 100 cm2 x 10 cm = 1 L
//    bom tu 30% len 80% = 5 L
//    o luu luong TOI DA  : 5 / 1,67 x 60 = 180 s  <- dung bang gia tri cu
//    o luu luong thuc te : bom co cot nuoc nen cham hon, khoang 300 s
// Gia tri cu 180 s se bao FILL_TIMEOUT ngay trong lan bom binh thuong.
// Dat 600 s = khoang hai lan thoi gian day thuc te. PHAI do lai bang thi
// nghiem E3 roi chinh cho khop bom cua ban.
// TINH LAI THEO BON THAT (1 lit) VA BOM THAT (JT-DC3L, 1,67 L/phut):
//    day tu 30% len 70% = 0,4 lit -> 14 giay o luu luong toi da
//    day tu can len day  = 1,0 lit -> 36 giay o luu luong toi da
// Co cot nuoc thi cham hon, cu cho la cham gap doi: 72 giay.
// Dat 90 giay. Gia tri cu 240 giay rong gap hon ba lan thuc te, tuc la
// bom co the chay them hon hai phut sau khi bon da day.
// DO THAT NGAY 21/09 tren bom da lap vao he: muc len 1,2 cm trong 20 giay,
// tuc 0,06 cm/s = 0,36 L/phut. Cham hon datasheet (1,67 L/phut) gan nam lan
// vi con cot nuoc va suc can duong ong.
//    day tu can len 70 phan tram = 7 cm -> 117 giay
// Dat 200 giay, rong gan gap doi. Gia tri cu 90 giay se bao FILL_TIMEOUT
// ngay giua mot lan bom hoan toan binh thuong.
#define MAX_FILL_MS         200000UL

// ---------- Ba chan an toan KHONG phu thuoc cam bien sieu am ----------
// Bon 10 lit. Bom them qua so nay trong MOT lan bom la chac chan co van de:
// hoac cam bien muc sai, hoac nuoc dang chay di dau do.
#define MAX_FILL_VOLUME_L    2.0f

// Bom chay ma muc nuoc khong nhich len duoc NO_PROGRESS_CM trong
// NO_PROGRESS_MS thi ngat. Bom that day 1,67 L/phut vao tiet dien 400 cm2
// tuc 0,069 cm/s, nen trong 60 s phai len it nhat 4,1 cm. Lay 1,5 cm la
// rong gap gan ba lan, du cho bom yeu hay cot nuoc cao.
// Voi toc do do duoc 0,06 cm/s, trong 45 giay muc phai len 2,7 cm.
// Doi hoi 1,0 cm la chi bang mot phan ba thuc te — du rong de nhieu +-1 cm
// cua sieu am khong gay bao dong gia, van du chat de bat duoc cam bien noi doi.
// Ban cu 20 giay/1,0 cm da bao NO_PROGRESS ngay giua lan bom binh thuong.
#define NO_PROGRESS_MS      45000UL
#define NO_PROGRESS_CM      1.0f

// Tran cuoi cung. Khong dieu kien, khong ngoai le, khong tu phuc hoi.
// Bom khong duoc phep chay lien tuc lau hon so nay du bat ky ly do gi.
#define PUMP_HARD_LIMIT_MS  240000UL

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

// Bao lau khong co mot phep do hop le nao thi coi la MAT cam bien.
//
// HC-SR04 trong thung 10 x 10 cm rot khoang 30 phan tram so lan phat: chum
// song 15 do o khoang 15 cm da rong hon 7 cm nen tia bien danh vao thanh
// thung, co lan khong co tieng doi tro ve kip.
//
// Ban cu cho levelOk = false ngay tu MOT lan rot. Hau qua: levelOk lat
// lien tuc 200 ms mot lan, va nhom ST_FAULT_SENSOR — von doi 5 GIAY LIEN TUC
// khong rot lan nao moi chiu phuc hoi — khong bao gio thoat ra duoc.
// Thiet bi ket o SENSOR_TIMEOUT vinh vien du cam bien van chay.
//
// Nay giu lai so doc hop le cuoi trong 2 giay. Rot le di qua duoc, mat that
// van bi bat sau 2 giay, va ST_FILLING van ngat bom ngay khi levelOk false.
#define LEVEL_STALE_MS      3000UL
#define SENSOR_RECOVER_MS   5000UL
#define CONFLICT_LEVEL_PCT  70.0f
// Phao muc thap bao "da tut duoi vach thap" ma sieu am lai bao day hon so
// nay thi hai cam bien dang noi nguoc nhau — mot trong hai dang hong.
#define CONFLICT_MIN_PCT    60.0f
// Toc do doi muc nuoc toi da coi la co the ve mat vat ly, cm moi giay.
// TINH THEO BOM THAT: 1,67 L/phut / 100 cm2 = 0,278 cm/s khi bom.
// Xa nhanh qua voi mo cung chi khoang 0,2 cm/s.
// Dat 1 cm/s: rong gap 3,6 lan truong hop bom nhanh nhat, du cho ca luc xa
// nuoc, nhung chan duoc phan lon xung nhieu cua cam bien sieu am.
#define MAX_LEVEL_RATE_CMS  1.0f

// Sai so do cua rieng phep do, khong lien quan gi toi nuoc chay nhanh hay cham.
// Do that trong thung 10 x 10 cm: hai lan doc lien tiep cach nhau 0,2 giay
// lech nhau toi 1,1 cm. Chum song 15 do cua HC-SR04 o khoang 15 cm da rong
// hon 7 cm, tia bien danh vao THANH thung roi moi doi ve nen moi lan do lai
// chon mot duong di khac.
//
// Neu chi lay MAX_LEVEL_RATE_CMS x dt lam gioi han thi trong mot chu ky 0,2 s
// chi cho phep lech 0,2 cm, va moi so doc that deu bi loai oan — thiet bi
// bao SENSOR_TIMEOUT lien tuc du cam bien van chay tot.
//
// Cong them so nay vao gioi han: nhieu tung mau di qua duoc, nhung neu muc
// nuoc troi that thi qua vai giay tich luy lai van vuot gioi han va bi bat.
#define LEVEL_NOISE_CM      2.0f

// ---------- Chu ky ----------
#define CONTROL_PERIOD_MS   200UL
// Khoang cach toi thieu giua hai lan phat sieu am. Datasheet HC-SR04 khuyen
// tren 60 ms de tieng vong cua lan truoc kip tat han. Ban cu phat 5 lan lien
// tiep cach nhau 6 ms nen lan sau bat phai tieng vong cua lan truoc.
// Nay moi chu ky dieu khien chi phat MOT lan, va lay trung vi truot 5 mau.
// 9 chu khong phai 5. Do that trong thung 10 x 10 cm: ngoai nhieu +-1 cm,
// cam bien con thinh thoang cho SAI SO THO — doc 0,00 cm trong khi bon dang
// o 4,6 cm. Cua so 5 mau chi chiu duoc 2 mau hong lien tiep; 9 mau chiu duoc 4.
// 9 mau x 200 ms = 1,8 giay, trong do nuoc chi kip dang 0,11 cm nen khong
// lam cham phan ung chut nao.
#define LEVEL_MEDIAN_WINDOW 9
// Bien ngoai dai hinh hoc con chap nhan, tinh bang cm. So doc nam ngoai
// [15,5-10-6 , 15,5+6] = [-0,5 , 21,5] cm bi loai truoc khi vao cua so trung vi.
#define LEVEL_GATE_MARGIN_CM   6.0f

// Do phan tan toi da trong cua so trung vi, tinh bang cm.
//
// Do that khi bom dang chay: cam bien nhay qua lai giua 4,8 cm va 0,00 cm —
// luc thay mat nuoc, luc nhin xuyen xuong DAY thung. Mat nuoc bi dong nuoc
// vao lam gon nen tieng doi tan di, chi con tieng doi tu day tro ve.
//
// Trung vi khong cuu duoc kieu hong nay: khi hai nhom xap xi bang nhau thi
// trung vi chi la mot lan tung dong xu. Thay vi doan bua, hay noi thang la
// KHONG BIET — cac chan an toan theo thoi gian van lam viec binh thuong.
#define LEVEL_SPREAD_MAX_CM    3.0f
// Bao nhieu lan phat hong LIEN TIEP thi coi la mat cam bien va xoa cua so.
// 5 lan x chu ky 200 ms = 1 giay, van con thua truoc SENSOR_TIMEOUT_MS = 4 s.
// Do that: rot khoang 30 phan tram, nen chuoi 5 lan rot lien tiep xay ra
// vai lan moi phut. Moi lan xoa cua so la mat them 600 ms nua de gom lai du
// 3 mau. Nang len 10 lan (2 giay) — van con thua truoc SENSOR_TIMEOUT_MS 4 s.
#define LEVEL_FAIL_STREAK_MAX  10
#define TELEMETRY_PERIOD_MS 1000UL
#define NVS_SAVE_PERIOD_MS  60000UL
#define OFFLINE_BUFFER_SIZE 240

// ---------- Ket noi lai ----------
#define RECONNECT_BASE_MS   1000UL
#define RECONNECT_MAX_MS    30000UL
#define MQTT_KEEPALIVE_S    15
