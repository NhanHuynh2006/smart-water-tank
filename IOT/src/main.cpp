// ============================================================
//  Smart Water Tank - firmware chinh cho ESP32
//  Toan bo luat dieu khien va luat phat hien su co nam o day,
//  nen he van chay dung khi mat ket noi mang.
// ============================================================
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <time.h>
#include <sys/time.h>
#include "config.h"

// ------------------------------------------------------------
//  Trang thai
// ------------------------------------------------------------
enum State { ST_BOOT, ST_IDLE, ST_FILLING, ST_MANUAL_ON,
             ST_FAULT_DRYRUN, ST_FAULT_SENSOR, ST_OVERFLOW_LOCK };

static const char* STATE_NAME[] = {
  "BOOT","IDLE","FILLING","MANUAL_ON",
  "FAULT_DRYRUN","FAULT_SENSOR","OVERFLOW_LOCK" };

State state = ST_BOOT;
bool  autoMode  = true;
bool  pumpOn    = false;
char  faultCode[24] = "";

// ------------------------------------------------------------
//  Do luong
// ------------------------------------------------------------
volatile uint32_t flowPulses = 0;        // cam bien DAU VAO
uint32_t lastPulseSnapshot   = 0;
volatile uint32_t flowOutPulses = 0;     // cam bien DAU RA
uint32_t lastPulseOutSnapshot   = 0;

float levelCm   = 0, levelPct = 0;
bool  levelOk   = false;
float flowLpm    = 0;                    // L/phut DAU VAO
float volumeL    = 0, volumeTodayL    = 0;
float flowOutLpm = 0;                    // L/phut DAU RA
float volumeOutL = 0, volumeOutTodayL = 0;
float currentMv = 0, currentOffsetMv = 0;
bool  floatMax  = false, floatSrc = true;

uint32_t lastValidLevelMs = 0;
uint32_t sensorGoodSince  = 0;
float    lastGoodLevelCm  = -1;

// ------------------------------------------------------------
//  Dinh thi
// ------------------------------------------------------------
uint32_t pumpOnSince = 0, pumpOffSince = 0;
uint32_t dryRunSince = 0, noCurrentSince = 0, leakSince = 0;
uint32_t lastControlMs = 0, lastTelemetryMs = 0, lastNvsMs = 0;
uint32_t seqNo = 0;

// ------------------------------------------------------------
//  MQTT
// ------------------------------------------------------------
WiFiClient   net;
PubSubClient mqtt(net);
Preferences  prefs;

char topicTelemetry[64], topicPumpState[64], topicFault[64];
char topicStatus[64], topicCmd[64], topicAck[64];

uint32_t nextReconnectMs = 0;
uint8_t  reconnectFails  = 0;

// ------------------------------------------------------------
//  Bo dem vong khi mat mang
// ------------------------------------------------------------
struct Sample {
  uint32_t ts;
  uint32_t seq;
  int16_t  levelPct10;   // phan tram x10
  int16_t  flowLpm100;    // L/phut x100, dau vao
  int16_t  flowOutLpm100; // L/phut x100, dau ra
  uint32_t volumeL100;    // lit x100, dau vao
  uint8_t  flags;        // bit0 pump, bit1 levelOk, bit2 floatMax
  uint8_t  state;
};
Sample  ring[OFFLINE_BUFFER_SIZE];
uint16_t ringHead = 0, ringCount = 0;

void ringPush(const Sample& s) {
  ring[ringHead] = s;
  ringHead = (ringHead + 1) % OFFLINE_BUFFER_SIZE;
  if (ringCount < OFFLINE_BUFFER_SIZE) ringCount++;
}

