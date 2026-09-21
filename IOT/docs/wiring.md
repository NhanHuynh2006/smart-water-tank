# Đấu dây

## Bảng chân

| ESP32 | Nối tới | Mạch trung gian |
|---|---|---|
| GPIO 5 | TRIG cảm biến siêu âm | nối thẳng |
| GPIO 18 | ECHO cảm biến siêu âm | chia áp 10k/20k |
| GPIO 4 | dây tín hiệu cảm biến lưu lượng ĐẦU VÀO | xem ghi chú bên dưới |
| GPIO 19 | dây tín hiệu cảm biến lưu lượng ĐẦU RA | xem ghi chú bên dưới |
| GPIO 26 | chân IN module rơ le | nối thẳng |
| GPIO 34 | ngõ ra ACS712 | chia áp 10k/10k |
| GPIO 27 | phao mức cao | điện trở kéo lên nội bộ |
| GPIO 14 | phao mức THẤP, trên bồn chứa | điện trở kéo lên nội bộ |
| GPIO 33 | nút xóa lỗi | điện trở kéo lên nội bộ |
| GPIO 2 | đèn báo trực tuyến | có sẵn trên bo |
| GPIO 25 | đèn báo sự cố | qua điện trở 220 ohm |

GPIO 34 là chân chỉ vào, không có điện trở kéo nội bộ, đúng cho việc đọc tín hiệu tương tự.

## Hai cảm biến lưu lượng: chọn đúng một trong hai cách đấu

Đây là chỗ sai nhiều nhất, và sai thì cảm biến đọc ra hàng nghìn lít mỗi phút.

**Cách A, khuyên dùng.** Nối dây tín hiệu vào GPIO **không qua chia áp**, và hàn thêm một **điện trở 4,7 kΩ từ dây tín hiệu lên 3,3 V**, rồi để `FLOW_PIN_PULLUP = 1` trong `config.h`. Điện trở ngoài này bắt buộc: đo thật cho thấy điện trở kéo lên nội bộ 45 kΩ quá yếu, khi Wi-Fi bật thì chân bắt 1600 tới 2700 Hz nhiễu dù không có nước chảy. Điện trở kéo lên nội bộ của ESP32 kéo chân lên 3,3 V, còn cảm biến chỉ kéo xuống đất khi có xung. Ngõ ra hall của YF-S201 và YF-S401 là cực thu hở nên cách này đúng về điện. Cảm biến vẫn cấp nguồn 5 V bình thường.

**Cách B.** Nếu mô đun của bạn **đã có sẵn điện trở kéo lên VCC 5 V** thì ngõ ra là 0–5 V thật, khi đó phải qua chia áp 10k/20k và đặt `FLOW_PIN_PULLUP = 0`.

**Tuyệt đối không làm cả hai cùng lúc.** Điện trở kéo lên nội bộ khoảng 45 kΩ, mắc song song với 20 kΩ xuống đất cho điện áp nghỉ:

```
3,3 × 20 / (45 + 20) = 1,02 V
```

Ngưỡng vào của ESP32 là 0,825 V và 2,475 V, nên 1,02 V rơi đúng **vùng không xác định**. Chân sẽ dao động theo nhiễu và sinh ra hàng nghìn xung giả mỗi giây. Chạy `test_flow` mà thấy cột xung/giây vượt 700 khi bơm chưa chạy thì gần như chắc chắn bạn đang mắc lỗi này.

Cách phân biệt nhanh: rút hẳn dây tín hiệu ra khỏi GPIO. Nếu số xung vẫn đếm thì lỗi ở cấu hình chân, không phải ở cảm biến.

## Đừng cho chân ECHO đi qua TXS0108E

TXS0108E **không hợp với HC-SR04**, dù nó là mô đun chuyển mức 5 V ↔ 3,3 V rất phổ biến.

Nó là loại **tự dò chiều**, thiết kế cho bus hai chiều kiểu I²C. Cách nó đoán chiều là dựa vào điện trở kéo yếu khoảng 10 kΩ ở hai phía cộng một mạch *one-shot* tăng tốc sườn. Ba hệ quả:

