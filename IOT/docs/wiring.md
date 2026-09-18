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
| GPIO 14 | phao bồn nguồn | điện trở kéo lên nội bộ |
| GPIO 33 | nút xóa lỗi | điện trở kéo lên nội bộ |
| GPIO 2 | đèn báo trực tuyến | có sẵn trên bo |
| GPIO 25 | đèn báo sự cố | qua điện trở 220 ohm |

GPIO 34 là chân chỉ vào, không có điện trở kéo nội bộ, đúng cho việc đọc tín hiệu tương tự.

## Hai cảm biến lưu lượng: chọn đúng một trong hai cách đấu

Đây là chỗ sai nhiều nhất, và sai thì cảm biến đọc ra hàng nghìn lít mỗi phút.

**Cách A, khuyên dùng.** Nối dây tín hiệu **thẳng** vào GPIO, không chia áp, và để `FLOW_PIN_PULLUP = 1` trong `config.h`. Điện trở kéo lên nội bộ của ESP32 kéo chân lên 3,3 V, còn cảm biến chỉ kéo xuống đất khi có xung. Ngõ ra hall của YF-S201 và YF-S401 là cực thu hở nên cách này đúng về điện. Cảm biến vẫn cấp nguồn 5 V bình thường.

**Cách B.** Nếu mô đun của bạn **đã có sẵn điện trở kéo lên VCC 5 V** thì ngõ ra là 0–5 V thật, khi đó phải qua chia áp 10k/20k và đặt `FLOW_PIN_PULLUP = 0`.

**Tuyệt đối không làm cả hai cùng lúc.** Điện trở kéo lên nội bộ khoảng 45 kΩ, mắc song song với 20 kΩ xuống đất cho điện áp nghỉ:

```
3,3 × 20 / (45 + 20) = 1,02 V
```

Ngưỡng vào của ESP32 là 0,825 V và 2,475 V, nên 1,02 V rơi đúng **vùng không xác định**. Chân sẽ dao động theo nhiễu và sinh ra hàng nghìn xung giả mỗi giây. Chạy `test_flow` mà thấy cột xung/giây vượt 700 khi bơm chưa chạy thì gần như chắc chắn bạn đang mắc lỗi này.

Cách phân biệt nhanh: rút hẳn dây tín hiệu ra khỏi GPIO. Nếu số xung vẫn đếm thì lỗi ở cấu hình chân, không phải ở cảm biến.

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
