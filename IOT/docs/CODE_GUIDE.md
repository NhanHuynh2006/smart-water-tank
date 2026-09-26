# Đọc hiểu mã nguồn trong một buổi

Tài liệu cho người **mới mở dự án lần đầu**. Đọc từ trên xuống: trước hết là bức tranh toàn cảnh, sau đó đi theo đường đi của **một con số** (mức nước) và **một cái bấm nút** (bật bơm) qua từng tầng, cuối cùng là bản đồ từng file.

Sơ đồ viết bằng Mermaid. GitHub và VS Code (tiện ích *Markdown Preview Mermaid Support*) vẽ được trực tiếp.

---

## 1. Dự án này làm gì

Một bồn nước nhỏ (đáy 10 × 10 cm, cao 17,5 cm) có:

- cảm biến siêu âm HC-SR04 đo mức nước từ trên xuống,
- hai phao: phao **trên** báo sắp tràn, phao **dưới** báo sắp cạn,
- một bơm 5 V bơm nước từ bồn nguồn lên, đóng/ngắt qua rơ-le, có diode chống ngược,
- cảm biến dòng ACS712 để biết bơm **có thật sự ăn điện** không,
- hai cảm biến lưu lượng YF-S401 (vào và ra),
- một ESP32 đọc tất cả, **tự quyết định** bật/tắt bơm, và gửi số liệu qua Wi-Fi.

Máy tính chạy broker MQTT, dịch vụ nền Python lưu số liệu vào SQLite, và một trang web để xem và điều khiển. Trang web ra Internet qua đường hầm Cloudflare nên ai có link cũng xem được; muốn bấm nút thì phải có mật khẩu quản trị.

**Ý tưởng quan trọng nhất:** mọi luật điều khiển và luật an toàn nằm trong ESP32. Máy chủ chỉ **xem** và **gửi yêu cầu**. Mất mạng, bồn vẫn tự bơm và tự ngắt đúng ngưỡng.

## 2. Bức tranh toàn cảnh

```mermaid
flowchart LR
  subgraph Bồn["Phần cứng"]
    US[HC-SR04<br/>mức nước]
    FL[Phao trên / dưới]
    CUR[ACS712<br/>dòng bơm]
    FS[YF-S401 ×2<br/>lưu lượng]
    RL[Rơ-le + diode] --> P[Bơm 5 V]
  end
  subgraph ESP["ESP32 — src/main.cpp"]
    R[Đọc cảm biến<br/>200 ms/lần] --> F[Lọc mức nước] --> S[Luật lỗi +<br/>máy trạng thái]
    S --> RL
    S --> T[Đóng gói JSON]
    T --> RB[(Bộ đệm vòng<br/>240 mẫu)]
  end
  US & FL & CUR & FS --> R
  T -- MQTT --> B[(Mosquitto<br/>cổng 1883)]
  RB -- phát lại khi có mạng --> B
  B -- MQTT --> BE[backend/app.py<br/>FastAPI]
  BE --> DB[(SQLite<br/>watertank.db)]
  BE -- REST /api --> UI[dashboard/index.html]
  UI -- "POST /api/command<br/>(cần đăng nhập)" --> BE
  BE -- "wt/.../cmd" --> B -- lệnh --> S
  CF[cloudflared] -. link công khai .-> UI
```

Chiều **đi lên** (thiết bị → người xem) gọi là *northbound*: telemetry, trạng thái bơm, sự cố, khả dụng. Chiều **đi xuống** gọi là *southbound*: chỉ có lệnh.

## 3. Cây thư mục

```
IOT/                         ← thư mục gốc repo
├── start.sh / stop.sh       chạy / dừng broker + backend + đường hầm
├── LENH.md                  sổ tay lệnh hay dùng
├── IOT/
│   ├── src/main.cpp         ★ firmware ESP32 (≈1300 dòng, đọc kỹ nhất)
│   ├── src/tests/           các chương trình thử từng linh kiện
│   ├── include/config.h     hằng số + mật khẩu Wi-Fi (KHÔNG lên GitHub)
│   ├── include/config.example.h  bản mẫu không có bí mật
│   ├── platformio.ini       env main + các env test_*
│   ├── backend/app.py       ★ dịch vụ nền: MQTT → SQLite → REST
│   ├── backend/simulator.py ESP32 giả để chạy thử khi chưa có mạch
│   ├── dashboard/index.html ★ toàn bộ giao diện, một file, không framework
│   ├── mosquitto/           cấu hình broker, ACL, file mật khẩu
│   ├── tools/               phân tích thí nghiệm, bench.py
│   └── docs/                protocol.md, wiring.md, TESTING.md, file này
├── water-tank-arduino/      cùng firmware nhưng cho Arduino IDE
└── iot-report-en/           báo cáo LaTeX tiếng Anh
```

