# Bản mô tả để vẽ sơ đồ nguyên lý và sơ đồ đi dây

Tài liệu này là **đề bài đầy đủ** để đưa cho công cụ vẽ sơ đồ. Mọi con số lấy trực tiếp từ `include/config.h` và `src/main.cpp` của dự án, không phải giá trị giả định.

Cách dùng: sao chép **toàn bộ** nội dung dưới đây, dán cho công cụ vẽ kèm câu *"Vẽ sơ đồ nguyên lý và sơ đồ đi dây theo đúng đặc tả này"*.

**Cấu hình đã chốt:** bơm **5 V**, nguồn gốc **12 V**, hạ áp bằng mạch buck 12 V xuống 5 V 5 A, **hai cảm biến lưu lượng** (một đầu vào, một đầu ra).

---

## 1. Danh mục linh kiện

| Ký hiệu | Linh kiện | Thông số | Ghi chú |
|---|---|---|---|
| U1 | ESP32 DevKit V1 | 30 hoặc 38 chân | mạch nạp CP210x |
| U2 | Mạch giảm áp buck | 12 V vào, **5 V ra, 5 A** | cấp toàn bộ hệ |
| PS1 | Nguồn tổ ong | 12 V, tối thiểu 3 A | nguồn gốc duy nhất |
| F1 | Cầu chì ống kèm đế | **2 A, 250 V** | trên đường 12 V vào |
| S1 | Cảm biến siêu âm HC-SR04 | 5 V | 4 chân VCC TRIG ECHO GND |
| S2 | Cảm biến lưu lượng ĐẦU VÀO | 5 V | 3 dây đỏ đen vàng |
| S6 | Cảm biến lưu lượng ĐẦU RA | 5 V | 3 dây đỏ đen vàng |
| S3 | Cảm biến dòng ACS712 | mô đun 5 A | ngõ ra tương tự |
| S4 | Phao mức cao | công tắc phao thường mở | |
| S5 | Phao mức THẤP, trên bồn chứa | công tắc phao thường mở | |
| K1 | Mô đun rơ le 1 kênh | cuộn 5 V, tiếp điểm 10 A | **kích mức thấp**, có opto |
| M1 | Bơm chìm | **5 V** | tải cảm |
| SW1 | Nút nhấn | 4 chân, thường mở | nút xoá lỗi |
| D1 | Điốt | **1N4007** | chống xung ngược, tại cực bơm |
| C1 | Tụ gốm | **100 nF** | tại cực bơm |
| C2 | Tụ hoá | **1000 µF, 16 V** | tại chân nguồn ESP32 |
| C3 | Tụ hoá | **470 µF, 16 V** | tại nhánh cấp bơm |
| C4, C5, C6 | Tụ gốm | **100 nF** | mỗi cảm biến một con |
| R1 | Điện trở | **10 kΩ** | nhánh trên chia áp ECHO |
| R2 | Điện trở | **20 kΩ** | nhánh dưới chia áp ECHO |
| R5, R6 | Điện trở | **10 kΩ** | chia áp cho ACS712 |
| R7 | Điện trở | **220 Ω** | hạn dòng đèn báo sự cố |
| LED1 | LED | đỏ 5 mm | đèn báo sự cố |
| TB1..TB4 | Cầu đấu vít 2 chân | | nguồn vào, bơm, hai phao |

Đèn báo trực tuyến dùng **LED xanh có sẵn trên bo ESP32** ở GPIO 2, không cần linh kiện rời.

**Mạch giảm áp 12 V xuống 3,3 V mà bạn có: KHÔNG dùng.** Lý do ở mục 3.

---

## 2. Bảng chân đầy đủ

**Mười một** chân GPIO. Không chân nào được đổi vì đã cố định trong `config.h`.