- Chân ECHO của HC-SR04 là ngõ ra **đẩy kéo mạnh**, không phải cực máng hở yếu. TXS0108E dò nhầm chiều, có lúc tự dao động hoặc kẹt.
- Mạch *one-shot* nổ theo từng sườn và **tự sinh thêm xung**. Với `pulseIn` thì mỗi xung thừa là một phép đo khoảng cách sai.
- Ở trạng thái nghỉ, trở kháng ngõ ra khoảng 4 kΩ. Cộng với điện dung dây là sườn tín hiệu bị bo tròn, và độ rộng xung — thứ duy nhất mang thông tin khoảng cách — bị lệch.

**Cách làm đúng, và cũng là cách bản thiết kế này vốn yêu cầu: một cầu chia áp điện trở, chỉ trên chân ECHO.** R1 nối từ ECHO vào chân ESP32, R2 từ chân đó xuống GND, điểm giữa đi vào GPIO 18. Chân TRIG nối thẳng, hầu hết mô đun HC-SR04 nhận mức 3,3 V làm mức cao.

### Dùng 10k/20k hay 1k/2k?

Cả hai đều cho **đúng 3,33 V**, vì chỉ tỉ số mới quyết định điện áp: `5 × 20/(10+20) = 5 × 2/(1+2)`. Khác nhau ở **trở kháng nguồn** của điểm giữa:

| Cặp điện trở | Trở kháng điểm giữa | Dòng lấy từ ECHO | Sườn tín hiệu¹ | Sai số khoảng cách² |
|---|---|---|---|---|
| **10 kΩ / 20 kΩ** | 6,7 kΩ | 0,17 mA | ~730 ns | 0,013 cm |
| **1 kΩ / 2 kΩ** | 0,67 kΩ | 1,7 mA | ~73 ns | 0,001 cm |

¹ thời gian lên 10–90%, ước tính với khoảng 50 pF gồm điện dung chân ESP32 và dây nối ngắn
² HC-SR04 mã hoá khoảng cách bằng độ rộng xung, 58 µs cho mỗi cm

**Về thời gian thì khác biệt không đáng kể** — 0,013 cm chìm hoàn toàn dưới sai số ±1 cm mà cảm biến đang có. Cứ dùng 10k/20k, đó là giá trị ghi trong bảng vật tư.

**Khác biệt thật nằm ở khả năng chống nhiễu.** Điểm giữa 6,7 kΩ nhạy với nhiễu gấp mười lần điểm giữa 0,67 kΩ. Bo mạch này đã có sẵn vấn đề nhiễu nặng: hai chân lưu lượng thả nổi bắt được 3 kHz, và dây bơm từng làm cảm biến siêu âm đọc sai hẳn. Vì vậy:

- Đặt hai điện trở **sát chân ESP32**, không đặt ở đầu cảm biến. Đoạn dây 6,7 kΩ càng ngắn càng tốt, dưới 10 cm.
- Đi dây ECHO **tách khỏi dây bơm và dây rơ le**, đừng bó chung.
- Nếu làm hai điều trên mà vẫn nhiễu, hạ xuống **1k/2k** hoặc **2,2k/4,7k**. Đổi lại là ECHO phải gánh 1,7 mA — vẫn nhẹ nhàng với tầng đẩy kéo của HC-SR04.

## Mặt nước gợn khi bơm cũng làm sai số

Đo thật khi bơm chạy: cảm biến nhảy qua lại giữa 4,8 cm và 0,00 cm — lúc thấy mặt nước, lúc nhìn xuyên xuống đáy thùng. Dòng nước đổ vào làm mặt nước gợn, tiếng dội tán đi hướng khác, chỉ còn tiếng dội từ đáy quay về.

Hai cách chữa, nên làm cả hai:

- **Ống lặng.** Một đoạn ống nhựa thẳng đứng, đường kính 4–5 cm, cắm từ dưới mặt cảm biến xuống ngập trong nước, khoan vài lỗ nhỏ ở đáy ống. Nước trong ống đứng yên trong khi nước ngoài ống gợn. Đây là cách chuẩn cho đo mức bằng siêu âm ở bồn nhỏ.
- **Đưa đầu vào xuống dưới mặt nước**, hoặc cho nó chảy men theo thành thùng thay vì rơi thẳng xuống giữa.