`water-tank-arduino/water_tank/water_tank_firmware.cpp` là **bản sao** của `IOT/src/main.cpp`. Sửa ở `src/main.cpp` rồi chép sang, đừng sửa hai nơi.

## 4. Firmware — `IOT/src/main.cpp`

### 4.1 Vòng lặp chính

ESP32 không dùng hệ điều hành đa luồng cho logic. Mọi thứ nằm trong `loop()` và **không có lệnh nào được phép chặn lâu**:

```mermaid
flowchart TD
  L([loop]) --> C{MQTT còn nối?}
  C -- không --> RC[mqttTryConnect<br/>thử lại có lùi dần,<br/>mỗi lần tối đa 0,5 s]
  C -- có --> ML[mqtt.loop<br/>nhận lệnh → onMessage]
  RC --> T1
  ML --> T1{đủ 200 ms?}
  T1 -- có --> A[readLevel<br/>readFlow<br/>readCurrent<br/>readFloats]
  A --> M[updateLevelModel]
  M --> CF[checkFaults<br/>11 luật]
  CF --> SM[runStateMachine]
  SM --> BTN[nút xóa lỗi trên mạch]
  T1 -- chưa --> T2
  BTN --> T2{đủ 1 s?}
  T2 -- có --> PT[publishTelemetry<br/>có mạng: gửi<br/>mất mạng: ringPush]
  T2 -- chưa --> T3
  PT --> T3{đủ chu kỳ NVS?}
  T3 -- có --> NV[lưu thể tích vào flash]
  T3 --> L
  NV --> L
```

Chu kỳ điều khiển 200 ms chạy **dù có mạng hay không**. Mạng chỉ ảnh hưởng nhánh gửi đi.

### 4.2 Đường đi của một số đo mức nước

HC-SR04 đo khoảng cách `d` từ cảm biến xuống mặt nước. Mức nước `h = 17,5 − d`. Nghe đơn giản, nhưng bồn hẹp làm sóng dội thành vách nên số đo thô rất bẩn. Hàm `readLevel()` cho số đo đi qua nhiều cổng, cổng nào cũng có bộ đếm từ chối riêng (`rj_*` trong telemetry) để biết cổng nào đang chặn:

```mermaid
flowchart TD
  P[1 lần phát siêu âm<br/>readDistanceOnce] --> W[Cửa sổ 15 mẫu gần nhất]
  W --> G{Trong hình học bồn?}
  G -- không --> X1[rj_gate]
  G -- có --> Q{Độ trải tứ phân vị<br/>≤ 3,5 cm?}
  Q -- không --> X2[rj_spread]
  Q -- có --> MED[Lấy trung vị → d]
  MED --> H["h = 17,5 − d<br/>h = A·h + B (hiệu chuẩn)"]
  H --> RG{Thay đổi nhanh hơn<br/>vật lý cho phép?}
  RG -- có --> X3[rj_rate]
  RG -- không --> FG{Đang bơm mà<br/>mức tụt > 1 cm?}
  FG -- có --> X4[rj_fall]
  FG -- không --> AB[Bộ lọc alpha-beta<br/>α = 0,03]
  AB --> OUT[levelCm, levelPct,<br/>levelRateCmS, levelOk = true]
  X1 & X2 & X3 & X4 --> ST[markLevelStale:<br/>quá 5 s không có số tốt<br/>→ levelOk = false]
```