// ------------------------------------------------------------
//  Tien ich
// ------------------------------------------------------------
// Loc xung qua gan nhau. Do tren mach that: khi Wi-Fi bat, hai chan nay bat
// duoc 1600 den 2700 Hz nhieu trong khi khong he co nuoc chay; tat Wi-Fi thi
// dem duoc dung 0. Dien tro keo len NOI BO cua ESP32 khoang 45 kOhm, qua yeu
// de giu muc cao truoc nhieu vo tuyen tren day dan dai.
// YF-S401 o luu luong toi da 6 L/phut chi cho 6 x 98 = 588 Hz, tuc moi xung
// cach nhau it nhat 1,7 ms. Bo qua xung den som hon FLOW_MIN_PULSE_US.
// LUU Y: day CHI la lop phong thu phan mem. Cach sua that la han them dien
// tro keo len NGOAI 4,7 kOhm len 3,3 V, manh gap 10 lan loai noi bo.
volatile uint32_t lastFlowUs = 0, lastFlowOutUs = 0;

void IRAM_ATTR onFlowPulse() {
  uint32_t now = micros();
  if (now - lastFlowUs >= FLOW_MIN_PULSE_US) { flowPulses++; lastFlowUs = now; }
}
void IRAM_ATTR onFlowOutPulse() {
  uint32_t now = micros();
  if (now - lastFlowOutUs >= FLOW_MIN_PULSE_US) { flowOutPulses++; lastFlowOutUs = now; }
}

void setRelay(bool on) {
#if RELAY_ACTIVE_LOW
  digitalWrite(PIN_RELAY, on ? LOW : HIGH);
#else
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
#endif
}

uint32_t nowTs() {
  time_t t = time(nullptr);
  return (t > 1600000000) ? (uint32_t)t : 0;   // 0 = chua dong bo NTP
}

// Dau thoi gian do phan giai mili giay. Truong "ts" cu chi co do phan giai
// mot giay, nen hieu (recv_ts - ts) bi sai so luong tu hoa toi +-500 ms va
// nhan chim do tre mang that. Truong nay giu nguyen "ts" de khong pha vo
// cac ben doc cu, va bo sung "ts_ms" cho phep do E7.
uint64_t nowTsMs() {
  struct timeval tv;
  if (gettimeofday(&tv, nullptr) != 0) return 0;
  if (tv.tv_sec < 1600000000) return 0;        // 0 = chua dong bo NTP
  return (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000);
}

// ------------------------------------------------------------
//  Doc cam bien
// ------------------------------------------------------------
float readDistanceOnce() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  uint32_t dur = pulseIn(PIN_ECHO, HIGH, 30000UL);   // timeout 30 ms
  if (dur == 0) return -1;
  return dur * 0.0343f / 2.0f;                        // cm
}

// Bo loc trung vi 5 mau: loai bo dot bien don le do song mat nuoc
// Trung vi truot. Ban cu phat 5 lan lien tiep cach nhau 6 ms ngay trong mot
// lan goi: vi pham khuyen cao >= 60 ms cua HC-SR04 nen lan phat sau bat phai
// tieng vong con lai cua lan truoc, va ham chan toi 180 ms trong chu ky 200 ms.
// Nay moi chu ky dieu khien phat DUNG MOT lan, cach nhau tron CONTROL_PERIOD_MS
// = 200 ms, va trung vi lay tren cua so LEVEL_MEDIAN_WINDOW mau gan nhat.
static float   levelWin[LEVEL_MEDIAN_WINDOW];
static uint8_t levelWinCount = 0, levelWinHead = 0, levelFailStreak = 0;