| Chân ESP32 | Tên trong mã | Hướng | Nối tới | Mạch trung gian |
|---|---|---|---|---|
| GPIO 5 | `PIN_TRIG` | ra | S1 chân TRIG | nối thẳng |
| GPIO 18 | `PIN_ECHO` | vào | S1 chân ECHO | **chia áp 10 k / 20 k** |
| GPIO 4 | `PIN_FLOW` | vào, ngắt cạnh xuống | S2 dây vàng, ĐẦU VÀO | **nối thẳng**, xem mục 4B |
| GPIO 19 | `PIN_FLOW_OUT` | vào, ngắt cạnh xuống | S6 dây vàng, ĐẦU RA | **nối thẳng**, xem mục 4B |
| GPIO 26 | `PIN_RELAY` | ra | K1 chân IN | nối thẳng |
| GPIO 34 | `PIN_CURRENT` | vào tương tự | S3 chân OUT | **chia áp 10 k / 10 k** |
| GPIO 27 | `PIN_FLOAT_MAX` | vào, kéo lên trong | S4 | nối thẳng xuống GND |
| GPIO 14 | `PIN_FLOAT_MIN` | vào, kéo lên trong | S5 | nối thẳng xuống GND |
| GPIO 33 | `PIN_BTN_RESET` | vào, kéo lên trong | SW1 | nối thẳng xuống GND |
| GPIO 2 | `PIN_LED_OK` | ra | LED có sẵn trên bo | không đi dây |
| GPIO 25 | `PIN_LED_FAULT` | ra | LED1 qua R7 220 Ω | anode về GPIO |

**GPIO 34 là chân chỉ vào**, không có điện trở kéo lên nội bộ, không xuất ra được. Đúng cho tín hiệu tương tự. Đừng đổi sang chân khác vì các chân ADC2 xung đột với Wi-Fi.

**GPIO 2 và GPIO 5 là chân trạng thái khởi động.** Không treo tải kéo mạnh lên chúng, nếu không bo sẽ không vào được chế độ nạp.

---

## 3. Kiến trúc nguồn, và vì sao phải phân phối hình sao

Bơm 5 V nghĩa là **động cơ và vi điều khiển dùng chung một đường 5 V**. Đây là điểm yếu lớn nhất của cấu hình này: dòng khởi động và nhiễu chổi than của động cơ đi thẳng vào nguồn nuôi ESP32.

Cách chống hiệu quả nhất mà không cần thêm linh kiện là **phân phối hình sao**: mọi nhánh tải xuất phát **từ chính cọc ngõ ra của mạch buck**, không nhánh nào mắc nối tiếp qua nhánh khác.

```
PS1 12 V ──► F1 cầu chì 2 A ──► U2 buck 12 V xuống 5 V 5 A
                                        │
                        ┌───────────────┴───────────────┐
                        │   CỌC NGÕ RA 5 V CỦA U2       │   ← điểm sao
                        │   (mọi nhánh bắt đầu từ đây)  │
                        └───────────────┬───────────────┘
                                        │
        ┌──────────────┬────────────────┼──────────────┬──────────────┐
        ▼              ▼                ▼              ▼              ▼
   nhánh A         nhánh B          nhánh C        nhánh D        nhánh E
   ESP32 VIN     HC-SR04 VCC   HAI cảm biến   ACS712 VCC    K1 VCC + tiếp
   + C2 1000µF     + C4 100nF    lưu lượng đỏ   + C6 100nF    điểm COM
                                 + C5 100nF                    + C3 470µF
```

**Nhánh E là nhánh bẩn**, mang dòng động cơ. Bốn nhánh còn lại là nhánh sạch. Chúng chỉ gặp nhau tại cọc buck, nên sụt áp do động cơ không truyền qua đường dây của ESP32.

**Dây của nhánh E phải to hơn hẳn**: dùng tối thiểu **20 AWG** cho đường cấp bơm, còn các nhánh tín hiệu và cảm biến dùng 24 AWG là đủ. Dây nhỏ trên nhánh động cơ tự nó tạo sụt áp.

