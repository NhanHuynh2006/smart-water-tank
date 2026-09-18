# Hệ thống bồn nước thông minh

Mã nguồn đầy đủ cho đồ án cuối kỳ môn IoT Foundations and Applications: giám sát mức nước và lưu lượng, điều khiển bơm tự động, phát hiện sự cố và giao diện giám sát thời gian thực.

Điểm quan trọng nhất của thiết kế: **toàn bộ luật điều khiển và luật phát hiện sự cố nằm trong ESP32**, không nằm trên máy chủ. Nhờ vậy bồn vẫn được cấp đầy đúng ngưỡng khi mất mạng, chỉ mất khả năng giám sát từ xa.

---

## 1. Bạn cần chuẩn bị gì

| Thứ | Ghi chú |
|---|---|
| Một máy tính (Linux, macOS hoặc Windows) | chạy broker, backend và dashboard |
| Python 3.9 trở lên | `python3 --version` để kiểm tra, trên Windows là `python --version` |
| Mosquitto | broker MQTT |
| ESP32 DevKit + phần cứng | **chỉ cần khi chạy thật**, phần mô phỏng không cần |
| VS Code + tiện ích PlatformIO | chỉ cần khi nạp phần sụn |

**Bạn có thể chạy thử toàn bộ hệ thống mà chưa cần mua linh kiện nào.** Chương trình mô phỏng đóng vai một ESP32 ảo, phát bản tin đúng theo đặc tả.

---

## 2. Chạy thử trong 5 phút trên Linux hoặc macOS, không cần phần cứng

### Bước 1. Cài Mosquitto

```bash
# Ubuntu / Debian
sudo apt update && sudo apt install -y mosquitto mosquitto-clients

# macOS
brew install mosquitto

# Windows: xem phần 3, các bước khác hẳn
```

Sau khi cài, Ubuntu thường tự chạy Mosquitto như một dịch vụ nền. Hãy tắt nó đi vì ta sẽ chạy bằng file cấu hình riêng:

```bash
sudo systemctl stop mosquitto
sudo systemctl disable mosquitto
```

### Bước 2. Tạo tài khoản cho broker

```bash
cd mosquitto
mosquitto_passwd -c -b passwd device1 device_pass
mosquitto_passwd    -b passwd backend backend_pass
```

Trên Linux, Mosquitto chạy dưới người dùng `mosquitto` nên cần cấp quyền đọc hai file:

```bash
sudo chown mosquitto:mosquitto passwd aclfile
```

### Bước 3. Chạy broker

Mở **cửa sổ terminal thứ nhất**, để nguyên đó:

```bash
cd mosquitto
mosquitto -c mosquitto.conf -v
```

### Bước 4. Cài thư viện Python và chạy backend

Mở **cửa sổ terminal thứ hai**:

```bash
cd backend
python3 -m venv .venv
source .venv/bin/activate          # Windows: .venv\Scripts\activate
pip install -r requirements.txt
python app.py
```

Thấy dòng `Dashboard: http://localhost:8000/` là được.

### Bước 5. Chạy ESP32 ảo

Mở **cửa sổ terminal thứ ba**:

```bash
cd backend
source .venv/bin/activate
python simulator.py
```

### Bước 6. Mở giao diện

Vào trình duyệt, gõ **http://localhost:8000/**

Bạn sẽ thấy mức nước dao động lên xuống giữa hai đường ngưỡng 30% và 80%, bơm tự bật tắt theo.

---

## 3. Chạy thử trên Windows, không cần phần cứng

Các bước giống hệt phần 2 về mặt ý tưởng, chỉ khác cách gõ lệnh. Dùng **PowerShell**, đừng dùng Command Prompt cũ.

### Bước 1. Cài Python

Tải tại https://www.python.org/downloads/ . Trong màn hình cài đặt, **bắt buộc tích ô `Add python.exe to PATH`**. Bỏ sót ô này thì mọi lệnh phía sau đều báo không tìm thấy.