// Cua so truot chi nhan nhung so doc NAM TRONG DAI VAT LY CO THE.
// Mat cam bien cach day bon TANK_SENSOR_TO_BOTTOM_CM, nuoc day nhat thi
// khoang cach con TANK_SENSOR_TO_BOTTOM_CM - TANK_MAX_LEVEL_CM. Ngoai dai
// do cong them bien LEVEL_GATE_MARGIN_CM la nhieu, loai truoc khi vao cua so
// chu khong de no lam ban trung vi.
float medianOf5() {
  const float dMin = TANK_SENSOR_TO_BOTTOM_CM - TANK_MAX_LEVEL_CM - LEVEL_GATE_MARGIN_CM;
  const float dMax = TANK_SENSOR_TO_BOTTOM_CM + LEVEL_GATE_MARGIN_CM;

  float d = readDistanceOnce();
  bool good = (d > 0) && (d >= dMin) && (d <= dMax);

  if (good) {
    levelWin[levelWinHead] = d;
    levelWinHead = (levelWinHead + 1) % LEVEL_MEDIAN_WINDOW;
    if (levelWinCount < LEVEL_MEDIAN_WINDOW) levelWinCount++;
    levelFailStreak = 0;
  } else {
    // Vai lan hong roi rac thi bo qua, cua so cu van dung. Hong LIEN TIEP
    // moi co nghia la mat cam bien that, luc do xoa han cua so de levelOk
    // chuyen sang false va luat SENSOR_TIMEOUT co co hoi phat.
    if (levelFailStreak < 255) levelFailStreak++;
    if (levelFailStreak >= LEVEL_FAIL_STREAK_MAX) { levelWinCount = 0; return -1; }
  }
  if (levelWinCount < 3) return -1;

  // Chep levelWinCount mau MOI NHAT, di nguoc tu dau ghi. Chep tu chi so 0
  // la sai khi vong dem da quay vong: cac o dau khong con la mau moi nhat.
  float v[LEVEL_MEDIAN_WINDOW];
  for (uint8_t i = 0; i < levelWinCount; i++)
    v[i] = levelWin[(levelWinHead + LEVEL_MEDIAN_WINDOW - 1 - i) % LEVEL_MEDIAN_WINDOW];

  for (uint8_t i = 0; i + 1 < levelWinCount; i++)
    for (uint8_t j = i + 1; j < levelWinCount; j++)
      if (v[j] < v[i]) { float t = v[i]; v[i] = v[j]; v[j] = t; }
  return v[levelWinCount / 2];
}

void readLevel() {
  float d = medianOf5();
  if (d < 0) { levelOk = false; return; }

  float h = TANK_SENSOR_TO_BOTTOM_CM - d;
  h = LEVEL_CAL_A * h + LEVEL_CAL_B;

  // Loc so doc phi vat ly ngay tai nguon
  if (h < -2.0f || h > TANK_MAX_LEVEL_CM + 5.0f) { levelOk = false; return; }
  if (lastGoodLevelCm >= 0) {
    float dt = (millis() - lastValidLevelMs) / 1000.0f;
    if (dt > 0.05f && fabs(h - lastGoodLevelCm) / dt > MAX_LEVEL_RATE_CMS) {
      levelOk = false; return;
    }
  }

  if (h < 0) h = 0;
  levelCm  = h;
  levelPct = (h / TANK_MAX_LEVEL_CM) * 100.0f;
  if (levelPct > 100) levelPct = 100;
  levelOk  = true;
  lastGoodLevelCm  = h;
  lastValidLevelMs = millis();
}

void readFlow(uint32_t dtMs) {
  noInterrupts();
  uint32_t p = flowPulses;
  interrupts();
  uint32_t d = p - lastPulseSnapshot;
  lastPulseSnapshot = p;
  float freq = (dtMs > 0) ? (d * 1000.0f / dtMs) : 0;
  flowLpm = freq / FLOW_K_FACTOR;
  float addL = flowLpm * (dtMs / 1000.0f) / 60.0f;
  volumeL      += addL;
  volumeTodayL += addL;

  // Cam bien dau ra: cung cong thuc, bo dem va he so K rieng
  noInterrupts();
  uint32_t po = flowOutPulses;
  interrupts();
  uint32_t dOut = po - lastPulseOutSnapshot;
  lastPulseOutSnapshot = po;
  float freqOut = (dtMs > 0) ? (dOut * 1000.0f / dtMs) : 0;
  flowOutLpm = freqOut / FLOW_OUT_K_FACTOR;
  float addOut = flowOutLpm * (dtMs / 1000.0f) / 60.0f;
  volumeOutL      += addOut;
  volumeOutTodayL += addOut;
}