- **Alpha-beta** là bộ lọc hai trạng thái (mức và tốc độ). Nó cho ra cả `levelRateCmS` — tốc độ dâng/hạ — dùng cho phân loại sức khỏe bơm và tính lưu lượng.
- **Mô hình song song** (`updateLevelModel`): một mức nước "ảo" chạy theo công thức `bơm vào − xả ra`, luôn bị kéo về số đo thật. Khi siêu âm mất tín hiệu trong lúc bơm, `controlPct()` dùng mô hình thay số đo, tối đa 45 s. Quá 45 s → lỗi `LEVEL_LOST`.
- **Chặn tràn cứng** `waterTooClose()`: đọc thẳng khoảng cách thô, **không đi qua bộ lọc**. Ba lần liên tiếp nước cách cảm biến < 4,5 cm là khóa bơm. Bộ lọc có hỏng thế nào thì đường này vẫn còn.

### 4.3 Lưu lượng

Hằng số `FLOW_FROM_LEVEL` trong `config.h` chọn nguồn:

- `1` (đang dùng): **dòng vào** = hằng số bơm 0,36 L/phút khi bơm chạy (đã đo bằng tốc độ dâng mức). **Dòng ra** = tốc độ hạ mức × diện tích đáy, chỉ đo khi bơm đã tắt quá 20 s, và **giữ nguyên** trong lúc bơm chạy. Lý do: YF-S401 bị nhiễu dẫn ~1500 Hz khi bơm chạy, và dòng ra thật (0,1 L/phút) thấp hơn ngưỡng đo của nó.
- `0`: đếm xung từ YF-S401 qua ngắt `onFlowPulse`, có bộ lọc xung quá gần nhau.

Thể tích cộng dồn `volumeL`, `volumeOutL` lưu vào flash (NVS) định kỳ nên mất điện không mất số.

### 4.4 Luật lỗi — `checkFaults()`

Chạy mỗi 200 ms, **trước** máy trạng thái, theo thứ tự ưu tiên. Luật nào bắn thì gọi `raiseFault(code)`: ngắt bơm ngay, bật đèn lỗi, gửi bản tin `event/fault` kèm `pump_health` và dòng điện lúc đó.

| Mã | Điều kiện | Nhóm |
|---|---|---|
| `SENSOR_TIMEOUT` | không có số đo mức hợp lệ quá 45 s | cảm biến, tự hồi phục |
| `SENSOR_CONFLICT` | phao trên báo đầy mà siêu âm < 70 %, hoặc phao dưới báo cạn mà siêu âm > 60 % | cảm biến, tự hồi phục |
| `LEVEL_LOST` | đang bơm, mù quá 45 s (trong máy trạng thái) | cảm biến, tự hồi phục |
| `OVERFLOW` | phao trên **hoặc** khoảng cách thô quá gần **hoặc** mức ≥ 85 % | khóa, phải xóa tay |
| `NO_CURRENT` | rơ-le đóng mà dòng < ngưỡng quá 2 s (đứt dây bơm) | khóa |
| `DRY_RUN` | bơm chạy mà cảm biến lưu lượng < 0,25 L/phút quá 6 s (tắt khi `FLOW_FROM_LEVEL=1`) | khóa |
| `NO_PROGRESS` | bơm chạy 90 s mà mức không lên nổi 1,5 cm (chạy khô, tuột ống) | khóa |
| `FILL_TIMEOUT` | một lần bơm quá 240 s | khóa |
| `HARD_LIMIT` | quá 300 s, không điều kiện, không ngoại lệ | khóa |
| `VOLUME_LIMIT` | một lần bơm quá 2,5 L | khóa |
| `LEAK_SUSPECTED` | bơm tắt mà vẫn có dòng chảy vào quá 60 s | chỉ cảnh báo |

**Bất biến:** hễ có mã lỗi thì trạng thái phải là một trạng thái lỗi. Đầu `runStateMachine()` ép điều này mỗi chu kỳ.

### 4.5 Phân loại sức khỏe bơm — `pumpHealth()`

Đây là phần "nâng cao" của đề (nhất quán dòng điện ↔ lưu lượng). Không ngắt bơm, chỉ **gắn nhãn** để người vận hành biết hỏng ở đâu:

```mermaid
flowchart TD
  A{Bơm đang bật?} -- không --> I[idle]
  A -- có --> B{Bật chưa tới 2 s?}
  B -- có --> S[starting]
  B -- không --> C{Có dòng điện?}
  C -- không --> NC[no_current<br/>đứt dây, hỏng rơ-le]
  C -- có --> D{Mức đang dâng<br/>> 0,01 cm/s?}
  D -- có --> OK[ok]
  D -- không --> E{Đã chạy 25 s?}
  E -- chưa --> S
  E -- rồi --> NF[no_flow<br/>ăn điện mà không có nước:<br/>chạy khô, tắc, tuột ống]
```

### 4.6 Máy trạng thái — `runStateMachine()`

```mermaid
stateDiagram-v2
  [*] --> BOOT
  BOOT --> IDLE: có số đo mức đầu tiên
  IDLE --> FILLING: TỰ ĐỘNG và (mức < 30 % hoặc phao dưới)<br/>và qua rào an toàn
  FILLING --> IDLE: mức > 70 % và đã chạy ≥ 3 s
  FILLING --> IDLE: chuyển sang THỦ CÔNG
  IDLE --> MANUAL_ON: lệnh bật bơm (THỦ CÔNG)<br/>và qua rào an toàn
  MANUAL_ON --> IDLE: lệnh tắt / rào an toàn chặn / về TỰ ĐỘNG
  IDLE --> FAULT_SENSOR: lỗi cảm biến
  FILLING --> FAULT_SENSOR: LEVEL_LOST, SENSOR_*
  FAULT_SENSOR --> IDLE: số đo tốt liên tục 2,5 s
  FILLING --> FAULT_DRYRUN: NO_CURRENT, NO_PROGRESS,<br/>FILL_TIMEOUT
  MANUAL_ON --> FAULT_DRYRUN
  FILLING --> OVERFLOW_LOCK: OVERFLOW, HARD_LIMIT,<br/>VOLUME_LIMIT
  MANUAL_ON --> OVERFLOW_LOCK
  FAULT_DRYRUN --> IDLE: lệnh Xóa lỗi / nút trên mạch
  OVERFLOW_LOCK --> IDLE: Xóa lỗi, chỉ khi hết điều kiện tràn
```

Khoảng trễ 30 %–70 % chính là điều khiển có trễ (hysteresis). Thêm `MIN_OFF` 20 s: tắt rồi phải nghỉ 20 s mới được bật lại, chống đóng cắt liên tục.

### 4.7 Rào an toàn — `pumpBlockReason()`

**Mọi** đường bật bơm (tự động, lệnh tay) đều đi qua `startPump()`, và `startPump()` hỏi `pumpBlockReason()` trước. Hàm trả về lý do từ chối, theo thứ tự: `fault_active`, `float_max_triggered`, `water_too_close`, `overflow_guard`, `level_sensor_invalid`, `min_off_time`. Ở `ST_MANUAL_ON`, hàm này được hỏi lại **mỗi chu kỳ**: chế độ tay không bao giờ vượt được rào.

### 4.8 Đường đi của một cái bấm nút "Bật bơm"

```mermaid
sequenceDiagram
  participant U as Người dùng (đã đăng nhập)
  participant D as dashboard
  participant B as backend
  participant M as Mosquitto
  participant E as ESP32
  U->>D: bấm Bật bơm
  D->>B: POST /api/command {action:"pump", value:true}<br/>cookie wt_session
  B->>B: require_control(): phiên hợp lệ? không → 401
  B->>B: sinh cmd_id, ghi bảng commands(sent_ts)
  B->>M: wt/lab1/esp32-01/cmd (QoS 1)
  B-->>D: {cmd_id}
  D->>D: "Đang gửi lệnh…" — KHÔNG đổi trạng thái bơm
  M->>E: lệnh
  E->>E: onMessage: chép payload ra bộ đệm riêng,<br/>kiểm tra chế độ, rào an toàn, MIN_OFF
  E->>M: cmd/ack {status, reason, pump, state}
  M->>B: ack
  B->>B: ghi ack_ts, latency_ms, status, reason
  D->>B: GET /api/commands (hỏi lại vài lần)
  B-->>D: accepted / rejected + lý do
  D->>U: báo kết quả bằng lời dễ hiểu,<br/>trạng thái bơm theo telemetry thật
```

