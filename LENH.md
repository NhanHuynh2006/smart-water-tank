# Sổ tay lệnh

Mọi lệnh hay dùng gom về một chỗ. File này để cho bạn, **không nằm trong bài nộp**.

---

## 1. Nạp chương trình vào ESP32

Chạy từ thư mục `IOT`:

```bash
cd ~/Documents/IOT/IOT
```

### Đúng thứ tự bảy bước lắp ráp

Đừng lắp cả mạch rồi nạp một phát. Mỗi bước chạy đúng mới sang bước sau.

```bash
pio run -e test_calib   -t upload -t monitor   # bước 1 · ĐO hình học bồn, bơm luôn ngắt
pio run -e test_level   -t upload -t monitor   # bước 2 · cảm biến siêu âm
pio run -e test_relay   -t upload -t monitor   # bước 3 · rơ le, CHƯA nối bơm
pio run -e test_flow    -t upload -t monitor   # bước 5 · cảm biến lưu lượng
pio run -e test_current -t upload -t monitor   # bước 6 · cảm biến dòng
pio run -e test_floats  -t upload -t monitor   # bước 7 · hai phao
```

Xong hết mới nạp phần sụn chính:

```bash
pio run -e main -t upload -t monitor
```

### Các biến thể hay cần

```bash
pio run -e main                    # chỉ biên dịch, KHÔNG nạp
pio run -e main -t upload          # nạp, không mở màn hình theo dõi
pio device monitor -b 115200       # chỉ mở màn hình theo dõi
pio run -t clean                   # xoá cache build khi nghi build bẩn
pio device list                    # xem cổng nào đang có mạch
```

Thoát màn hình theo dõi: **Ctrl+C**.

### Nếu dùng Arduino IDE thay vì PlatformIO

Không có lệnh. Mở file rồi bấm nút **Upload**:

| Chương trình | Mở file này |
|---|---|
| Phần sụn chính | `water-tank-arduino/water_tank/water_tank.ino` |
| Cảm biến siêu âm | `water-tank-arduino/test_level/test_level.ino` |
| Rơ le | `water-tank-arduino/test_relay/test_relay.ino` |
| Cảm biến lưu lượng | `water-tank-arduino/test_flow/test_flow.ino` |
| Cảm biến dòng | `water-tank-arduino/test_current/test_current.ino` |
| Cực tính rơ le | `water-tank-arduino/test_relaytruth/test_relaytruth.ino` |
| Nhiễu dây lưu lượng | `water-tank-arduino/test_flownoise/test_flownoise.ino` |
| Lưu lượng có đấu ngược | `water-tank-arduino/test_flowmap/test_flowmap.ino` |
| Bơm phá siêu âm | `water-tank-arduino/test_pumpecho/test_pumpecho.ino` |
| Hai phao | `water-tank-arduino/test_floats/test_floats.ino` |
| Đo hình học bồn | `water-tank-arduino/test_calib/test_calib.ino` |

`water_tank.ino` **rỗng là đúng**, code nằm ở `water_tank_firmware.cpp` cùng thư mục.

### Đặt lại ngưỡng cho đúng bồn của bạn

Ba chương trình dưới đây **không bao giờ bật bơm**, rơ le bị giữ ngắt suốt, và đều **tự dừng hẳn** sau khi chạy xong. Chạy chúng trước khi nạp phần sụn chính.

| Việc cần biết | Chạy lệnh | Sửa gì trong `include/config.h` |
|---|---|---|
| Cảm biến siêu âm cách đáy bồn bao nhiêu | `pio run -e test_calib -t upload -t monitor` | `TANK_SENSOR_TO_BOTTOM_CM` |
| Hai phao đóng hay mở khi kích hoạt | `pio run -e test_floats -t upload -t monitor` | `FLOAT_MAX_ACTIVE_LOW`, `FLOAT_MIN_ACTIVE_LOW` |
| **Rơ le kích mức cao hay mức thấp** | `pio run -e test_relaytruth -t upload -t monitor` | `RELAY_ACTIVE_LOW` |
| **Dây lưu lượng có sạch chưa** | `pio run -e test_flownoise -t upload -t monitor` | `FLOW_SENSOR_ENABLED` |
| **Hai cảm biến lưu lượng có bị đấu ngược** | `pio run -e test_flowmap -t upload -t monitor` | `PIN_FLOW`, `PIN_FLOW_OUT` |
| **Bơm làm hỏng siêu âm bằng điện hay bằng nước** | `pio run -e test_pumpecho -t upload -t monitor` | cầu chia áp ECHO |
| Hệ số K của cảm biến lưu lượng | `pio run -e test_flow -t upload -t monitor` | `FLOW_K_FACTOR`, `FLOW_OUT_K_FACTOR` |

