

## 0. Nạp file nào vào ESP32

Câu trả lời ngắn: **mở `water_tank/water_tank.ino` rồi bấm Upload.**

Điều dễ gây hoang mang: **file đó rỗng.** Mở ra không thấy dòng code nào là **đúng**, không phải lỗi.

Lý do: Arduino IDE không nạp một file, nó nạp **cả thư mục sketch**. Khi bạn mở `water_tank.ino`, nó tự gom mọi file cùng thư mục vào một chương trình:

```
water_tank/
├── water_tank.ino             ← file bạn MỞ (rỗng, chỉ để IDE nhận sketch)
├── water_tank_firmware.cpp    ← code thật nằm ở đây, IDE tự biên dịch kèm
└── config.h                   ← file bạn SỬA wifi và IP broker
```

Mở xong bạn sẽ thấy **ba thẻ** hiện trên đầu cửa sổ Arduino IDE, đúng ba file trên. Bấm sang thẻ `water_tank_firmware.cpp` là thấy toàn bộ phần sụn.

Vì sao code không nằm thẳng trong `.ino`: xem mục 2, đó là cách né một lỗi biên dịch có thật của Arduino IDE.

### Bảng tra nhanh

| Muốn nạp chương trình nào | Mở file này rồi bấm Upload |
|---|---|
| Phần sụn chính | `water_tank/water_tank.ino` |
| Kiểm thử cảm biến siêu âm | `test_level/test_level.ino` |
| Kiểm thử rơ le | `test_relay/test_relay.ino` |
| Kiểm thử cảm biến lưu lượng | `test_flow/test_flow.ino` |
| Kiểm thử cảm biến dòng | `test_current/test_current.ino` |
| Kiểm thử hai phao | `test_floats/test_floats.ino` |

Năm file kiểm thử **không rỗng**, code nằm thẳng trong đó.

Thứ tự đúng khi lắp mạch: nạp lần lượt năm chương trình kiểm thử theo bảy bước lắp ráp, mỗi bước chạy đúng mới sang bước sau. Xong hết mới nạp `water_tank`.

### Đối chiếu với bản PlatformIO

Bên PlatformIO bạn **không mở file nào cả**, mà chạy lệnh:

| Chương trình | Arduino IDE | PlatformIO |
|---|---|---|
| Phần sụn chính | mở `water_tank/water_tank.ino` | `pio run -e main -t upload` |
| Kiểm thử mức nước | mở `test_level/test_level.ino` | `pio run -e test_level -t upload` |

Trong VS Code, PlatformIO còn có nút mũi tên **→** ở thanh trạng thái dưới cùng, bấm là nạp môi trường mặc định (`default_envs = main` khai báo trong `platformio.ini`).

**Chỉ chọn một bản để nạp.** Hai bản tạo ra phần sụn giống nhau về hành vi, nạp chồng lên nhau không hỏng gì nhưng vô nghĩa.

---

## 1. Vì sao phải đổi cấu trúc

Arduino IDE ràng buộc ba thứ mà PlatformIO không ràng buộc:

1. Mỗi chương trình phải nằm trong một thư mục riêng, và trong đó phải có một file `.ino` **trùng tên thư mục**.
2. Arduino IDE biên dịch **mọi** file `.ino` trong thư mục thành một chương trình. Không có cơ chế `build_src_filter` như PlatformIO, nên năm chương trình kiểm thử phải tách thành năm thư mục.
3. File `.h` phải nằm ngay trong thư mục sketch. Không có `include/` dùng chung.

Vì vậy một dự án PlatformIO với sáu môi trường build biến thành **sáu thư mục sketch** riêng biệt.

---

## 2. Cái bẫy khiến đổi tên thẳng sẽ hỏng

Cách làm quen thuộc là đổi `main.cpp` thành `water_tank.ino`. **Cách đó không biên dịch được**, và đây là lý do.

Arduino IDE có một bước tiền xử lý riêng: nó tự dò các hàm trong file `.ino` rồi chèn khai báo mẫu của chúng vào ngay sau dòng `#include` cuối cùng. Trong `main.cpp`:

| Dòng | Nội dung |
|---|---|
| 12 | `#include "config.h"` — dòng include cuối |
| 70 | `struct Sample { ... }` |
| 82 | `void ringPush(const Sample& s)` |

Khai báo mẫu của `ringPush` sẽ bị chèn vào khoảng dòng 13, tức là **trước** chỗ định nghĩa `struct Sample` ở dòng 70. Trình biên dịch gặp `Sample` mà chưa biết nó là gì, và báo lỗi `'Sample' has not been declared`. Hàm `buildTelemetry` ở dòng 357 cũng vướng đúng lỗi này.

**Cách tránh:** để nguyên phần sụn trong một file `.cpp` đặt cùng thư mục sketch. Arduino IDE biên dịch file `.cpp` như C++ thuần, không đụng tới bước chèn khai báo mẫu. File `.ino` chỉ cần tồn tại cho đúng luật, và để trống.