**Đất cũng đi hình sao.** Mọi dây GND về thẳng cọc âm của buck, đặc biệt **GND của bơm không được đi chung dây với GND của ESP32**.

### Vì sao không dùng mạch buck 3,3 V

Trên lý thuyết, cấp ESP32 bằng đường 3,3 V riêng sẽ tách nó khỏi đường 5 V bẩn của động cơ, nghe rất hợp lý. Nhưng nó tạo ra một phiền toái thực tế lớn hơn lợi ích:

Cấp 3,3 V vào chân `3V3` là **đi vòng qua bộ ổn áp trên bo**. Mỗi lần bạn cắm USB để nạp chương trình, cổng USB cũng cấp 3,3 V qua bộ ổn áp đó, và hai nguồn sẽ đẩy nhau. Bạn sẽ phải **rút dây 3,3 V ra trước mỗi lần nạp** rồi cắm lại. Với đồ án phải nạp lại hàng chục lần thì đó là nguồn gây lỗi và hỏng bo.

Cấu hình hình sao cộng ba tụ lọc ở mục 5 đã đủ để chạy ổn định. Cứ để mạch buck 3,3 V lại dùng cho dự án khác.

**Không bao giờ chạy bơm bằng cổng USB của máy tính.**

---

## 4. Hai mạch chia áp

Công thức: `Vra = Vvào × R2 / (R1 + R2)`. R1 nối từ phía tín hiệu, R2 nối xuống đất, **điểm giữa hai điện trở** đi vào chân ESP32.

| Đường tín hiệu | R1 (trên) | R2 (dưới) | Vvào | Vra | Chân đích |
|---|---|---|---|---|---|
| ECHO của HC-SR04 | 10 kΩ | 20 kΩ | 5,0 V | **3,33 V** | GPIO 18 |
| OUT của ACS712 | 10 kΩ | 10 kΩ | 2,5 V nghỉ | **1,25 V** | GPIO 34 |

**Hai cảm biến lưu lượng KHÔNG nằm trong bảng này nữa.** Bản trước bắt chúng qua chia áp 10 k / 20 k, và đó là lỗi. Mục 4B giải thích.

**Vì sao ACS712 dùng tỉ số 1:1.** ACS712 xuất một nửa điện áp nguồn khi không tải, tức 2,5 V, rồi dao động quanh mốc đó theo chiều dòng điện. Chia đôi đưa điểm nghỉ về 1,25 V, tức **giữa dải đo 0 tới 3,3 V**, nên tín hiệu còn chỗ đi cả lên lẫn xuống mà không chạm hai vùng phi tuyến ở hai đầu thang đo. Firmware đọc trung bình 200 mẫu rồi lấy trị tuyệt đối độ lệch so với điểm nghỉ đã hiệu chuẩn; ngưỡng nhận biết bơm đang chạy là **15 mV**.

**Thiếu chia áp trên GPIO 18 sẽ phá hỏng chân ESP32.** HC-SR04 xuất chân ECHO ở mức 5 V bằng tầng đẩy kéo thật sự, còn ESP32 chỉ chịu 3,3 V.

---

## 4B. Hai cảm biến lưu lượng: nối thẳng, KHÔNG chia áp

Đây là thay đổi lớn nhất so với bản vẽ trước, và nó xuất phát từ một lỗi đo được trên phần cứng thật: cảm biến đọc ra hơn 500 lít mỗi phút trong khi bơm chưa hề chạy.

**Nguyên nhân.** Firmware bật điện trở kéo lên nội bộ của ESP32, trong khi phần cứng lại có chia áp 10 k / 20 k. Hai thứ đánh nhau:

```
kéo lên nội bộ ≈ 45 kΩ,  song song 20 kΩ xuống đất
điện áp nghỉ = 3,3 × 20 / (45 + 20) = 1,02 V

ngưỡng ESP32:   thấp < 0,825 V      cao > 2,475 V
1,02 V rơi đúng VÙNG KHÔNG XÁC ĐỊNH
```