// Chi dung o dang nhi phan: do phan giai khong du cho gia tri dinh luong
void readCurrent() {
  uint32_t acc = 0;
  for (int i = 0; i < 200; i++) acc += analogRead(PIN_CURRENT);
  float mv = (acc / 200.0f) * 3300.0f / 4095.0f;
  currentMv = fabs(mv - currentOffsetMv);

  // Diem nghi cua ACS712 KHONG dung yen. Do thuc te tren mach nay thay no
  // troi 32 mV trong 20 giay dau sau khi cap nguon, tuc lon hon ca nguong
  // nhan biet 15 mV. Hieu chuan mot lan duy nhat luc khoi dong la khong du:
  // sau vai phut moi phep so sanh deu lech theo.
  // Nen bam theo diem nghi bang trung binh truot cham, chi cap nhat KHI BOM
  // DANG TAT. Bom chay thi dung cap nhat, neu khong no se hoc luon ca dong
  // cua bom va lam luat NO_CURRENT mu han.
  if (!pumpOn) {
    currentOffsetMv += (mv - currentOffsetMv) * CURRENT_OFFSET_ALPHA;
  }
}

void readFloats() {
  floatMax = (digitalRead(PIN_FLOAT_MAX) == LOW);   // LOW = kich hoat
  floatSrc = (digitalRead(PIN_FLOAT_SRC) == LOW);   // LOW = con nuoc
}

// ------------------------------------------------------------
//  RAO AN TOAN
//  Moi duong dan co the bat bom deu phai di qua ham nay.
//  Tra ve nullptr neu duoc phep, tra ve ma ly do neu bi chan.
// ------------------------------------------------------------
const char* pumpBlockReason() {
  if (faultCode[0] != '\0')                 return "fault_active";
  if (floatMax)                             return "float_max_triggered";
  if (levelOk && levelPct >= LEVEL_OVERFLOW_PCT) return "overflow_guard";
  if (!levelOk)                             return "level_sensor_invalid";
  if (!floatSrc)                            return "source_tank_empty";
  if (!pumpOn && pumpOffSince && millis() - pumpOffSince < MIN_OFF_MS)
                                            return "min_off_time";
  return nullptr;
}

void publishPumpState();

bool startPump() {
  if (pumpBlockReason() != nullptr) return false;
  if (!pumpOn) {
    pumpOn = true; setRelay(true);
    pumpOnSince = millis();
    dryRunSince = noCurrentSince = 0;
    publishPumpState();
  }
  return true;
}

void stopPump() {
  if (pumpOn) {
    pumpOn = false; setRelay(false);
    pumpOffSince = millis();
    publishPumpState();
  }
}

void raiseFault(const char* code) {
  if (strcmp(faultCode, code) == 0) return;
  strncpy(faultCode, code, sizeof(faultCode) - 1);
  stopPump();

  StaticJsonDocument<192> doc;
  doc["dev"]  = DEVICE_ID;
  doc["code"] = code;
  doc["ts"]   = nowTs();
  doc["level_pct"] = levelPct;
  doc["flow_lpm"]  = flowLpm;
  char buf[192]; size_t n = serializeJson(doc, buf);
  if (mqtt.connected()) mqtt.publish(topicFault, (uint8_t*)buf, n, false);

  digitalWrite(PIN_LED_FAULT, HIGH);
  Serial.printf("[FAULT] %s\n", code);
}

