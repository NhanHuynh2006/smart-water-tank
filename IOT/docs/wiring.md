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

## Đo trạng thái rơ le bằng độ gợn dòng, không bằng mức trung bình

ACS712 là cảm biến **hai chiều**. Ngõ ra nằm giữa dải và lệch **lên hoặc xuống** tuỳ chiều dòng chạy qua IP+ và IP-. Vì vậy "mV cao hơn" **không** có nghĩa là "dòng lớn hơn", và dùng mức trung bình để đoán xem bơm có chạy không là sai.

Thứ đọc đúng là **độ gợn**. Động cơ chổi than chạy thì dòng gợn mạnh do cổ góp; lúc ngắt thì đường dòng phẳng. Đo thật ngày 21/09:

| Chân điều khiển | Trung bình | Độ gợn |
|---|---|---|
| MỨC THẤP | 1545,88 mV | **4,47 mV** |
| MỨC CAO | 1516,54 mV | **27,05 mV** |

Tỉ số gợn **6,0 lần** tách hai trạng thái ra rất rõ, và không phụ thuộc chiều đấu dây qua IP+ / IP-. Mức trung bình thì ngược lại: chân MỨC CAO cho số *nhỏ hơn* dù đó mới là lúc bơm chạy, vì dòng lệch ngõ ra xuống dưới điểm giữa.

Chạy `pio run -e test_relaytruth -t upload -t monitor` để đo lại bất cứ lúc nào. Bơm chỉ chạy 6 giây mỗi phía, và chương trình kết thúc bằng việc đưa rơ le về phía ngắt.

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

## Điốt 1N4007 không làm giảm nhiễu lưu lượng — nhiễu đi theo đường nguồn

Đo ngày 22/09, trước và sau khi gắn điốt song song ngược hai cực bơm **và** đưa dây bơm đi xa hẳn khỏi dây tín hiệu:

| | Trước điốt | Sau điốt + tách dây |
|---|---|---|
| GPIO 19, bơm chạy | 1 548,4 Hz | 1 567,0 Hz |
| GPIO 4, bơm chạy | 1 474,4 Hz | 1 487,3 Hz |
| Cả hai, bơm tắt | 0 Hz | 0 Hz |

Chênh nhau dưới 1,3%, nằm trong sai số phép đo. **Điốt và việc tách dây không làm giảm gì cả.**

Kết luận: phần nhiễu còn lại **không lan qua không khí**. Nếu là bức xạ thì tách dây ra xa phải có tác dụng. Nó **dẫn theo đường nguồn và đường đất** — bơm rút dòng, đường 5 V sụt và gợn, đất chung có sụt áp giữa điểm nối của cảm biến và điểm nối của ESP32. Điốt chỉ dập xung cảm ứng lúc ngắt, không chạm tới chuyện này.

Điốt vẫn nên giữ: nó bảo vệ tiếp điểm rơ le khỏi xung vài trăm vôn mỗi lần ngắt. Nó chỉ không phải thuốc cho bệnh này.

### Phép thử dứt điểm, chỉ cần dời hai sợi dây

Hiện cả bơm lẫn hai cảm biến lưu lượng đều ăn 5 V từ cọc buck. Hãy **rút dây đỏ của hai cảm biến khỏi cọc buck và cắm vào chân 5 V của ESP32** — chân đó lấy điện từ cổng USB, hoàn toàn tách khỏi đường bơm. Giữ nguyên dây đen và dây vàng.

Rồi chạy `pio run -e test_flowmap -t upload -t monitor`:

| Kết quả | Kết luận |
|---|---|
| Lúc bơm chạy về gần 0 Hz | nhiễu dẫn theo **nguồn cảm biến** → tách nguồn, hoặc thêm tụ 470 µF và đất hình sao |
| Vẫn 1 500 Hz | nhiễu dẫn theo **đường đất chung** → phải đi lại đất theo hình sao, gộp tại cọc âm của buck |

Đây là cách duy nhất còn lại để tách hai khả năng, và nó không tốn gì ngoài việc dời hai sợi dây.

## Bơm không phải nguyên nhân duy nhất — số đo đã loạn từ trước khi bơm chạy

Phép thử `test_pumpecho` phát siêu âm 200 ms một lần và đóng cắt rơ le giữa chừng, để tách nhiễu điện khỏi gợn nước. Kết quả bác bỏ **cả hai** giả thuyết trước đó:

| Vòng | Rơ le NGẮT · bơm không chạy | Rơ le ĐÓNG · bơm chạy |
|---|---|---|
| 1 | dải dao động **5,04 cm** | 26,89 cm |
| 2 | dải dao động **3,36 cm** | 17,75 cm |

**Số đo đã loạn 3–5 cm trong khi bơm chưa hề chạy.** Nếu nguyên nhân là nhiễu điện do bơm, đoạn rơ le ngắt phải sạch. Nó không sạch. Vì vậy hạ cầu chia áp ECHO xuống 1k/2k sẽ **không** chữa được chuyện này — đừng đổi vội.

Nhưng bơm vẫn làm nặng thêm khoảng **năm lần**, nên điốt 1N4007 song song ngược hai cực bơm vẫn đáng gắn.

### Biến thật sự là mực nước, không phải cái bơm

Đối chiếu với phép đo cùng ngày, cũng bơm tắt, nhưng bồn có **3,88 cm nước**:

| Điều kiện | Dải dao động |
|---|---|
| Bơm tắt, nước 3,88 cm | **0,02 cm** |
| Bơm tắt, bồn cạn | **3,36 – 5,04 cm** |
| Bơm chạy, bồn cạn | 17,75 – 26,89 cm |

Chênh nhau **hơn 200 lần** giữa hai dòng đầu, mà cả hai đều bơm tắt. Biến duy nhất là có nước hay không.

Lý do vật lý: mặt nước phẳng là **gương phản xạ âm gần như hoàn hảo** — gần hết năng lượng dội thẳng về. Đáy bồn khô thì không: sóng tán ra, chạm thành ống lặng, dội qua dội lại rồi mới về, nên thời gian bay dài hơn thực tế. Nhìn lại số liệu thấy đúng vậy: lúc bồn cạn cảm biến đọc **16 đến 21 cm**, tức **xa hơn cả đáy bồn ở 15,5 cm** — một khoảng cách không tồn tại về mặt hình học.

### Hệ quả với thiết kế

Cảm biến chính xác nhất đúng lúc ta cần nó nhất là lúc **gần đầy**, và tệ nhất lúc **gần cạn**. Với bài toán chống tràn thì đó là chiều thuận lợi. Việc còn lại — biết khi nào bồn đã cạn để bật bơm — nên giao cho **phao mức thấp**, là tiếp điểm cơ khí không phụ thuộc quang học chút nào. Firmware đã làm vậy: `floatMin` là một trong hai điều kiện cho phép bật bơm, song song với ngưỡng phần trăm.

Nếu muốn cải thiện thêm, hãy kiểm tra **đường kính ống lặng**. Chùm sóng 15° ở khoảng cách 15,5 cm đã rộng hơn 8 cm. Ống hẹp hơn con số đó thì chính thành ống trở thành nguồn phản xạ giả khi chưa có nước che đáy.

## HC-SR04 không nhìn thấy lớp nước mỏng ở đáy bồn

Cảm biến cách đáy 15,5 cm, sai số đo ±1 cm. Vì vậy **lớp nước dưới 1 cm nằm lọt trong nhiễu** — phần mềm không phân biệt được "cạn hẳn" với "còn một chút".

Hiện `0%` trong tình huống đó là nói dối: người vận hành nhìn vào bồn vẫn thấy nước, và nếu ống xả đang mở thì vẫn thấy nước chảy. Dashboard giờ hiện **"≤ 1 cm · dưới ngưỡng đo"** thay vì `0%`.

Điều này cũng có nghĩa là **đừng dùng số đo mức làm trọng tài** khi nó mâu thuẫn với thứ quan sát trực tiếp được. Trong ba cảm biến của hệ, HC-SR04 là cái kém tin cậy nhất: chùm sóng 15° rộng hơn lòng thùng 10×10 cm, mặt nước gợn làm tán tiếng dội, và lớp đáy thì nằm dưới ngưỡng.

## Vì sao cảm biến lưu lượng đầu ra luôn đọc 0 dù van xả luôn mở

Đây **không phải** lỗi đấu dây, không phải đấu ngược, và không phải cảm biến hỏng. Đo ngày 22/09, bơm tắt, van xả mở:

| | |
|---|---|
| Mức nước | 5,57 → 4,13 cm trong 59 giây |
| Lưu lượng xả **thật** | **0,147 L/phút** |
| Dải làm việc YF-S401 | **0,30 – 6,00 L/phút** |

Lưu lượng xả nằm **dưới đáy dải đo hai lần**. Dưới cận dưới thì áp lực nước không đủ thắng ma sát để quay cánh quạt — cánh đứng yên, không có xung nào sinh ra, và cảm biến đọc 0. Đó là **kết quả đúng với vật lý**, không phải sự cố.

Ba cách chữa, theo thứ tự dễ làm:

1. **Mở van to hơn** cho lưu lượng vượt 0,3 L/phút. Kiểm chứng bằng chính phép đo trên: nước phải tụt nhanh hơn 0,3 cm mỗi 10 giây.
2. **Đổi sang cảm biến hợp dải**, ví dụ loại hiệu ứng Hall cỡ nhỏ đo được từ 0,05 L/phút, hoặc cảm biến kiểu cánh quạt trong ống 1/4 inch.
3. **Bỏ đo định lượng đầu ra**, chỉ dùng mức nước để suy ra lượng tiêu thụ. Mực nước sau khi lọc đã đủ mượt để làm việc này: phép đo 0,147 L/phút ở trên chính là tính từ mức nước, và nó đáng tin hơn cảm biến đang lắp.

## ESP32 chỉ bắt được Wi-Fi 2,4 GHz

Đo ngày 22/09: máy tính nối mạng `NhanHuynh` bình thường, nhưng ở **5745 MHz**, tức băng **5 GHz**. ESP32 quét ra 31 mạng quanh đó mà **không có `NhanHuynh`** trong danh sách — vì phần cứng Wi-Fi của nó chỉ có băng 2,4 GHz.

Triệu chứng rất dễ chẩn đoán nhầm: điện thoại và máy tính đều vào mạng ngon lành, nên người ta tưởng mạng không có vấn đề gì và đi tìm lỗi ở chỗ khác. Thiết bị thì báo `WL_DISCONNECTED`, mã 6.

Cách chữa nằm ở **điểm phát**, không nằm ở code: vào cài đặt điểm phát di động và đổi băng tần sang **2,4 GHz**, hoặc bật chế độ tương thích nếu máy có. Máy tính sẽ tự nối lại vào cùng dải IP nên `MQTT_HOST` không cần sửa.

Chạy `pio run -e test_wifiscan -t upload -t monitor` để xem chính ESP32 nhìn thấy những mạng nào. Không thấy tên mạng trong danh sách thì không phải lỗi mật khẩu, và không có tham số nào trong firmware sửa được.

## Cách nối điện trở 4,7 kΩ cho hai cảm biến YF-S401

### Trước khi nối: kiểm tra mô đun có sẵn điện trở kéo lên chưa

Cấp nguồn 5 V cho cảm biến, **dây vàng chưa nối vào ESP32**, đo điện áp dây vàng so với GND bằng đồng hồ:

| Đo được | Nghĩa là | Làm gì |
|---|---|---|
| khoảng **0 V** hoặc trôi nổi | không có điện trở kéo lên sẵn | làm theo hướng dẫn dưới đây |
| khoảng **5 V** | đã có sẵn điện trở kéo lên 5 V | **đừng** làm theo mục này — dùng cầu chia áp 10k/20k và đặt `FLOW_PIN_PULLUP = 0` |

Bỏ qua bước này mà mô đun lại có sẵn điện trở kéo lên 5 V thì bạn đang đưa 5 V vào chân ESP32.

### Sơ đồ nối

YF-S401 có ba dây: **đỏ = 5 V**, **đen = GND**, **vàng = tín hiệu**.

```
                            ESP32
                        ┌───────────┐
      ┌──────────────── │ 3V3       │
      │                 │           │
     ┌┴┐  R1            │           │
     │ │  4,7 kΩ        │           │
     └┬┘                │           │
      │                 │           │
      ├──────────────── │ GPIO 19   │   cảm biến ĐẦU VÀO
      │                 │           │
   vàng                 │           │
  ┌───┴────┐            │           │
  │ YF-S401│ đen ────── │ GND       │
  │  VÀO   │ đỏ  ─────────────────────── 5 V (từ cọc buck, nhánh sạch)
  └────────┘            │           │
                        │           │
      ┌──────────────── │ 3V3       │   (cùng một chân 3V3)
      │                 │           │
     ┌┴┐  R2            │           │
     │ │  4,7 kΩ        │           │
     └┬┘                │           │
      │                 │           │
      ├──────────────── │ GPIO 4    │   cảm biến ĐẦU RA
      │                 │           │
   vàng                 │           │
  ┌───┴────┐            │           │
  │ YF-S401│ đen ────── │ GND       │
  │   RA   │ đỏ  ─────────────────────── 5 V
  └────────┘            └───────────┘
```