Chân dao động theo nhiễu và sinh hàng nghìn xung giả mỗi giây. Đo thực tế: 3849 tới 5147 xung/giây, trong khi YF-S401 ở lưu lượng tối đa chỉ cho 6 × 98 = **588 xung/giây**.

**Cách đấu đúng, dùng cho cả hai cảm biến:**

```
dây đỏ  → 5 V
dây đen → GND điểm sao
dây vàng → GPIO, VÀ một điện trở 4,7 kΩ từ dây vàng lên 3,3 V
```

**Điện trở kéo lên ngoài 4,7 kΩ là bắt buộc, không phải tuỳ chọn.** Đo trên mạch thật cho thấy điều này. Với điện trở kéo lên **nội bộ** của ESP32, khoảng 45 kΩ:

| Tình huống | Số xung đếm được |
|---|---|
| Chạy `test_flow`, Wi-Fi **tắt** | **0 Hz**, đúng |
| Chạy phần sụn chính, Wi-Fi **bật** | **1635 tới 2735 Hz** nhiễu |
| Thêm bộ lọc xung trong phần mềm | còn **455 Hz**, vẫn sai |

Nghĩa là 45 kΩ quá yếu để giữ mức cao trước nhiễu vô tuyến của chính Wi-Fi trên bo. Điện trở 4,7 kΩ mạnh gấp mười lần, đó mới là cách sửa thật. Phần mềm chỉ giảm bớt chứ không dứt điểm được.

Dấu hiệu nhận biết: nếu **cả hai** cảm biến đọc ra con số **giống hệt nhau** thì đó là nhiễu đồng pha, không phải nước. Nước chảy qua hai chỗ khác nhau không bao giờ cho hai số trùng khít.

Cấu hình `FLOW_PIN_PULLUP = 1` và `FLOW_OUT_PIN_PULLUP = 1` trong `config.h`. Ngõ ra hall của YF-S201 và YF-S401 là **cực thu hở**: nó chỉ kéo chân xuống đất, không bao giờ tự đẩy lên 5 V. Điện trở kéo lên nội bộ giữ chân ở 3,3 V lúc nghỉ, nên tín hiệu dao động sạch giữa 0 V và 3,3 V.

**Phép đo bắt buộc trước khi cắm vào ESP32.** Một số mô đun bán sẵn có gắn thêm điện trở kéo lên VCC ngay trên bo, khi đó ngõ ra là 5 V thật và cắm thẳng sẽ hỏng chân.

Cấp 5 V cho cảm biến, **chưa nối dây vàng vào ESP32**, rồi đo điện áp giữa dây vàng và GND:

| Đo được | Nghĩa | Phải làm |
|---|---|---|
| khoảng **5 V** | mô đun có điện trở kéo lên sẵn | **phải** dùng chia áp 10 k / 20 k, đặt `FLOW_PIN_PULLUP = 0` |
| khoảng **0 V** hoặc trôi nổi | cực thu hở thuần | nối thẳng, giữ `FLOW_PIN_PULLUP = 1` |

**Tuyệt đối không dùng đồng thời cả hai.**

Cách thử sau khi lắp: chạy `test_flow`, nếu cột xung/giây khác 0 trong khi không có nước chảy thì rút hẳn dây vàng ra. Nếu vẫn đếm xung thì lỗi ở cấu hình chân chứ không phải cảm biến.

---

## 5. Linh kiện chống nhiễu, đặt đúng chỗ mới có tác dụng

Vị trí quan trọng ngang giá trị. Vẽ đúng nơi, không gom về một góc cho gọn.

**D1, điốt 1N4007** mắc **song song ngược** trực tiếp hai cực bơm M1. Vạch trắng trên thân quay về phía **cực dương**. Khi rơ le ngắt, từ trường cuộn dây sụp đổ sinh xung ngược theo `V = L·di/dt` có thể vượt vài trăm vôn. Điốt tạo đường vòng cho dòng đó tự tiêu tán. Thiếu nó là hỏng tiếp điểm rơ le, nặng hơn thì hỏng cả mạch điều khiển.