// ------------------------------------------------------------
//  BAY LUAT PHAT HIEN SU CO, kiem tra theo thu tu uu tien
// ------------------------------------------------------------
void checkFaults() {
  uint32_t now = millis();

  // 1. Mat tin hieu cam bien muc
  // lastValidLevelMs == 0 nghia la chua tung doc duoc mau hop le nao ke tu khi
  // khoi dong. Ban cu doi dieu kien nay khac 0 nen thiet bi dut day cam bien
  // ngay tu dau se KHONG BAO GIO bao loi, chi im lang khong chay bom.
  uint32_t sinceValid = lastValidLevelMs ? (now - lastValidLevelMs) : now;
  if (!levelOk && sinceValid > SENSOR_TIMEOUT_MS) {
    raiseFault("SENSOR_TIMEOUT"); state = ST_FAULT_SENSOR; return;
  }
  // 2. Mau thuan giua phao va sieu am
  if (floatMax && levelOk && levelPct < CONFLICT_LEVEL_PCT) {
    raiseFault("SENSOR_CONFLICT"); state = ST_FAULT_SENSOR; return;
  }
  // 3. Chong tran
  if (floatMax || (levelOk && levelPct >= LEVEL_OVERFLOW_PCT)) {
    raiseFault("OVERFLOW"); state = ST_OVERFLOW_LOCK; return;
  }
  // 4. Bom chay kho
  if (pumpOn) {
    if (flowLpm < DRYRUN_FLOW_LPM) {
      if (dryRunSince == 0) dryRunSince = now;
      else if (now - dryRunSince > DRYRUN_MS) {
        raiseFault("DRY_RUN"); state = ST_FAULT_DRYRUN; return;
      }
    } else dryRunSince = 0;
  } else dryRunSince = 0;

  // 5. Bom duoc lenh bat nhung khong co dong dien
  if (pumpOn) {
    if (currentMv < CURRENT_ON_MV) {
      if (noCurrentSince == 0) noCurrentSince = now;
      else if (now - noCurrentSince > NOCURRENT_MS) {
        raiseFault("NO_CURRENT"); state = ST_FAULT_DRYRUN; return;
      }
    } else noCurrentSince = 0;
  } else noCurrentSince = 0;

  // 6. Nghi ngo ro ri: bom tat ma van co dong chay
  if (!pumpOn && flowLpm > LEAK_FLOW_LPM) {
    if (leakSince == 0) leakSince = now;
    else if (now - leakSince > LEAK_MS) {
      raiseFault("LEAK_SUSPECTED");   // canh bao, khong khoa bom
      leakSince = now;
    }
  } else leakSince = 0;

  // 7. Bom chay qua lau ma chua day
  if (pumpOn && pumpOnSince && now - pumpOnSince > MAX_FILL_MS) {
    raiseFault("FILL_TIMEOUT"); state = ST_FAULT_DRYRUN; return;
  }
}

// ------------------------------------------------------------
//  MAY TRANG THAI
// ------------------------------------------------------------
void runStateMachine() {
  uint32_t now = millis();

  switch (state) {
    case ST_BOOT:
      if (levelOk) state = ST_IDLE;
      break;

    case ST_IDLE:
      if (autoMode && levelOk && levelPct < LEVEL_LOW_PCT) {
        if (startPump()) state = ST_FILLING;
      }
      break;

    case ST_FILLING:
      if (!autoMode) { stopPump(); state = ST_IDLE; break; }
      if (levelPct > LEVEL_HIGH_PCT && now - pumpOnSince >= MIN_ON_MS) {
        stopPump(); state = ST_IDLE;
      }
      break;

    case ST_MANUAL_ON:
      // Rao an toan van duoc kiem tra moi chu ky ngay ca o che do thu cong
      if (autoMode || pumpBlockReason() != nullptr) {
        stopPump();
        state = autoMode ? ST_IDLE : ST_IDLE;
      }
      break;

    case ST_FAULT_SENSOR:
      // Nhom tu phuc hoi: tin hieu hop le lien tuc du lau thi tu het
      if (levelOk && !(floatMax && levelPct < CONFLICT_LEVEL_PCT)) {
        if (sensorGoodSince == 0) sensorGoodSince = now;
        else if (now - sensorGoodSince > SENSOR_RECOVER_MS) {
          faultCode[0] = '\0';
          digitalWrite(PIN_LED_FAULT, LOW);
          sensorGoodSince = 0;
          state = ST_IDLE;
        }
      } else sensorGoodSince = 0;
      break;

    case ST_FAULT_DRYRUN:
    case ST_OVERFLOW_LOCK:
      stopPump();   // giu trang thai cho toi khi nguoi van hanh xoa loi
      break;
  }
}

// ------------------------------------------------------------
//  Cong bo ban tin
// ------------------------------------------------------------
void publishPumpState() {
  StaticJsonDocument<128> doc;
  doc["dev"]   = DEVICE_ID;
  doc["pump"]  = pumpOn;
  doc["state"] = STATE_NAME[state];
  doc["ts"]    = nowTs();
  char buf[128]; size_t n = serializeJson(doc, buf);
  if (mqtt.connected()) mqtt.publish(topicPumpState, (uint8_t*)buf, n, true);
}