Kiểm tra:

```powershell
python --version
```

Nếu Windows mở Microsoft Store thay vì in ra số phiên bản, vào `Settings > Apps > Advanced app settings > App execution aliases` rồi tắt hai mục `python.exe` và `python3.exe`.

### Bước 2. Cài Mosquitto

Tải bản cài đặt 64 bit tại https://mosquitto.org/download/ , cài vào đường dẫn mặc định `C:\Program Files\mosquitto`.

Bản cài đặt tự đăng ký một **dịch vụ nền chạy sẵn và chiếm cổng 1883**. Ta chạy broker bằng file cấu hình riêng nên phải tắt dịch vụ đó. Mở PowerShell **quyền quản trị** (chuột phải vào biểu tượng, chọn `Run as administrator`):

```powershell
net stop mosquitto
sc.exe config mosquitto start= disabled
```

Bản cài đặt cũng không tự thêm vào PATH. Trong **mỗi** cửa sổ PowerShell có gõ lệnh `mosquitto`, chạy trước dòng này:

```powershell
$env:Path += ";C:\Program Files\mosquitto"
```

Muốn thêm vĩnh viễn thì vào `Settings > System > About > Advanced system settings > Environment Variables` và bổ sung đường dẫn trên vào biến `Path`.

### Bước 3. Tạo tài khoản cho broker

```powershell
cd <duong-dan-du-an>\water-tank-iot\mosquitto
mosquitto_passwd -c -b passwd device1 device_pass
mosquitto_passwd    -b passwd backend backend_pass
```

Windows không có người dùng hệ thống tên `mosquitto`, nên **bỏ qua hoàn toàn bước phân quyền `chown`** của phần Linux.

### Bước 4. Chạy broker

Cửa sổ PowerShell **thứ nhất**, mở rồi để nguyên đó:

```powershell
$env:Path += ";C:\Program Files\mosquitto"
cd <duong-dan-du-an>\water-tank-iot\mosquitto
mosquitto -c mosquitto.conf -v
```

Phải đứng đúng trong thư mục `mosquitto`, vì file cấu hình trỏ tới `passwd` và `aclfile` bằng đường dẫn tương đối.

### Bước 5. Cài thư viện Python và chạy backend

Cửa sổ PowerShell **thứ hai**:

```powershell
cd <duong-dan-du-an>\water-tank-iot\backend
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
python app.py
```

Nếu PowerShell báo `running scripts is disabled on this system`, chạy dòng sau rồi kích hoạt lại:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

Cách khác, khỏi cần kích hoạt môi trường ảo, gọi thẳng trình thông dịch nằm bên trong nó:

```powershell
.\.venv\Scripts\python.exe app.py
```

Lần chạy đầu tiên, Windows Defender Firewall sẽ hỏi có cho phép Python mở cổng mạng không. Chọn **Allow** cho mạng riêng tư. Việc này chỉ cần khi ESP32 thật phải kết nối tới máy; nếu chỉ xem dashboard ngay trên máy đó thì bấm từ chối vẫn chạy bình thường.

### Bước 6. Chạy ESP32 ảo

Cửa sổ PowerShell **thứ ba**:

```powershell
cd <duong-dan-du-an>\water-tank-iot\backend
.\.venv\Scripts\Activate.ps1
python simulator.py
```

### Bước 7. Mở giao diện

Vào trình duyệt, gõ **http://localhost:8000/**

Từ đây trở đi, mọi thứ trong phần 4 đều áp dụng được, chỉ cần thay `python3` bằng `python`.

### Bảng đối chiếu lệnh