**C1, tụ gốm 100 nF** hàn **ngay tại hai cực động cơ**, càng sát càng tốt, tốt nhất là hàn thẳng lên chân bơm. Nó dập nhiễu tần số cao do chổi than sinh ra. Thiếu nó, nhiễu lan theo đường nguồn và làm số đọc siêu âm nhảy loạn mỗi lần bơm chạy.

**C2, tụ hoá 1000 µF 16 V** giữa **5 V và GND, sát chân VIN của ESP32**. Cực âm về GND. Đây là kho điện dự trữ cục bộ cho vi điều khiển khi động cơ khởi động kéo sụt đường chung.

**C3, tụ hoá 470 µF 16 V** giữa **5 V và GND trên nhánh cấp bơm**, đặt gần rơ le. Nó cấp phần lớn dòng khởi động ngay tại chỗ, giúp xung dòng không phải chạy ngược về tận mạch buck.

**C4, C5, C6, ba tụ gốm 100 nF**, mỗi cảm biến một con, nối giữa chân VCC và GND **ngay tại chân cảm biến**. Rẻ và hiệu quả, đừng bỏ.

Bốn tụ C2 tới C6 chính là thứ thay thế cho việc phải tách đường nguồn riêng cho ESP32.

---

## 6. Cực tính hai phao và nút nhấn, phần dễ sai nhất

Cả ba dùng **điện trở kéo lên nội bộ** của ESP32: **một chân về GPIO, chân còn lại về GND**. Không cần điện trở ngoài.

Firmware đọc như sau, cách đấu phải khớp:

| Thiết bị | Chân | Mã trong firmware | Nghĩa của mức LOW |
|---|---|---|---|
| S4 phao mức cao | GPIO 27 | `floatMax = (digitalRead(...) == LOW)` | **LOW = phao đã nổi lên, nước gần tràn** |
| S5 phao mức THẤP, trên bồn chứa | GPIO 14 | `floatMin = (digitalRead(...) == HIGH)` | **HIGH = nước đã tụt dưới vạch thấp** |
| SW1 nút xoá lỗi | GPIO 33 | tích cực mức thấp | LOW = đang bấm |

Hai phao mang ý nghĩa **ngược nhau về mặt vật lý**, đây là chỗ hay đấu nhầm nhất:

- **S4 phải đóng mạch khi nước dâng cao.** Lắp gần miệng bồn chính, hướng sao cho nước dâng thì tiếp điểm đóng.
- **S5 là phao mức THẤP của bồn chứa, không phải phao bồn nguồn.** Bồn nguồn không có cảm biến nào. Khi nước trong bồn chứa tụt xuống dưới vạch thấp, S5 kích hoạt và **cho phép bật bơm** — nó không chặn bơm bao giờ.

Đấu ngược S5 làm hệ mất đường thứ hai để biết bồn đã cạn. Cảm biến siêu âm rớt khoảng 30% số lần đo trong thùng 10×10 cm, nên mất S5 là có lúc bồn cạn thật mà hệ thống vẫn đứng yên. Đặt `FLOAT_MIN_ACTIVE_LOW` cho khớp thay vì đổi cách đấu dây.

Phao thường bán loại có hai hướng lắp, phân biệt bằng chiều mũi tên hoặc bằng cách lật ngược thân phao. **Đo thông mạch bằng đồng hồ trước khi lắp cố định.**

---

## 7. Mô đun rơ le

`config.h` đặt `RELAY_ACTIVE_LOW = 1`, firmware viết:

```c
digitalWrite(PIN_RELAY, on ? LOW : HIGH);   // LOW = bơm CHẠY
```

GPIO 26 xuống **mức thấp thì bơm chạy**. Hầu hết mô đun bán sẵn đều kích mức thấp nên thường khớp luôn. Nếu mô đun của bạn kích mức cao thì sửa `RELAY_ACTIVE_LOW` thành `0` trong `config.h`, **đừng đổi cách đấu dây**.