size_t buildTelemetry(char* buf, size_t cap, const Sample* s) {
  StaticJsonDocument<512> doc;
  doc["dev"] = DEVICE_ID;
  if (s) {
    doc["ts"]  = s->ts;
    doc["seq"] = s->seq;
    doc["level_pct"] = s->levelPct10 / 10.0f;
    doc["flow_lpm"]     = s->flowLpm100 / 100.0f;
    doc["flow_out_lpm"] = s->flowOutLpm100 / 100.0f;
    doc["volume_l"]  = s->volumeL100 / 100.0f;
    doc["pump"]      = (bool)(s->flags & 0x01);
    doc["level_ok"]  = (bool)(s->flags & 0x02);
    doc["float_max"] = (bool)(s->flags & 0x04);
    doc["state"]     = STATE_NAME[s->state];
    doc["replay"]    = true;
  } else {
    doc["ts"]    = nowTs();
    doc["ts_ms"] = nowTsMs();
    doc["seq"]   = seqNo;
    doc["level_pct"]      = levelPct;
    doc["level_cm"]       = levelCm;
    doc["level_ok"]       = levelOk;
    doc["flow_lpm"]           = flowLpm;
    doc["volume_l"]           = volumeL;
    doc["volume_today_l"]     = volumeTodayL;
    doc["flow_out_lpm"]       = flowOutLpm;
    doc["volume_out_l"]       = volumeOutL;
    doc["volume_out_today_l"] = volumeOutTodayL;
    doc["pump"]      = pumpOn;
    doc["state"]     = STATE_NAME[state];
    doc["mode"]      = autoMode ? "AUTO" : "MANUAL";
    doc["current_mv"]= currentMv;
    doc["float_max"] = floatMax;
    doc["float_src"] = floatSrc;
    doc["fault"]     = faultCode;
    doc["rssi"]      = WiFi.RSSI();
  }
  return serializeJson(doc, buf, cap);
}

void publishTelemetry() {
  seqNo++;
  char buf[448];
  size_t n = buildTelemetry(buf, sizeof(buf), nullptr);

  if (mqtt.connected()) {
    mqtt.publish(topicTelemetry, (uint8_t*)buf, n, false);
  } else {
    Sample s;
    s.ts = nowTs(); s.seq = seqNo;
    s.levelPct10 = (int16_t)(levelPct * 10);
    s.flowLpm100    = (int16_t)(flowLpm * 100);
    s.flowOutLpm100 = (int16_t)(flowOutLpm * 100);
    s.volumeL100 = (uint32_t)(volumeL * 100);
    s.flags = (pumpOn ? 1 : 0) | (levelOk ? 2 : 0) | (floatMax ? 4 : 0);
    s.state = (uint8_t)state;
    ringPush(s);
  }
}

void flushRing() {
  if (ringCount == 0) return;
  Serial.printf("[MQTT] phat lai %u ban tin da dem\n", ringCount);
  uint16_t idx = (ringHead + OFFLINE_BUFFER_SIZE - ringCount) % OFFLINE_BUFFER_SIZE;
  char buf[448];
  for (uint16_t i = 0; i < ringCount; i++) {
    size_t n = buildTelemetry(buf, sizeof(buf), &ring[idx]);
    mqtt.publish(topicTelemetry, (uint8_t*)buf, n, false);
    idx = (idx + 1) % OFFLINE_BUFFER_SIZE;
    mqtt.loop();
    delay(5);
  }
  ringCount = 0;
}

void sendAck(const char* cmdId, const char* status, const char* reason) {
  StaticJsonDocument<256> doc;
  doc["dev"]    = DEVICE_ID;
  doc["cmd_id"] = cmdId;
  doc["status"] = status;
  if (reason) doc["reason"] = reason;
  doc["state"]  = STATE_NAME[state];
  doc["pump"]   = pumpOn;
  doc["mode"]   = autoMode ? "AUTO" : "MANUAL";
  doc["ts"]     = nowTs();
  char buf[256]; size_t n = serializeJson(doc, buf);
  mqtt.publish(topicAck, (uint8_t*)buf, n, false);
}