Nguyên tắc: giao diện **không bao giờ tự đổi** công tắc khi người dùng bấm. Nó chỉ hiện điều thiết bị xác nhận.

Chi tiết từng gặp: PubSubClient dùng **chung một bộ đệm** cho gói nhận và gói gửi. Nếu `onMessage` gửi ack trong khi ArduinoJson còn trỏ vào bộ đệm đó, `cmd_id` bị ghi đè. Vì thế `onMessage` chép payload ra mảng cục bộ trước khi phân tích.

### 4.9 Mất mạng

```mermaid
sequenceDiagram
  participant E as ESP32
  participant M as Mosquitto
  participant B as backend
  Note over E,M: đang kết nối, gửi telemetry 1 Hz
  E-xM: mất Wi-Fi / broker
  Note over E: vòng điều khiển vẫn chạy 200 ms<br/>bơm vẫn tự bật/tắt theo 30–70 %
  Note over E: telemetry → bộ đệm vòng 240 mẫu (4 phút)
  M->>B: sau ~22 s không có PINGREQ:<br/>di chúc status {"online":false}
  Note over B: dashboard: quá 5 s không có số → "mất kết nối"
  E->>M: nối lại (lùi dần 1 s → 30 s)
  E->>M: status {"online":true} (giữ lại)
  E->>M: phát lại các mẫu đã đệm, "replay": true
  M->>B: B ghi vào DB, loại khỏi thống kê độ trễ
```

Khi nối lại, ESP32 hỏi mDNS tên máy chủ (`Nolan.local`) để tìm IP broker mới, và thử lần lượt tối đa 3 mạng Wi-Fi khai báo trong `config.h`.

## 5. Backend — `IOT/backend/app.py`

Một file, bốn phần:

1. **Cấu hình và CSDL** (đầu file): tên chủ đề, `SCHEMA`, `init_db()` tự thêm cột mới cho CSDL cũ, ghi đè bảng `config` bằng ngưỡng hiện tại.
2. **`LeakDetector`**: đường nền trượt 48 khe × 30 phút (EWMA). Mỗi khe học lượng nước dùng "bình thường" vào giờ đó; dùng vượt trung bình + 3σ là cảnh báo `LEAK_BASELINE`. Lúc khởi động, `rebuild_leak_baseline()` học lại từ lịch sử trong DB.
3. **Nhận MQTT** (`_on_message`): mỗi chủ đề ghi một bảng — telemetry → `telemetry`, `state/pump` → `pump_events`, `event/fault` → `faults`, `status` → `availability`, `cmd/ack` → cập nhật `commands`. Chạy trong luồng riêng của paho.
4. **REST** (FastAPI):

| Đường dẫn | Việc |
|---|---|
| `GET /` | trả về dashboard |
| `GET /api/latest` | số đo mới nhất + `online` + `age_s` (độ tươi) |
| `GET /api/telemetry?minutes=` | chuỗi thời gian cho biểu đồ |
| `GET /api/volume/daily` | thể tích vào/ra theo ngày (cộng các bước tăng, bỏ qua lúc xóa bộ đếm) |
| `GET /api/faults`, `/api/events`, `/api/commands` | lịch sử |
| `GET /api/leak/baseline` | tình trạng đường nền rò rỉ |
| `GET /api/stats/latency` | độ trễ lệnh khứ hồi và telemetry |
| `POST /api/login`, `/api/logout`, `GET /api/auth` | phiên quản trị bằng mật khẩu |
| `POST /api/command`, `PUT /api/config` | **cần đăng nhập** |

Mật khẩu quản trị đọc từ `~/.cache/water-tank/admin_password` (ngoài repo). Đăng nhập đúng → cookie `wt_session` HttpOnly, SameSite=Strict, Secure khi qua https, sống 7 ngày. Sai 5 lần trong 5 phút → khóa IP đó. Phân quyền **không** dựa vào IP vì qua đường hầm mọi yêu cầu đều đến từ 127.0.0.1.

Lược đồ CSDL:

```mermaid
erDiagram
  telemetry {
    int id PK
    text dev
    int ts "đồng hồ thiết bị, giây"
    int ts_ms "đồng hồ thiết bị, ms"
    real recv_ts "đồng hồ máy chủ"
    int seq "đếm mất gói"
    real level_pct
    real level_cm
    int level_ok
    real flow_lpm
    real volume_l
    real flow_out_lpm
    real volume_out_l
    int pump
    text state
    text mode
    real current_mv
    int float_max
    int float_min
    text fault
    int rssi
    int replay
  }
  faults {
    int id PK
    text code
    real recv_ts
    real level_pct
    text pump_health
  }
  pump_events {
    int id PK
    int pump
    text state
    real recv_ts
  }
  commands {
    text cmd_id PK
    text action
    real sent_ts
    real ack_ts
    text status
    text reason
    real latency_ms
  }
  availability {
    int id PK
    int online
    real recv_ts
  }
  config {
    text key PK
    text value
  }
```

## 6. Dashboard — `IOT/dashboard/index.html`

Một file HTML + CSS + JavaScript thuần, không build. Các khối:

- **Sơ đồ bồn** (SVG): mực nước, thước 0–14 cm, vạch 30/70/85 %, bơm, ống, phao. Số hiển thị được **nội suy mượt** giữa hai lần nhận và có vùng chết 0,25 cm để không rung.
- **Ô số**: mức, dòng vào, dòng ra, dòng điện (mA) kèm nhãn sức khỏe bơm, thể tích hôm nay.
- **Biểu đồ**: mức và lưu lượng theo thời gian; **thể tích theo ngày** dạng cột.
- **Bảng**: sự cố (có cột Bơm = `pump_health`), sự kiện bơm, lệnh đã gửi.
- **Điều khiển**: ẩn cho tới khi `/api/auth` trả `can_control`. Có ô mật khẩu, nút Tự động/Thủ công, Bật/Tắt bơm, Xóa lỗi, Đặt lại thể tích. Kết quả lệnh báo bằng lời dễ hiểu; bơm đang trong thời gian nghỉ thì nút hiện "Bật bơm · chờ N s".

Vòng làm mới: `/api/latest` mỗi giây, biểu đồ vài giây một lần, thể tích theo ngày 30 s.

## 7. Chạy, sửa, thử

| Việc | Lệnh |
|---|---|
| Chạy tất cả | `./start.sh` (thêm `local` để không mở link công khai) |
| Dừng | `./stop.sh` |
| Nạp firmware | `cd IOT && pio run -e main -t upload` |
| Xem log ESP32 | `pio device monitor` |
| Chạy thử không cần mạch | `backend/simulator.py` (xem `IOT/README.md`) |
| Thử một linh kiện | `pio run -e test_level -t upload` (và các `test_*` khác) |
| Nghe MQTT | `mosquitto_sub -u backend -P backend_pass -t 'wt/#' -v` |
| Phân tích dữ liệu | `tools/analyze_experiments.py`, `tools/bench.py` — xem `docs/TESTING.md` |

Muốn đổi một ngưỡng (ví dụ 30/70 %): sửa `include/config.h`, nạp lại firmware, **và** sửa `DEFAULT_CONFIG` trong `app.py` để dashboard vẽ vạch đúng.

## 8. Những chỗ dễ vấp

- `include/config.h` không có trên GitHub. Sao chép `config.example.h` thành `config.h` rồi điền Wi-Fi.
- ESP32 chỉ thấy Wi-Fi **2,4 GHz**.
- Trên máy này phải chạy Python bằng `env -u PYTHONPATH ...` vì ROS chèn thư viện của nó vào.
- ArduinoJson **cắt cụt âm thầm** khi bộ đệm nhỏ; PubSubClient **từ chối gửi âm thầm** khi gói lớn hơn `setBufferSize`. Thêm trường vào telemetry thì kiểm tra cả hai (hiện 1024 / 1200 byte).
- Link công khai của Cloudflare **đổi mỗi lần** chạy `start.sh`, xem lại trong `~/.cache/water-tank/public_url`.
- Nhật ký các lỗi phần cứng đã gặp và cách xử lý: `docs/wiring.md`. Mỗi đoạn chú thích dài trong `main.cpp` thường kể một lỗi thật đã gặp — đọc chúng trước khi "đơn giản hóa" đoạn mã đó.