**Cách đọc `test_calib`**: đo bằng thước chiều cao mặt nước hiện tại, rồi lấy con số *khoảng cách trung bình* chương trình in ra cộng với chiều cao vừa đo. Tổng đó chính là `TANK_SENSOR_TO_BOTTOM_CM`. Nếu mọi dòng đều hiện `NGOÀI DẢI!` thì cảm biến đang gắn sai chỗ so với con số trong `config.h`.

**Cách đọc `test_floats`**: nhấc tay từng phao lên hết cỡ. Dòng chữ phải đổi thành `DA KICH HOAT`. Nếu nó đổi ngược lại, đảo `FLOAT_MAX_ACTIVE_LOW` hoặc `FLOAT_MIN_ACTIVE_LOW` giữa `0` và `1` rồi nạp lại.

**Cách đọc `test_relaytruth`**: đọc cột **độ gợn**, đừng đọc mức trung bình. ACS712 là cảm biến hai chiều nên mV cao hơn không có nghĩa là dòng lớn hơn. Động cơ chổi than chạy thì dòng gợn mạnh; tỉ số gợn trên 2 lần là tách được hai trạng thái.

**Cách đọc `test_flownoise`**: cột **kéo lên 3,3 V** phải cho **0 Hz** ở cả hai chân, giống chân đối chiếu GPIO 23. Còn thấy 50 Hz là điện lưới cảm ứng — thiếu điện trở 4,7 kΩ lên 3,3 V.

**Cách đọc `test_pumpecho`**: nhìn 12 lần phát đầu của đoạn rơ le đóng. Bơm chỉ đẩy 0,06 cm/s nên trong 0,4 giây nước chỉ kịp lên 0,024 cm. Hỏng ngay từ lần phát thứ 1–2 là **nhiễu điện**; hỏng dần sau vài giây là **nước**.

---

## 2. Chạy hệ thống và mở dashboard

```bash
cd ~/Documents/IOT

./start.sh          # bật broker + backend, tự mở trình duyệt
./start.sh sim      # bật thêm ESP32 ảo, khi chưa cắm mạch thật
./start.sh sim 30   # ESP32 ảo chạy nhanh 30 lần
./stop.sh           # tắt hết
./stop.sh sim       # chỉ tắt ESP32 ảo, giữ broker và backend
```

Dashboard: **http://localhost:8000/** · Tài liệu API: **http://localhost:8000/docs**

### Cho người khác cùng xem

Người trong cùng Wi-Fi mở **`http://<IP máy chủ>:8000/`** — `./start.sh` in sẵn địa chỉ này ở dòng *Trong Wi-Fi*.

| Ai | Làm được gì |
|---|---|
| Chính máy chủ (máy chạy backend) | xem **và** điều khiển |
| Mọi máy khác trong mạng | **chỉ xem** — không thấy nút bấm, gửi lệnh thẳng cũng bị từ chối 403 |
| Ai có mã, qua `./start.sh public` | điều khiển từ xa qua Internet |

Máy chủ được nhận ra bằng địa chỉ IP của chính nó, nên đổi Wi-Fi hay đổi IP vẫn đúng mà không phải cấu hình gì.

Log nằm ở `~/.cache/water-tank/`.

### Nếu muốn chạy tay từng cửa sổ

```bash
# cửa sổ 1 · broker
cd ~/Documents/IOT/IOT/mosquitto && mosquitto -c mosquitto.conf -v

# cửa sổ 2 · backend
cd ~/Documents/IOT/IOT/backend && env -u PYTHONPATH .venv/bin/python app.py

# cửa sổ 3 · ESP32 ảo
cd ~/Documents/IOT/IOT/backend && env -u PYTHONPATH .venv/bin/python simulator.py --speed 10
```