Đó chính là lý do `water_tank.ino` trong này rỗng, còn toàn bộ mã nằm ở `water_tank_firmware.cpp`.

Năm chương trình kiểm thử thì không vướng, vì chúng không định nghĩa kiểu riêng nào và chỉ có `setup()` với `loop()`. Nên chúng được đổi thẳng sang `.ino` cho dễ đọc.

---

## 3. Cấu trúc thư mục

```
water-tank-arduino/
├── water_tank/                    chương trình chính
│   ├── water_tank.ino             rỗng, chỉ để Arduino IDE nhận sketch
│   ├── water_tank_firmware.cpp    toàn bộ phần sụn, giống hệt src/main.cpp
│   └── config.h                   FILE DUY NHẤT BẠN CẦN SỬA
├── test_level/                    bước 2, cảm biến siêu âm
├── test_relay/                    bước 3, rơ le
├── test_flow/                     bước 5, cảm biến lưu lượng
├── test_current/                  bước 6, cảm biến dòng
└── test_floats/                   bước 7, hai phao
```

Mỗi thư mục kiểm thử chứa một file `.ino` và một bản `config.h`.

---

## 4. Chuẩn bị Arduino IDE

### Bước 1. Thêm bo ESP32

Mở **File > Preferences**, ô *Additional boards manager URLs*, dán vào:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

Rồi vào **Tools > Board > Boards Manager**, tìm `esp32`, cài gói của Espressif Systems.

Chọn bo: **Tools > Board > ESP32 Arduino > ESP32 Dev Module**.

### Bước 2. Cài hai thư viện

Vào **Tools > Manage Libraries**, cài:

| Thư viện | Phiên bản | Ghi chú |
|---|---|---|
| `PubSubClient` | 2.8 | của Nick O'Leary |
| `ArduinoJson` | **6.21.6** | của Benoit Blanchon |

**Phải chọn đúng nhánh 6.x.** Bản 7.x đã đổi API và mã nguồn này sẽ không biên dịch được. Trong Library Manager, bấm vào ô phiên bản rồi chọn tay `6.21.6`.

### Bước 3. Sửa cấu hình

Mở `water_tank/config.h`, đổi ít nhất ba dòng:

```c
#define WIFI_SSID     "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD "MAT_KHAU_WIFI"
#define MQTT_HOST     "192.168.1.10"   // IP máy chạy mosquitto
```

Lưu ý: mỗi thư mục sketch có **bản `config.h` riêng**. Sửa trong `water_tank/` không tự lan sang `test_level/`. Xem mục 7 về chuyện này.

### Bước 4. Nạp

Mở `water_tank/water_tank.ino` bằng Arduino IDE, chọn cổng ở **Tools > Port**, bấm nút mũi tên **Upload**.

Làm theo đúng thứ tự bảy bước lắp ráp thì mở lần lượt `test_level`, `test_relay`, `test_flow`, `test_current`, `test_floats` trước, xong hết mới nạp `water_tank`.

---

## 5. So sánh hai bản

Cả hai đã được biên dịch thật trên cùng một mã nguồn. Số liệu đo được:

| | PlatformIO | Arduino IDE |
|---|---|---|
| Arduino-ESP32 core | **2.0.17** | **3.3.11** |
| Nền bên dưới | ESP-IDF 4.4 | ESP-IDF 5.x |
| Flash dùng | 766.777 B (58,5%) | 934.922 B (71%) |
| RAM tĩnh | 51.020 B (15,6%) | 52.980 B (16,2%) |
| Số thư mục sketch | 1 dự án, 6 môi trường | 6 thư mục riêng |
| Số bản `config.h` | 1 | 6 |
| Quản lý thư viện | khai báo trong `platformio.ini`, tự tải | bấm tay trong Library Manager |
| Ghim phiên bản thư viện | có, `^6.21.5` trong file cấu hình | không, phải nhớ chọn tay |
| Chọn chương trình để nạp | `-e test_level` | mở đúng thư mục sketch |
| Biên dịch lại | có cache, lần sau vài giây | chậm hơn, ít cache |

**Bản Arduino IDE nặng hơn 168 KB flash.** Đây không phải do mã nguồn khác nhau, mà do Arduino IDE hiện phát hành core 3.x dựng trên ESP-IDF 5.x, còn PlatformIO `espressif32@6.5.0` vẫn ghim core 2.0.17. Cả hai đều thừa chỗ trên ESP32 4 MB nên không ảnh hưởng gì thực tế.

### Nên dùng bản nào

**Arduino IDE** hợp khi bạn muốn cắm vào là chạy, không cần biết file cấu hình, hoặc khi thầy cô và bạn cùng nhóm đều quen công cụ này.