Nói bằng lời, làm hai lần giống hệt nhau:

1. Dây **vàng** của cảm biến cắm thẳng vào chân GPIO. Không qua bộ chuyển mức, không qua cầu chia áp.
2. Một điện trở **4,7 kΩ** nối từ **chính chân GPIO đó** lên **chân 3V3** của ESP32.
3. Dây **đen** về GND chung với ESP32.
4. Dây **đỏ** lấy 5 V từ cọc buck, nhánh sạch — **không lấy từ nhánh bơm**.

Trên breadboard: cắm dây vàng vào một hàng, cắm một chân điện trở vào **cùng hàng đó**, chân còn lại cắm vào đường ray 3,3 V.

### Vì sao phải là 3,3 V chứ không phải 5 V

**Nối lên 5 V là hỏng chân ESP32.** Ngõ ra hall của YF-S401 là **cực thu hở** — nó chỉ kéo xuống đất, không tự đẩy điện áp ra. Mức cao hoàn toàn do điện trở kéo lên quyết định. Kéo lên 3,3 V thì mức cao là 3,3 V, đúng bằng thứ ESP32 chịu được. Kéo lên 5 V thì mức cao là 5 V, và chân GPIO chỉ chịu 3,6 V.

Đó cũng là lý do cách này an toàn mà **không cần bộ chuyển mức nào cả**.

### Vài con số để yên tâm

| | |
|---|---|
| Dòng qua mỗi điện trở khi cảm biến kéo xuống | 3,3 V ÷ 4,7 kΩ = **0,70 mA** |
| Tổng hai cảm biến | 1,4 mA — chân 3V3 của ESP32 dư sức |
| Trở kháng đường tín hiệu | 45 kΩ → **4,3 kΩ** (4,7 k song song với kéo lên nội bộ 45 k) |

Giảm hơn mười lần chính là thứ dập tắt cảm ứng 50 Hz. Giữ nguyên `FLOW_PIN_PULLUP = 1` trong `config.h` — điện trở nội bộ mắc song song không gây hại, điện trở ngoài 4,7 kΩ áp đảo nó.

### Ba điều dễ làm hỏng kết quả

- **Đặt điện trở sát chân ESP32**, không đặt ở đầu cảm biến. Đoạn dây từ cảm biến tới điện trở vẫn còn trở kháng cao.
- **Tách dây vàng khỏi dây bơm.** Đừng bó chung, đừng chạy song song. Cắt ngang thì cắt vuông góc.
- **Điốt 1N4007 song song ngược hai cực bơm**, vạch trắng quay về cực dương. Không có nó thì mỗi lần rơ le ngắt là một xung vài trăm vôn phóng vào toàn mạch.

### Kiểm chứng sau khi nối

Khoá van xả, tắt bơm, rồi chạy:

```bash
cd ~/Documents/IOT/IOT && pio run -e test_flownoise -t upload -t monitor
```

Cột **kéo lên 3,3 V** của cả hai chân phải cho **0 Hz**, giống hệt chân đối chiếu GPIO 23. Còn thấy 50 Hz là điện trở chưa ăn — kiểm tra lại xem nó có thực sự nối vào đúng hàng của dây vàng không.

## 50 Hz trên dây tín hiệu là điện lưới, không phải nước

Đo ngày 21/09, bơm tắt, chế độ thủ công:

| | Tần số xung |
|---|---|
| GPIO 19 | **50,00 Hz** — đúng, không xê dịch, mọi mẫu |
| GPIO 4 | 20 – 35 Hz, dao động |

Một cảm biến lưu lượng không thể cho đúng 50,00 Hz suốt hàng chục giây trong khi dòng chảy thay đổi. Đó là **điện lưới cảm ứng vào dây tín hiệu đang thả nổi** ở trở kháng cao. Trong cùng một lần chạy trước đó, GPIO 19 lúc cho 0 Hz lúc cho 50,5 Hz — nó bám vào rồi nhả ra khỏi điện lưới.