| Việc cần làm | Linux, macOS | Windows PowerShell |
|---|---|---|
| Gọi Python | `python3` | `python` hoặc `py` |
| Kích hoạt môi trường ảo | `source .venv/bin/activate` | `.\.venv\Scripts\Activate.ps1` |
| Tắt dịch vụ broker nền | `sudo systemctl stop mosquitto` | `net stop mosquitto`, cần quyền quản trị |
| Phân quyền file mật khẩu | `sudo chown mosquitto:mosquitto passwd aclfile` | không cần |
| Xem địa chỉ IP của máy | `ip addr` | `ipconfig` |
| Cổng nối ESP32 | `/dev/ttyUSB0` | `COM3`, `COM5`... |
| Dấu ngăn dòng lệnh dài | `\` | `` ` `` |

### Hai cái bẫy về dấu nháy

PowerShell hiểu dấu nháy khác shell của Linux, nên hai lệnh hay dùng trong tài liệu này phải viết lại.

Theo dõi bản tin thô, phải dùng nháy **kép**, vì nháy đơn kèm ký tự `#` sẽ bị hiểu sai:

```powershell
mosquitto_sub -h localhost -u backend -P backend_pass -t "wt/lab1/#" -v
```

Gửi lệnh qua API thì dùng `Invoke-RestMethod`, vì `curl` trong PowerShell chỉ là bí danh của `Invoke-WebRequest` và không hiểu cú pháp `-X`, `-d`:

```powershell
Invoke-RestMethod -Uri http://localhost:8000/api/command -Method Post `
  -ContentType 'application/json' `
  -Body '{"action":"mode","value":false}'
```

Muốn dùng đúng `curl` thật thì gọi `curl.exe`, đừng gọi `curl`.

---

## 4. Những thứ nên thử ngay

### Thử cơ chế lệnh và xác nhận

Bấm nút **Bật bơm** khi đang ở chế độ tự động. Lệnh sẽ bị từ chối với lý do `auto_mode_active` hiện trong bảng nhật ký lệnh. Đây là bằng chứng chế độ thủ công không vượt qua được rào an toàn.

Bấm **Chế độ thủ công** rồi **Bật bơm**, lệnh sẽ được chấp nhận, kèm độ trễ tính bằng mi li giây.

### Thử tiêm lỗi

Dừng simulator bằng `Ctrl+C` rồi chạy lại với một trong ba chế độ lỗi:

```bash
python simulator.py --fault dryrun   # bơm chạy nhưng không lên nước
python simulator.py --fault stuck    # cảm biến mức kẹt giá trị
python simulator.py --fault leak     # rò rỉ 0,3 L/phút khi bơm đã tắt
```

Với `dryrun`, sau khoảng 6 giây bạn sẽ thấy mã `DRY_RUN` xuất hiện trong bảng nhật ký sự cố và bơm bị khóa lại.

### Chạy nhanh hơn thời gian thật

```bash
python simulator.py --speed 30
```

Một giờ mô phỏng chỉ mất hai phút, tiện để quan sát nhiều chu kỳ đầy vơi.

### Quét tham số điều khiển, phục vụ thí nghiệm E8

Lệnh này không cần broker, chỉ chạy mô hình thuỷ lực:

```bash
python simulator.py --sweep
```

Kết quả mẫu chạy thật trên mô hình:

```
      nguong   rang buoc TG   dong cat/gio   bom chay (s)  ngoai dai (s)
------------------------------------------------------------------------
      49/51%          khong          472.0          630.8            0.0
      49/51%             co          232.0          624.8            0.0
      45/55%          khong           46.0          627.4            0.0
      40/60%          khong           22.0          602.2            0.0
      30/80%             co            8.0          617.4            0.0
      20/90%             co            6.0          600.0          259.0
------------------------------------------------------------------------
Ti le giam eta: 98.3 %
```

Đây chính là con số chứng minh giá trị của vùng trễ: giảm 98,3% số lần đóng cắt mà thời gian nằm ngoài dải mục tiêu vẫn bằng 0. Cấu hình 20/90 cho thấy điểm đánh đổi bắt đầu xuất hiện.

---

## 5. Chạy với phần cứng thật

### Bước 1. Đấu dây

Xem `docs/wiring.md`. Tóm tắt các chân:

| Chân ESP32 | Nối tới | Lưu ý |
|---|---|---|
| GPIO 5 | TRIG cảm biến siêu âm | |
| GPIO 18 | ECHO cảm biến siêu âm | **bắt buộc qua chia áp 10k/20k** |
| GPIO 4 | Xung cảm biến lưu lượng **đầu vào** | xem ghi chú về chia áp bên dưới |
| GPIO 19 | Xung cảm biến lưu lượng **đầu ra** | xem ghi chú về chia áp bên dưới |
| GPIO 26 | Chân IN của module rơ le | |
| GPIO 34 | Ngõ ra ACS712 | **bắt buộc qua chia áp 10k/10k** |
| GPIO 27 | Phao mức cao | dùng điện trở kéo lên nội bộ |
| GPIO 14 | Phao bồn nguồn | dùng điện trở kéo lên nội bộ |
| GPIO 33 | Nút xóa lỗi | dùng điện trở kéo lên nội bộ |

Ba việc **không được bỏ qua**:

1. Điốt 1N4007 mắc song song ngược hai cực bơm, vạch dấu về phía cực dương. Thiếu nó, xung điện áp ngược khi ngắt bơm sẽ phá hỏng mạch.
2. Tụ gốm 100 nF hàn ngay tại hai cực động cơ để dập nhiễu chổi than.
3. Tụ hóa 1000 µF giữa đường 5V và đất, đặt sát chân cấp nguồn ESP32, chống sụt áp lúc bơm khởi động.

### Bước 2. Sửa file cấu hình

Mở `include/config.h`. **Đây là file duy nhất bạn cần sửa.** Ít nhất phải đổi:

```c
#define WIFI_SSID     "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD "MAT_KHAU_WIFI"
#define MQTT_HOST     "192.168.1.10"   // IP máy đang chạy mosquitto
```

Tìm IP máy tính bằng `ip addr` trên Linux, `ipconfig` trên Windows, `ifconfig` trên macOS.

Đo và sửa tiếp hai thông số hình học:

```c
#define TANK_SENSOR_TO_BOTTOM_CM 55.0f  // đo từ mặt cảm biến xuống đáy bồn
#define TANK_MAX_LEVEL_CM        25.0f  // chiều cao cột nước khi đầy
```

### Bước 3. Nạp phần sụn theo bảy bước

Đừng lắp cả mạch rồi nạp một lần. Làm theo thứ tự, mỗi bước phải chạy đúng mới sang bước sau:

```bash
pio run -e test_level   -t upload -t monitor   # bước 2: cảm biến siêu âm
pio run -e test_relay   -t upload -t monitor   # bước 3: rơ le, CHƯA nối bơm
pio run -e test_flow    -t upload -t monitor   # bước 5: cảm biến lưu lượng
pio run -e test_current -t upload -t monitor   # bước 6: cảm biến dòng
pio run -e test_floats  -t upload -t monitor   # bước 7: hai phao
```

Xong hết mới nạp chương trình chính:

```bash
pio run -e main -t upload -t monitor
```

Nếu chưa có PlatformIO, cài tiện ích PlatformIO IDE trong VS Code, hoặc cài dòng lệnh:

```bash
pipx install platformio     # dùng pipx vì Python hệ thống chặn pip
```

### Bước 4. Lỗi thường gặp trên Linux

Nếu không thấy cổng `/dev/ttyUSB0`:

```bash
sudo apt purge brltty              # brltty chiếm cổng CH340, phải gỡ
sudo usermod -aG dialout $USER     # rồi đăng xuất và đăng nhập lại
```

Driver CH340 và CP210x đã có sẵn trong nhân Linux hiện đại, **không cần tải driver từ đâu cả**.

### Bước 5. Lỗi thường gặp trên Windows

Ngược với Linux, Windows **không có sẵn driver** cho hai loại mạch nạp USB thường gặp trên bo ESP32, phải tải về và cài:

- CH340, CH341: https://www.wch-ic.com/downloads/CH341SER_EXE.html
- CP210x: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