Thùng 10 × 10 cm còn một khó khăn riêng: chùm sóng 15° của HC-SR04 ở khoảng cách 15,5 cm đã rộng hơn 8 cm, tức là gần bằng cả lòng thùng. Tia biên chạm thành thùng là chuyện chắc chắn xảy ra. Ống lặng cũng giải quyết luôn chuyện này.

## Cả hai phao đều nằm trên bồn chứa

Bồn nguồn **không có cảm biến nào**. Phần mềm không cách nào biết bồn nguồn còn nước hay không, nên nó không được phép viện vào đó để chặn bơm. Chạy khô được bắt bằng ba luật khác: `DRY_RUN` (có lệnh bơm mà không có dòng chảy), `NO_CURRENT` (không có dòng điện qua bơm) và `NO_PROGRESS` (bơm chạy mà mực nước không lên).

| Phao | Chân | Khi kích hoạt nghĩa là | Tác dụng |
|---|---|---|---|
| Mức CAO | GPIO 27 | nước chạm vạch cao | **ngắt bơm**, khoá chống tràn |
| Mức THẤP | GPIO 14 | nước tụt dưới vạch thấp | **cho phép bật bơm** |

Phao mức thấp là **đường thứ hai** song song với siêu âm. Cảm biến siêu âm rớt khoảng 30% số lần đo trong thùng 10×10 cm, nên nếu chỉ dựa vào nó thì có lúc bồn cạn thật mà hệ thống vẫn đứng yên.

Hai phao nói ngược nhau so với siêu âm thì báo `SENSOR_CONFLICT` và không tin cái nào hết.

## Hai phao: cực tính phải đo, không được đoán

Cả hai chân phao đều bật kéo lên nội bộ, nên **chân hở đọc ra mức cao**. Câu hỏi duy nhất là: khi nước đầy, tiếp điểm phao **đóng** hay **mở**?

Hai kiểu phao đều bán đầy ngoài chợ và nhìn bên ngoài giống hệt nhau. Đoán sai là phần mềm tưởng bồn còn rỗng trong khi nước đã lên tới miệng — **đây đúng là kiểu lỗi làm bơm không chịu dừng**.

Đo bằng lệnh `pio run -e test_floats -t upload -t monitor`, nhấc tay phao lên hết cỡ, rồi đặt `FLOAT_MAX_ACTIVE_LOW` trong `config.h` cho khớp.

Nếu chọn được, hãy dùng kiểu **mở khi đầy** và đặt `FLOAT_MAX_ACTIVE_LOW 0`. Kiểu đó hỏng theo hướng an toàn: đứt dây thì chân lên mức cao, phần mềm hiểu là "bồn đầy" và chặn bơm. Kiểu ngược lại thì đứt dây thành "bồn rỗng" và bơm chạy mãi.

## Chân điều khiển rơ le cần điện trở kéo lên 10 kΩ

**Cập nhật 21/09: mô đun trên mạch này kích mức CAO, không phải mức thấp.** Đã đo: firmware đưa chân lên mức cao để ngắt bơm mà bơm vẫn chạy, mực nước dâng từ 0,2 cm lên 11,2 cm trong khi web báo "bơm tắt". `RELAY_ACTIVE_LOW` đã đổi thành `0`. Vì vậy điện trở kéo ở mục này phải nối **xuống GND**, không phải lên 3,3 V.

Mô đun rơ le kích mức thấp hiểu **mức thấp là lệnh bật**. Từ lúc cấp điện tới lúc `setup()` chạy được dòng đầu tiên, chân GPIO 26 vẫn **thả nổi**, và mô đun có thể hiểu nhầm thành lệnh bật. Khoảng đó dài chừng 300 ms của bootloader ROM, lặp lại **mỗi lần khởi động, mỗi lần nhấn nút reset, và mỗi lần nạp chương trình**.