// ------------------------------------------------------------
//  Xu ly lenh di xuong
// ------------------------------------------------------------
void onMessage(char* topic, byte* payload, unsigned int len) {
  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, payload, len)) return;

  const char* cmdId  = doc["cmd_id"] | "unknown";
  const char* action = doc["action"] | "";

  if (!strcmp(action, "mode")) {
    bool wantAuto = doc["value"] | true;
    autoMode = wantAuto;
    if (autoMode && state == ST_MANUAL_ON) { stopPump(); state = ST_IDLE; }
    sendAck(cmdId, "accepted", nullptr);

  } else if (!strcmp(action, "pump")) {
    bool want = doc["value"] | false;
    if (autoMode) { sendAck(cmdId, "rejected", "auto_mode_active"); return; }
    if (want) {
      const char* why = pumpBlockReason();
      if (why) { sendAck(cmdId, "rejected", why); return; }
      startPump(); state = ST_MANUAL_ON;
      sendAck(cmdId, "accepted", nullptr);
    } else {
      stopPump(); state = ST_IDLE;
      sendAck(cmdId, "accepted", nullptr);
    }

  } else if (!strcmp(action, "clear_fault")) {
    if (faultCode[0] == '\0') { sendAck(cmdId, "rejected", "no_active_fault"); return; }
    if (floatMax || (levelOk && levelPct >= LEVEL_OVERFLOW_PCT)) {
      sendAck(cmdId, "rejected", "condition_still_present"); return;
    }
    faultCode[0] = '\0';
    digitalWrite(PIN_LED_FAULT, LOW);
    state = ST_IDLE;
    sendAck(cmdId, "accepted", nullptr);

  } else if (!strcmp(action, "reset_volume")) {
    volumeTodayL = 0;
    volumeOutTodayL = 0;
    sendAck(cmdId, "accepted", nullptr);

  } else {
    sendAck(cmdId, "rejected", "unknown_action");
  }
}

// ------------------------------------------------------------
//  Ket noi khong chan
// ------------------------------------------------------------
void mqttTryConnect() {
  if (millis() < nextReconnectMs) return;
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    nextReconnectMs = millis() + RECONNECT_BASE_MS;
    return;
  }
  Serial.print("[MQTT] dang ket noi... ");
  bool ok = mqtt.connect(DEVICE_ID, MQTT_USER, MQTT_PASS,
                         topicStatus, 1, true, "{\"online\":false}");
  if (ok) {
    Serial.println("thanh cong");
    reconnectFails = 0;
    mqtt.publish(topicStatus, "{\"online\":true}", true);
    mqtt.subscribe(topicCmd, 1);
    publishPumpState();
    flushRing();
  } else {
    reconnectFails = min<uint8_t>(reconnectFails + 1, 5);
    uint32_t wait = RECONNECT_BASE_MS * (1UL << reconnectFails);
    if (wait > RECONNECT_MAX_MS) wait = RECONNECT_MAX_MS;
    nextReconnectMs = millis() + wait;
    Serial.printf("that bai rc=%d, thu lai sau %lu ms\n", mqtt.state(), wait);
  }
}

// ------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  // Trang thai an toan mac dinh, dat truoc khi doc bat ky cam bien nao
  pinMode(PIN_RELAY, OUTPUT);
  setRelay(false);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
#if FLOW_PIN_PULLUP
  pinMode(PIN_FLOW, INPUT_PULLUP);   // noi thang, keo len noi bo len 3,3 V
#else
  pinMode(PIN_FLOW, INPUT);          // qua chia ap, KHONG keo len noi bo
#endif
  pinMode(PIN_FLOAT_MAX, INPUT_PULLUP);
  pinMode(PIN_FLOAT_SRC, INPUT_PULLUP);
  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_FAULT, OUTPUT);
  pinMode(PIN_BTN_RESET, INPUT_PULLUP);
  analogReadResolution(12);

  attachInterrupt(digitalPinToInterrupt(PIN_FLOW), onFlowPulse, FALLING);