Cắm ESP32 rồi mở `Device Manager`, xem mục `Ports (COM & LPT)`. Thấy dòng kiểu `USB-SERIAL CH340 (COM5)` là nhận đúng, và `COM5` chính là cổng cần dùng. Thấy thiết bị kèm dấu chấm than vàng nghĩa là driver chưa cài xong.

PlatformIO thường tự dò ra cổng. Nếu dò sai, hoặc bạn cắm nhiều thiết bị nối tiếp cùng lúc, chỉ định thẳng bằng cách thêm hai dòng sau vào mục `[env]` trong `platformio.ini`:

```ini
upload_port  = COM5
monitor_port = COM5
```

Cài PlatformIO dòng lệnh trên Windows thì dùng thẳng pip, không cần pipx như trên Linux:

```powershell
pip install platformio
```

Nếu nạp báo `Failed to connect to ESP32: Timed out waiting for packet header`, hãy giữ nút `BOOT` trên bo ngay khi lệnh nạp bắt đầu, thả ra lúc thấy dòng `Connecting...`.

---

## 6. Hiệu chuẩn cảm biến

Hai thí nghiệm này cho ra bốn con số cần điền lại vào `config.h`.

### E1, hiệu chuẩn cảm biến mức

Dán thước chia vạch lên thành bồn. Đổ nước dừng ở mười mức đều nhau. Tại mỗi mức, đọc thước và ghi ba lần giá trị hiển thị trên dashboard, cách nhau năm giây.

Hồi quy tuyến tính giữa giá trị thước và giá trị đo cho ra hệ số góc và độ lệch không:

```c
#define LEVEL_CAL_A  1.02f
#define LEVEL_CAL_B -0.35f
```

### E2, hiệu chuẩn cảm biến lưu lượng

Đặt bình hứng lên cân, trừ bì. Bật bơm đúng 60 giây. Ghi khối lượng nước thu được và tổng số xung (chạy `test_flow` để thấy số xung).

```
K = tổng số xung / (60 × thể tích tính bằng lít)
```

Điền vào:

```c
#define FLOW_K_FACTOR  7.5f
```

Trong báo cáo, đừng chỉ công bố giá trị K. Hãy công bố **độ lệch so với giá trị danh định của nhà sản xuất**, vì đó mới là bằng chứng bạn thật sự hiệu chuẩn chứ không chép datasheet.

---

## 7. Cấu trúc thư mục

```
├── platformio.ini          cấu hình build, 6 môi trường nạp
├── include/config.h        FILE DUY NHẤT BẠN CẦN SỬA
├── src/
│   ├── main.cpp            phần sụn chính: FSM, 7 luật lỗi, MQTT, bộ đệm offline
│   └── tests/              5 chương trình kiểm thử đơn vị theo 7 bước lắp ráp
├── backend/
│   ├── app.py              thu nhận MQTT + SQLite + REST API + phát hiện rò rỉ
│   ├── simulator.py        ESP32 ảo, mô hình thuỷ lực, chế độ quét tham số
│   └── requirements.txt
├── dashboard/index.html    bảng điều khiển một file, không cần build
├── mosquitto/              cấu hình broker kèm phân quyền ACL
└── docs/
    ├── protocol.md         đặc tả cây chủ đề và cấu trúc bản tin
    ├── wiring.md           sơ đồ đấu dây chi tiết
    ├── schematic-spec.md   đặc tả đầy đủ để vẽ sơ đồ nguyên lý
    └── architecture.html   sơ đồ kiến trúc tương tác, mở bằng trình duyệt
```

---

## 8. Tập điểm cuối API

Mở http://localhost:8000/docs để xem tài liệu tự sinh và thử trực tiếp.