Phần mềm đã đưa rơ le về ngắt ngay dòng đầu của `setup()`, nhưng không rút ngắn được phần bootloader. Cách bịt hẳn là **một điện trở 10 kΩ từ chân IN của rơ le lên 3,3 V**: khi ESP32 chưa điều khiển, điện trở giữ chân ở mức cao, tức lệnh ngắt.

Nếu mô đun của bạn kích mức cao thì điện trở này nối **xuống GND** thay vì lên 3,3 V.

## Ba mạch chia áp

Công thức: `Vout = Vin × R2 / (R1 + R2)`, trong đó R1 nối từ tín hiệu, R2 nối xuống đất, điểm giữa đi vào ESP32.

| Đường tín hiệu | R1 | R2 | Vào | Ra |
|---|---|---|---|---|
| ECHO siêu âm | 10k | 20k | 5,0 V | 3,33 V |
| Xung lưu lượng | 10k | 20k | 5,0 V | 3,33 V |
| Ngõ ra ACS712 | 10k | 10k | 2,5 V khi không tải | 1,25 V |

Tỉ số 1:1 cho đường cảm biến dòng là có chủ đích: sau khi chia, điểm làm việc nằm ở khoảng giữa dải đo 0 tới 3,3 V, tránh hai vùng phi tuyến ở hai đầu.

## Ba linh kiện bảo vệ bắt buộc

**Điốt 1N4007** mắc song song ngược hai cực bơm, vạch dấu về phía cực dương. Khi ngắt bơm, từ trường sụp đổ sinh xung điện áp ngược theo `V = L·di/dt`, có thể vượt hàng trăm vôn.

**Tụ gốm 100 nF** hàn ngay tại hai cực động cơ. Không nhằm chống quá áp mà nhằm dập nhiễu tần số cao do chổi than sinh ra. Thiếu tụ này, nhiễu lan theo đường nguồn và làm số đọc siêu âm nhảy loạn mỗi khi bơm chạy.

**Tụ hóa 1000 µF** giữa đường 5 V và đất, đặt sát chân cấp nguồn ESP32. Dòng khởi động của động cơ lớn hơn nhiều lần dòng làm việc, gây sụt áp. Nếu điện áp tụt dưới ngưỡng tới hạn, mạch phát hiện sụt áp tích hợp sẽ đưa chip về trạng thái đặt lại.

## Nguồn

Cấu hình đã chốt: nguồn tổ ong **12 V** qua cầu chì 2 A, hạ xuống **5 V bằng mạch buck 5 A**. Bơm là loại **5 V**, nên bơm và ESP32 dùng chung đường 5 V.

Vì dùng chung, cách phân phối trở nên quan trọng. Mọi nhánh tải phải xuất phát **từ chính cọc ngõ ra của mạch buck** theo hình sao, không nhánh nào mắc nối tiếp qua nhánh khác. Nhánh cấp bơm dùng dây tối thiểu 20 AWG, các nhánh còn lại 24 AWG. Đường đất cũng đi hình sao, đặc biệt GND của bơm không đi chung dây với GND của ESP32.

Không dùng cổng USB của máy tính để chạy bơm.

Đặc tả đầy đủ để vẽ sơ đồ nằm ở `docs/schematic-spec.md`, gồm cả danh mục linh kiện, sáu điểm đo kiểm và những lỗi đấu dây hay gặp.

## Cơ khí

Giá đỡ cảm biến siêu âm phải cao hơn mực nước tối đa ít nhất 25 cm vì vùng chết của cảm biến. Với `TANK_MAX_LEVEL_CM = 25`, khoảng cách từ mặt cảm biến xuống đáy bồn tối thiểu là 50 cm. Giá đỡ phải cứng vững, vì rung động làm số đọc dao động và sẽ bị chẩn đoán nhầm thành lỗi cảm biến.

Cảm biến lưu lượng lắp thẳng đứng, chiều dòng chảy đi lên, độ nghiêng không quá năm độ. Lắp nghiêng làm tuabin dễ kẹt ở lưu lượng thấp.

Bồn chính nên trong suốt và dán thước chia vạch để phục vụ hiệu chuẩn E1.