#if FLOW_OUT_PIN_PULLUP
  pinMode(PIN_FLOW_OUT, INPUT_PULLUP);
#else
  pinMode(PIN_FLOW_OUT, INPUT);
#endif
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_OUT), onFlowOutPulse, FALLING);

  snprintf(topicTelemetry, sizeof(topicTelemetry), "wt/%s/%s/telemetry",  SITE_ID, DEVICE_ID);
  snprintf(topicPumpState, sizeof(topicPumpState), "wt/%s/%s/state/pump", SITE_ID, DEVICE_ID);
  snprintf(topicFault,     sizeof(topicFault),     "wt/%s/%s/event/fault",SITE_ID, DEVICE_ID);
  snprintf(topicStatus,    sizeof(topicStatus),    "wt/%s/%s/status",     SITE_ID, DEVICE_ID);
  snprintf(topicCmd,       sizeof(topicCmd),       "wt/%s/%s/cmd",        SITE_ID, DEVICE_ID);
  snprintf(topicAck,       sizeof(topicAck),       "wt/%s/%s/cmd/ack",    SITE_ID, DEVICE_ID);

  // Do gia tri lech khong cua cam bien dong khi bom chac chan dang tat.
  // Cho nguon va cam bien on dinh truoc, neu khong se lay nham gia tri
  // dang troi ngay sau khi cap dien lam moc.
  delay(CURRENT_SETTLE_MS);
  uint32_t acc = 0;
  for (int i = 0; i < 200; i++) acc += analogRead(PIN_CURRENT);
  currentOffsetMv = (acc / 200.0f) * 3300.0f / 4095.0f;

  prefs.begin("wtank", false);
  volumeL         = prefs.getFloat("vol", 0);
  volumeTodayL    = prefs.getFloat("volday", 0);
  volumeOutL      = prefs.getFloat("volout", 0);
  volumeOutTodayL = prefs.getFloat("voloutday", 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Tat che do tiet kiem dien cua Wi-Fi. Mac dinh ESP32 chi bat radio theo
  // nhip DTIM cua router, nen goi tin den phai nam cho toi 100-300 ms moi
  // duoc xu ly. Tat di thi do tre lenh giam manh, doi lai ton them dong.
  WiFi.setSleep(false);
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);
  mqtt.setKeepAlive(MQTT_KEEPALIVE_S);
  mqtt.setBufferSize(512);

  lastValidLevelMs = millis();
  Serial.println("[BOOT] san sang");
}

void loop() {
  // Ket noi lai khong chan: vong dieu khien khong bao gio bi treo vi mang
  if (!mqtt.connected()) mqttTryConnect();
  else mqtt.loop();

  uint32_t now = millis();

  if (now - lastControlMs >= CONTROL_PERIOD_MS) {
    uint32_t dt = now - lastControlMs;
    lastControlMs = now;

    readLevel();
    readFlow(dt);
    readCurrent();
    readFloats();

    checkFaults();
    runStateMachine();

    digitalWrite(PIN_LED_OK, mqtt.connected() ? HIGH : LOW);

    // Nut xoa loi tren thiet bi
    if (digitalRead(PIN_BTN_RESET) == LOW && faultCode[0] != '\0') {
      if (!floatMax && !(levelOk && levelPct >= LEVEL_OVERFLOW_PCT)) {
        faultCode[0] = '\0';
        digitalWrite(PIN_LED_FAULT, LOW);
        state = ST_IDLE;
      }
    }
  }

  if (now - lastTelemetryMs >= TELEMETRY_PERIOD_MS) {
    lastTelemetryMs = now;
    publishTelemetry();
  }

  if (now - lastNvsMs >= NVS_SAVE_PERIOD_MS) {
    lastNvsMs = now;
    prefs.putFloat("vol", volumeL);
    prefs.putFloat("volday", volumeTodayL);
    prefs.putFloat("volout", volumeOutL);
    prefs.putFloat("voloutday", volumeOutTodayL);
  }
}