Ba dây tín hiệu: **VCC về 5 V, GND về GND chung, IN về GPIO 26**.

Phía tiếp điểm dùng **COM và NO**, **không dùng NC**, để khi mất điện thì bơm ở trạng thái ngắt. Đây là lớp phòng vệ chống tràn thứ tư trong thiết kế.

**Cách nâng cấp đáng làm.** Nhiều mô đun rơ le có một **jumper nối VCC với JD-VCC**. Để nguyên jumper thì cuộn hút và opto dùng chung nguồn, và opto mất tác dụng cách ly. **Rút jumper ra**, rồi cấp `JD-VCC` bằng một cặp dây riêng đi thẳng từ cọc buck, còn `VCC` vẫn lấy từ nhánh sạch. Khi đó dòng đóng cuộn hút không chạy qua đường nguồn của ESP32 nữa. Nếu mô đun của bạn có jumper này, hãy vẽ nó ở trạng thái đã rút, kèm chú thích.

---

## 8. Vị trí ACS712 trên mạch công suất

ACS712 đo dòng bằng cách để dòng chảy **xuyên qua** nó, nên mắc **nối tiếp** trên đường cấp bơm, không mắc song song.

```
5 V từ cọc buck ──► tiếp điểm COM của K1
                    tiếp điểm NO của K1 ──► IP+ của S3 ──► IP- của S3 ──► cực dương M1
                                                                          cực âm M1 ──► GND
                    D1 và C1 mắc song song hai cực M1
```

Chân VCC và GND của S3 lấy từ **nhánh sạch** (nhánh D ở mục 3), chỉ có hai chân IP+ và IP- nằm trên đường công suất. Chân OUT đi qua chia áp 10 k / 10 k rồi vào GPIO 34.

Mắc song song sẽ tạo ngắn mạch qua điện trở shunt bên trong cảm biến.

---

## 9. Ràng buộc cơ khí cần thể hiện trên sơ đồ đi dây

- **Cảm biến siêu âm** trên giá cứng, mặt cảm biến cách đáy bồn **tối thiểu 50 cm**, vì `TANK_SENSOR_TO_BOTTOM_CM = 55` và vùng chết của HC-SR04 là 25 cm. Giá rung làm số đọc dao động và bị chẩn đoán nhầm thành lỗi cảm biến.
- **Cảm biến lưu lượng** lắp **thẳng đứng, dòng chảy đi lên**, nghiêng không quá 5 độ. Trên thân có mũi tên chỉ chiều, phải đúng chiều. Lắp nghiêng làm tuabin kẹt ở lưu lượng thấp.
- **Bồn chính** trong suốt, dán thước chia vạch để hiệu chuẩn.
- Chiều cao cột nước khi đầy **25 cm**, tiết diện bồn **400 cm²**.
- **Dây tín hiệu đi tách khỏi dây động lực.** Đừng bó chung dây bơm với dây cảm biến siêu âm trong cùng một bó. Nếu buộc phải cắt ngang nhau thì cho cắt vuông góc.

---

## 10. Yêu cầu về hình vẽ

Xin **hai hình riêng biệt**.

**Hình 1, sơ đồ nguyên lý.** Ký hiệu điện tử chuẩn, ESP32 ở giữa.
- Bên trái: ba cảm biến đầu vào kèm ba mạch chia áp, vẽ rõ từng điện trở và trị số.
- Bên phải: khối công suất gồm rơ le, ACS712, bơm, điốt và tụ.
- Phía dưới: hai phao, nút nhấn, đèn báo.
- Góc riêng: khối nguồn, vẽ rõ **điểm sao** và năm nhánh toả ra từ đó.
- Ghi nhãn đầy đủ tên chân, ký hiệu linh kiện, trị số.
- Đánh dấu **sáu điểm đo** bằng ký hiệu test point, đúng theo bảng ở mục 12.