**PlatformIO** hợp hơn cho chính đồ án này, vì ba lý do cụ thể: phiên bản thư viện được ghim trong `platformio.ini` nên máy ai build cũng ra kết quả giống nhau; sáu chương trình nằm chung một cây thư mục với **một** file `config.h` duy nhất; và chuyển giữa các chương trình kiểm thử chỉ là đổi tham số `-e` thay vì mở lại thư mục khác.

---

## 6. Giữ hai bản không lệch nhau

Đây là rủi ro thật của việc có hai bản. `water_tank_firmware.cpp` là **bản sao** của `../water-tank-iot/src/main.cpp`. Sửa một bên thì bên kia không tự đổi theo.

Kiểm tra xem đã lệch chưa:

```bash
cd ~/Documents/IOT
diff water-tank-iot/src/main.cpp water-tank-arduino/water_tank/water_tank_firmware.cpp
```

Không in ra gì là hai bản còn giống nhau.

Lời khuyên: **chọn một bản làm bản chính** rồi chỉ sửa ở đó. Sửa cả hai song song là cách chắc chắn nhất để tạo ra hai phần sụn khác nhau mà không ai nhận ra.

---

## 7. Sáu bản `config.h`

Arduino IDE không cho dùng chung file header giữa các thư mục sketch, nên mỗi thư mục phải có bản riêng. Sau khi hiệu chuẩn cảm biến và sửa `water_tank/config.h`, chép sang năm thư mục còn lại:

```bash
cd ~/Documents/IOT/water-tank-arduino
for d in test_level test_relay test_flow test_current test_floats; do
  cp water_tank/config.h $d/config.h
done
```

Trên Windows PowerShell:

```powershell
cd ~\Documents\IOT\water-tank-arduino
foreach ($d in "test_level","test_relay","test_flow","test_current","test_floats") {
  Copy-Item water_tank\config.h $d\config.h -Force
}
```

Thực tế thì các chương trình kiểm thử chỉ đọc phần khai báo chân GPIO, mà phần đó hiếm khi đổi. Nên bước này chủ yếu cần khi bạn đổi sơ đồ đấu dây.

---

## 8. Đã kiểm chứng

Toàn bộ sáu sketch đã được biên dịch thật bằng `arduino-cli 1.5.1` với core `esp32:esp32@3.3.11`, `PubSubClient 2.8`, `ArduinoJson 6.21.6`, bo `ESP32 Dev Module`:

| Sketch | Kết quả | Flash |
|---|---|---|
| `water_tank` | thành công | 934.922 B (71%) |
| `test_level` | thành công | 272.344 B (20%) |
| `test_relay` | thành công | 271.624 B (20%) |
| `test_flow` | thành công | 273.804 B (20%) |
| `test_current` | thành công | 277.416 B (21%) |
| `test_floats` | thành công | 271.688 B (20%) |

Không có lỗi hay cảnh báo nào.

---

## 9. Hai thay đổi firmware mới nhất

Bản phần sụn trong thư mục này đã bao gồm hai thay đổi được thực hiện sau khi chạy thử trên phần cứng thật. Cả hai nằm trong `water_tank_firmware.cpp` và giống hệt bản PlatformIO.

**Tắt chế độ tiết kiệm điện Wi-Fi.** Một dòng `WiFi.setSleep(false)` đặt ngay sau khi kết nối mạng. Mặc định thư viện Arduino cho ESP32 bật chế độ ngủ modem, radio chỉ thức theo nhịp beacon nên gói tin phải nằm chờ. Đo thật trên cùng thiết bị, cùng access point, cùng cách đo, mỗi bên 12 mẫu:

| | Bật tiết kiệm điện | Tắt tiết kiệm điện |
|---|---|---|
| Trung vị | 462 ms | **187 ms** |
| p95 | 884 ms | **315 ms** |

Đánh đổi: dòng tiêu thụ liên tục của module tăng khoảng gấp đôi. Không đáng kể với hệ chạy điện lưới, nhưng sẽ quyết định nếu chạy pin.

**Thêm dấu thời gian mili giây.** Trường `ts_ms` bổ sung bên cạnh `ts` cũ. Trường `ts` chỉ có độ phân giải một giây nên sai số lượng tử hoá tới ±500 ms, lớn hơn cả độ trễ mạng cần đo. Trường cũ được giữ nguyên để không phá vỡ bên đọc cũ.

Hai thay đổi này làm phần sụn dài thêm khoảng 300 byte, không ảnh hưởng tới bố cục bộ nhớ.

---

## 10. Phần còn lại của hệ thống

Broker Mosquitto, backend Python và dashboard **dùng chung, không đổi gì**. Chúng nằm ở `../water-tank-iot/` và chạy theo đúng hướng dẫn trong README của dự án đó. Bản Arduino này chỉ thay phần nạp vào ESP32.