| Phương thức | Đường dẫn | Chức năng |
|---|---|---|
| GET | `/api/health` | tình trạng backend và kết nối broker |
| GET | `/api/latest` | trạng thái mới nhất, tuổi dữ liệu, lệnh đang chờ |
| GET | `/api/telemetry?minutes=10` | chuỗi thời gian |
| GET | `/api/volume/daily?days=7` | thể tích tiêu thụ theo ngày |
| GET | `/api/faults` | nhật ký sự cố |
| GET | `/api/events` | nhật ký đóng cắt bơm |
| GET | `/api/commands` | lệnh đã gửi kèm độ trễ xác nhận |
| POST | `/api/command` | gửi lệnh, trả về mã định danh |
| GET, PUT | `/api/config` | đọc và sửa ngưỡng |
| GET | `/api/stats/latency` | thống kê độ trễ, phục vụ thí nghiệm E7 |

Gửi lệnh bằng dòng lệnh:

```bash
curl -X POST localhost:8000/api/command \
  -H 'Content-Type: application/json' \
  -d '{"action":"mode","value":false}'
```

Trên Windows PowerShell, lệnh tương đương là:

```powershell
Invoke-RestMethod -Uri http://localhost:8000/api/command -Method Post `
  -ContentType 'application/json' `
  -Body '{"action":"mode","value":false}'
```

---

## 9. Bảy luật phát hiện sự cố

| Mã | Điều kiện kích hoạt | Hành động |
|---|---|---|
| `SENSOR_TIMEOUT` | không có số đọc mức hợp lệ trong 4 giây | tắt bơm, tự phục hồi |
| `SENSOR_CONFLICT` | phao mức cao kích nhưng siêu âm báo dưới 70% | tắt bơm, tự phục hồi |
| `OVERFLOW` | mức vượt 95% hoặc phao mức cao kích | tắt bơm, khóa |
| `DRY_RUN` | bơm bật 6 giây mà lưu lượng dưới 0,25 L/phút | tắt bơm, khóa |
| `NO_CURRENT` | bơm bật quá 2 giây mà không có dòng điện | tắt bơm, khóa |
| `LEAK_SUSPECTED` | bơm tắt mà lưu lượng vượt 0,20 L/phút trong 60 giây | cảnh báo |
| `FILL_TIMEOUT` | bơm chạy quá 180 giây mà chưa đầy | tắt bơm, khóa |

Ngoài ra backend chạy luật thứ tám là `LEAK_BASELINE`, so mức tiêu thụ với đường nền thống kê theo 48 khe 30 phút trong ngày.

Nhóm khóa yêu cầu người vận hành bấm **Xóa lỗi** sau khi đã xử lý nguyên nhân vật lý. Nhóm tự phục hồi tự hết khi tín hiệu trở lại bình thường liên tục 5 giây.

---

## 10. Rào an toàn

Mọi đường dẫn có thể bật bơm trong `main.cpp` đều bắt buộc đi qua đúng một hàm:

```c
const char* pumpBlockReason();
```

Hàm trả về `nullptr` nếu được phép, hoặc mã lý do nếu bị chặn. Sáu điều kiện chặn: đang có sự cố chưa xóa, phao mức cao đang kích, mức vượt ngưỡng an toàn, phép đo mức không đáng tin, bồn nguồn đã cạn, chưa hết thời gian nghỉ tối thiểu.

Nhờ vậy không thể vô tình tạo ra lối đi vòng. Muốn kiểm chứng rào an toàn, bạn chỉ cần đọc một hàm thay vì rà toàn bộ mã nguồn.

Bốn lớp phòng vệ chống tràn hoạt động độc lập:

1. Ngưỡng cao của vùng trễ, trong máy trạng thái
2. Ngưỡng an toàn 95%, trong luật phát hiện sự cố
3. Phao cơ khí mức cao, vẫn hiệu lực khi cảm biến siêu âm hỏng
4. Rơ le mặc định ở trạng thái ngắt, vẫn hiệu lực khi vi điều khiển mất nguồn

---

## 11. Xử lý sự cố