**Hình 2, sơ đồ đi dây kiểu breadboard.** Vẽ đúng hình dáng mô đun thật để người lắp đối chiếu được.
- Quy ước màu dây: **vàng cho 12 V, đỏ cho 5 V sạch, cam cho 5 V nhánh bơm, đen cho GND, xanh lá cho tín hiệu vào, xanh dương cho tín hiệu ra**.
- Vẽ dây nhánh bơm **dày hơn** để thể hiện tiết diện lớn hơn.
- Ghi số chân lên từng đầu dây.

**Trên cả hai hình phải làm nổi bật bốn chỗ dễ hỏng nhất**, dùng khung đỏ hoặc chú thích cảnh báo:

1. Chia áp trên ECHO, thiếu là cháy chân GPIO 18
2. Hai cảm biến lưu lượng nối THẲNG, tuyệt đối không thêm chia áp
3. Điốt D1 và chiều lắp của nó
4. Điểm sao của nguồn và đất
5. Cực tính hai phao ngược nhau

---

## 11. Những điều tuyệt đối không được vẽ

- Không nối thẳng ECHO của HC-SR04 vào ESP32 khi chưa qua chia áp
- Không thêm chia áp cho hai dây vàng của cảm biến lưu lượng khi đã bật kéo lên nội bộ
- Không cấp 5 V vào chân `3V3` của ESP32
- Không đồng thời cấp cả `VIN` và `3V3`
- Không lấy nguồn bơm từ chân 5 V trên bo ESP32; bơm phải lấy thẳng từ cọc buck qua tiếp điểm rơ le
- Không dùng tiếp điểm NC của rơ le
- Không mắc ACS712 song song với bơm
- Không bỏ điốt D1 dù bơm nhỏ tới đâu
- Không nối đất kiểu chuỗi, mọi GND về thẳng điểm sao
- Không bó chung dây bơm với dây cảm biến

---

## 12. Tám điểm đo để kiểm tra sau khi lắp

| Điểm đo | Vị trí | Kết quả đúng |
|---|---|---|
| TP1 | GPIO 18, khi HC-SR04 đang phát | không vượt 3,3 V |
| TP2 | GPIO 34, khi bơm tắt | khoảng 1,25 V |
| TP3 | GPIO 27 xuống GND, khi phao mức cao đang nổi | thông mạch |
| TP4 | GPIO 14 xuống GND, khi nước bồn chứa trên vạch thấp | thông mạch |
| TP5 | Chân VIN của ESP32, **trong lúc bơm khởi động** | không tụt dưới 4,7 V |
| TP6 | GPIO 33, khi bấm SW1 | xuống mức thấp |
| TP7 | Dây vàng cảm biến lưu lượng, **chưa nối vào ESP32**, cảm biến đã cấp 5 V | xem bảng ở mục 4B: ~5 V thì phải chia áp, ~0 V thì nối thẳng |
| TP8 | GPIO 4 và GPIO 19 lúc nghỉ, sau khi đã nối và nạp firmware | khoảng 3,3 V, tuyệt đối không phải ~1 V |

**TP8 khoảng 1 V nghĩa là bạn đang mắc đúng lỗi ở mục 4B**: vừa bật kéo lên nội bộ vừa có chia áp. Chân sẽ đếm hàng nghìn xung giả mỗi giây.

**TP5 là phép đo quan trọng nhất của cấu hình bơm 5 V.** Nếu điện áp tụt dưới 4,7 V lúc bơm khởi động thì ESP32 sẽ khởi động lại, và nguyên nhân gần như luôn là một trong bốn thứ: thiếu tụ C2, dây nhánh bơm quá nhỏ, đấu đất kiểu chuỗi thay vì hình sao, hoặc mạch buck không đủ dòng.

Sau khi lắp xong, nạp lần lượt năm chương trình kiểm thử theo bảy bước lắp ráp trước khi nạp phần sụn chính. Đừng lắp cả mạch rồi nạp một lần.