`env -u PYTHONPATH` là **bắt buộc**: máy này có ROS đặt biến đó, không gỡ thì thư viện ROS che mất thư viện của venv.

### Các chế độ của ESP32 ảo

```bash
python simulator.py                  # thời gian thật
python simulator.py --speed 30       # nhanh 30 lần
python simulator.py --fault dryrun   # bơm chạy mà không lên nước
python simulator.py --fault stuck    # cảm biến mức kẹt số
python simulator.py --fault leak     # rò rỉ khi bơm đã tắt
python simulator.py --sweep          # quét tham số cho thí nghiệm E8, không cần broker
```

---

## 3. Mở ba thứ "web"

| Thứ | Lệnh hoặc link | Cần chạy dịch vụ |
|---|---|---|
| Dashboard | `http://localhost:8000/` | **có**, chạy `./start.sh` trước |
| Sơ đồ kiến trúc | `xdg-open ~/Documents/IOT/IOT/docs/architecture.html` | không |
| Sơ đồ đấu dây | https://claude.ai/code/artifact/e15f02da-07c9-49da-8274-32289c013124 | không |

Bản đấu dây còn có file cục bộ, mở offline được nhưng **sửa không lưu được**:

```bash
xdg-open ~/Documents/IOT/schematic-canvas/so-do-dau-day-bon-nuoc.html
```

Trong Claude Code, gõ `/artifacts` để liệt kê mọi sơ đồ đã lưu, hoặc **Ctrl+]** mở cái gần nhất.

---

## 4. Báo cáo LaTeX

```bash
cd ~/Documents/IOT/iot-report-en
latexmk -pdf main.tex     # build, tự chạy bibtex và lặp đủ số lượt
latexmk -c                # dọn file trung gian, giữ main.pdf
xdg-open main.pdf
```

---

## 5. Dò lỗi

```bash
# xem mạch có được nhận không
ls -l /dev/ttyUSB*
lsusb | grep -i "ch34\|cp210"

# đọc thông tin chip, chỉ đọc, không ghi flash
~/.arduino15/packages/esp32/tools/esptool_py/5.3.1/esptool --port /dev/ttyUSB0 flash-id

# xem bản tin MQTT thô
mosquitto_sub -h localhost -u backend -P backend_pass -t 'wt/lab1/#' -v

# tình trạng backend và độ trễ
curl -s localhost:8000/api/health | python3 -m json.tool
curl -s localhost:8000/api/stats/latency | python3 -m json.tool

# gửi lệnh tay
curl -X POST localhost:8000/api/command \
  -H 'Content-Type: application/json' -d '{"action":"mode","value":false}'
```

### Lỗi hay gặp

| Hiện tượng | Cách xử lý |
|---|---|
| `pio: command not found` | đã tạo liên kết ở `~/.local/bin/pio`; nếu mất thì `ln -sf ~/.platformio/penv/bin/pio ~/.local/bin/pio` |
| Không thấy `/dev/ttyUSB0` | rút cắm lại; nếu vẫn không thì `sudo apt purge brltty` |
| `Failed to connect to ESP32` khi nạp | giữ nút **BOOT** trên bo ngay lúc lệnh nạp bắt đầu, thả khi thấy `Connecting...` |
| Cổng bận, nạp không được | tắt màn hình theo dõi đang mở, hoặc `./stop.sh` |
| Dashboard hiện bản cũ | Ctrl+Shift+R một lần |
| Backend `Connection refused` | broker chưa chạy, chạy `./start.sh` |
| Thư viện Python lạ, lỗi import | quên `env -u PYTHONPATH`, xem mục 2 |

---

## 6. Đóng gói nộp bài

```bash
cd ~/Documents/IOT
ls -lh *.zip
```

Ba file: `water-tank-iot.zip`, `water-tank-arduino.zip`, `iot-report-en.zip`.

**Nhắc**: trong zip, `config.h` đã được thay về giá trị mẫu, không lộ mật khẩu Wi-Fi. Trên đĩa vẫn là giá trị thật để chạy được. Nếu bạn tự nén lại thì phải tự thay, nếu không mật khẩu sẽ đi theo bài nộp.