| Hiện tượng | Nguyên nhân và cách khắc phục |
|---|---|
| `Error opening password file` khi chạy mosquitto | chưa chạy `mosquitto_passwd`, hoặc file `passwd` không được người dùng `mosquitto` đọc được. Chạy `sudo chown mosquitto:mosquitto passwd aclfile` |
| Backend báo `Connection refused` | broker chưa chạy, hoặc sai `MQTT_HOST` |
| Dashboard trống, tuổi dữ liệu tăng dần | thiết bị hoặc simulator không phát. Kiểm tra bằng `mosquitto_sub -h localhost -u backend -P backend_pass -t 'wt/lab1/#' -v` |
| Lệnh gửi đi mà không có xác nhận | thiết bị không đăng ký được chủ đề lệnh. Kiểm tra `aclfile` và tài khoản trong `config.h` |
| ESP32 khởi động lại mỗi khi bơm bật | sụt áp. Thiếu tụ 1000 µF, hoặc nguồn không đủ dòng |
| Cảm biến siêu âm không có echo | thiếu chia áp trên chân ECHO, hoặc mặt nước nằm trong vùng chết 25 cm |
| Cảm biến lưu lượng luôn báo 0 | bơm quá yếu so với dải đo của cảm biến, hoặc lắp nghiêng làm tuabin kẹt |
| Số đọc mức nhảy loạn khi bơm chạy | thiếu tụ gốm 100 nF tại cực động cơ, nhiễu chổi than lan theo đường nguồn |
| Windows báo `mosquitto` không phải là lệnh nhận dạng được | chưa thêm `C:\Program Files\mosquitto` vào PATH. Chạy `$env:Path += ";C:\Program Files\mosquitto"` trong chính cửa sổ đó |
| Windows báo cổng 1883 đang bị chiếm | dịch vụ Mosquitto nền vẫn chạy. Chạy `net stop mosquitto` trong PowerShell quyền quản trị |
| PowerShell báo `running scripts is disabled on this system` | chạy `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass` rồi kích hoạt lại môi trường ảo, hoặc gọi thẳng `.\.venv\Scripts\python.exe app.py` |
| Windows gõ `python` thì mở Microsoft Store | tắt bí danh trong `Settings > Apps > Advanced app settings > App execution aliases` |
| `pio: command not found` dù đã cài tiện ích PlatformIO trong VS Code | tiện ích không đưa `pio` ra PATH của shell. Tạo liên kết một lần: `ln -sf ~/.platformio/penv/bin/pio ~/.local/bin/pio` (Linux, macOS), hoặc chạy lệnh nạp từ terminal tích hợp của VS Code |
| Nạp báo `Failed to connect to ESP32` | giữ nút `BOOT` trên bo ngay khi lệnh nạp bắt đầu, thả ra lúc thấy `Connecting...` |
| Cổng bận, không nạp được | còn một màn hình theo dõi đang mở giữ cổng. Đóng nó bằng `Ctrl+C` rồi nạp lại |

---

## 12. Ghi chú về độ trễ trong bản mô phỏng

Khi chạy simulator, `/api/stats/latency` sẽ cho trung vị khoảng 500 ms. Con số này **không phải độ trễ mạng thật**. Nguyên nhân là simulator sinh dấu thời gian ở độ phân giải một giây, nên hiệu giữa hai dấu thời gian phân bố đều trong khoảng 0 tới 1000 ms.

Trên phần cứng thật, ESP32 đồng bộ thời gian qua giao thức thời gian mạng và độ trễ đo được sẽ nhỏ hơn nhiều. Vì vậy số liệu cho thí nghiệm E7 trong báo cáo **bắt buộc phải đo trên phần cứng thật**, không được lấy từ mô phỏng.

Ngược lại, độ trễ lệnh khứ hồi trong bảng nhật ký lệnh là số thật ngay cả khi mô phỏng, vì nó đo bằng đồng hồ của chính máy chủ ở cả hai đầu.

## 13. web xem kiến trúc 
```c
cd ~/Documents/IOT/water-tank-iot
xdg-open docs/architecture.html
```