Đây chính là lý do **điện trở 4,7 kΩ lên 3,3 V là bắt buộc**, không phải tuỳ chọn. Nó hạ trở kháng đường tín hiệu xuống mười lần và dập tắt cảm ứng này.

## YF-S401 có thể sai dải đo cho hệ này

Dải làm việc của YF-S401 là **0,3 – 6 L/phút**. Dưới cận dưới thì cánh quạt không đủ lực quay, và cảm biến im lặng — không phải vì hỏng, mà vì nó không được thiết kế cho lưu lượng đó.

Lưu lượng thật của hệ này, đo bằng tốc độ đổi mực nước:

| Đường | Lưu lượng đo được | So với dải 0,3–6 L/phút |
|---|---|---|
| Bơm đẩy vào | **0,36 L/phút** | sát đáy dải, chỉ dư 20% |
| Xả trọng lực | **0,134 L/phút** | **dưới đáy dải** |

Nghĩa là ngay cả khi đấu dây hoàn hảo, cảm biến đầu ra nhiều khả năng vẫn đọc 0 trong lúc nước đang xả thật. Và cảm biến đầu vào thì làm việc ngay mép dải, nơi sai số hệ số K lớn nhất.

Muốn đo được dải này cần cảm biến nhỏ hơn, hoặc chấp nhận rằng lưu lượng chỉ dùng để **phát hiện có hay không có dòng chảy**, chứ không dùng làm số định lượng. Cách thứ hai vẫn đủ cho luật `DRY_RUN`.

## YF-S401 qua TXS0108E: cũng hỏng, và theo một kiểu khác

TXS0108E **danh nghĩa là hợp** với ngõ ra cực thu hở như YF-S401 — nó vốn sinh ra cho bus kiểu I²C. Nhưng trên bo mạch này nó vẫn hỏng, vì ba lý do chồng lên nhau.

**Thứ nhất, nó đã có sẵn điện trở kéo lên khoảng 10 kΩ ở cả hai phía.** TI ghi rõ trong datasheet: điện trở kéo lên gắn thêm từ bên ngoài phải **trên 50 kΩ**, nếu không sẽ phá mạch dò chiều. Điện trở kéo lên nội bộ của ESP32 chỉ khoảng **45 kΩ** — nằm ngay dưới ngưỡng đó. `FLOW_PIN_PULLUP = 1` trong `config.h` chính là đang bật nó.

**Thứ hai, mạch *one-shot* tăng tốc sườn.** Mỗi lần thấy sườn, nó đạp một dòng lớn trong chốc lát rồi nhả ra. Nếu lúc nhả mà mức tín hiệu trôi về ngưỡng cũ, nó lại thấy một sườn nữa và lại đạp. Đó là **tự dao động**, và tần số của nó nằm đúng dải vài kHz.

**Thứ ba, tín hiệu lưu lượng lúc không có nước là tín hiệu đứng yên.** TXS0108E được thiết kế cho tín hiệu có nhịp, không cho mức tĩnh kéo dài. Đứng yên là lúc nó dễ trôi và dễ chattering nhất.

### Số đo khớp với chẩn đoán này

Bơm đã rút, không một giọt nước chảy:

| | Kéo lên 3,3 V | Không kéo | Kéo xuống đất |
|---|---|---|---|
| GPIO 4 (vào) | 3 445 Hz | 13 403 Hz | 3 150 Hz |
| GPIO 19 (ra) | 3 282 Hz | 12 123 Hz | 2 938 Hz |
| **GPIO 23** (không nối gì) | **0 Hz** | 51 Hz | **0 Hz** |

Ba điều trong bảng này chỉ thẳng vào TXS0108E:

- **Hai kênh cho tần số gần bằng nhau.** Hai cảm biến rời nhau không có lý do gì trùng nhau tới 5%. Cùng một con chip thì có.
- **Tắt Wi-Fi không làm thay đổi gì.** Vậy không phải nhiễu sóng vô tuyến — là chính con chip đang dao động.
- **Tần số đổi theo cách cấu hình chân.** Chân thả nổi thật thì im (GPIO 23 cho 0 Hz). Chân bị một nguồn thật điều khiển thì không đổi theo cách kéo. Chỉ có mạch dò chiều đang bị điện trở kéo của ESP32 quấy nhiễu mới cho kiểu này.

### Bỏ chip rồi vẫn còn nhiễu: bơm đang bơm vào dây tín hiệu

Sau khi bỏ TXS0108E, nền nhiễu lúc bơm tắt giảm khoảng 100 lần, từ 3 445 Hz xuống 26 Hz. Nhưng **lúc bơm chạy** thì cả hai chân cùng vọt lên:

| | GPIO 4 | GPIO 19 |
|---|---|---|
| Bơm tắt | 26,4 Hz | 25,2 Hz |
| **Bơm chạy** | **4 755,9 Hz** | **4 722,0 Hz** |

4 750 Hz chia hệ số K 98 ra 48 L/phút, trong khi bơm thật chỉ đẩy 0,36 L/phút. Và hai kênh trùng nhau tới **0,7%** — hai cảm biến rời nhau không thể trùng như vậy. Đây là **nhiễu đồng pha** do bơm bơm vào cả hai đường tín hiệu cùng lúc.

Bốn thứ cần làm, theo thứ tự hiệu quả:

1. **Điốt 1N4007 song song ngược hai cực bơm.** Vạch trắng quay về cực dương. Khi rơ le ngắt, cuộn dây động cơ sinh xung ngược vài trăm vôn; điốt cho nó tự tiêu tán tại chỗ thay vì phóng ra toàn mạch.
2. **Điện trở 4,7 kΩ từ mỗi chân tín hiệu lên 3,3 V.** Điện trở kéo lên nội bộ 45 kΩ quá yếu, để đường tín hiệu ở trở kháng cao và biến nó thành ăng ten.
3. **Tách dây bơm khỏi dây tín hiệu.** Đừng bó chung, đừng chạy song song. Cắt ngang nhau thì cắt vuông góc.
4. **Tụ 470 µF trên nhánh cấp nguồn bơm**, đặt gần rơ le, và đất đi hình sao như mục nguồn mô tả.

Chừng nào bơm còn bơm 4 750 Hz vào dây tín hiệu thì **không thể biết hai cảm biến có bị đấu ngược hay không**, vì nhiễu át hoàn toàn tín hiệu thật.

### Cách làm đúng: bỏ hẳn bộ chuyển mức

Ngõ ra hall của YF-S401 là **cực thu hở** — nó chỉ kéo XUỐNG đất, không bao giờ đẩy 5 V ra. Nối thẳng dây tín hiệu vào GPIO, rồi gắn **điện trở 4,7 kΩ lên 3,3 V**. Mức cao khi đó do điện trở quyết định, tức đúng 3,3 V. Cảm biến vẫn cấp nguồn 5 V bình thường. Không cần, và không nên, có bộ chuyển mức ở đây.

**Kiểm tra trước khi nối thẳng.** Một số mô đun có sẵn điện trở kéo lên 5 V; loại đó nối thẳng là đưa 5 V vào chân ESP32. Cấp nguồn cho cảm biến, **chưa nối vào ESP32**, đo điện áp chân tín hiệu bằng đồng hồ:

| Đo được | Nghĩa là | Đấu thế nào |
|---|---|---|
| khoảng 0 V hoặc trôi nổi | không có điện trở kéo lên sẵn | nối thẳng + 4,7 kΩ lên 3,3 V, đặt `FLOW_PIN_PULLUP = 1` |
| khoảng 5 V | có điện trở kéo lên sẵn | qua chia áp 10k/20k, đặt `FLOW_PIN_PULLUP = 0` |

Xong rồi chạy `pio run -e test_flownoise -t upload -t monitor`. Đấu đúng thì cả hai chân phải cho **0 Hz** ở cả ba kiểu kéo, giống hệt chân đối chiếu GPIO 23.

### Hai phao và chân rơ le cũng vậy

Hai phao chỉ là công tắc nối xuống đất — **nối thẳng**, không có gì để chuyển mức. Chân điều khiển rơ le là ngõ ra của ESP32; nếu nó cũng đang đi qua TXS0108E thì mạch one-shot có thể sinh xung giả ngay trên đường bật tắt bơm. Cho nó đi thẳng.

Nói gọn: trong toàn bộ mạch này, **chỗ duy nhất cần hạ áp là chân ECHO của HC-SR04 và ngõ ra ACS712**, và cả hai đều dùng cầu chia áp điện trở chứ không dùng chip.

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
