# Kịch bản thuyết trình — Bồn nước thông minh (Project 08)

Bản tiếng Việt của kịch bản, đi theo đúng 43 slide tiếng Anh. Dùng để **hiểu và tập**. Khi lên thuyết trình bằng tiếng Anh thì đọc bản `SCRIPT_EN.md` (cũng chính là phần ghi chú người nói trong từng slide).

- Phần kỹ thuật khoảng **50 phút** nếu nói chậm, rõ (khoảng 145 từ tiếng Anh mỗi phút). Sau đó là **10–15 phút demo**.
- Slide 8 là slide nhấn mạnh. Nói chậm lại và dừng một nhịp.
- Slide mở đầu mỗi phần (nền tối, số to) chỉ nói một câu rồi chuyển.
- Slide 10 (ảnh mô hình) chờ bổ sung ảnh. Chỗ nào có ngoặc vuông như [số nhóm] thì tự điền.

| # | Slide | Người nói | Thời gian | Hết lúc |
|---|---|---|---|---|
| 1 | Smart Water Tank | Bảo | 0:45 | 0:45 |
| 2 | Seven parts, then the live demo | Bảo | 0:30 | 1:15 |
| 3 | 01 · Problem and requirements | Bảo | 0:10 | 1:25 |
| 4 | What a smart tank must do | Bảo | 1:05 | 2:30 |
| 5 | Requirements and where they are met | Bảo | 1:10 | 3:40 |
| 6 | 02 · Architecture and hardware | Bảo | 0:10 | 3:50 |
| 7 | System architecture | Bảo | 1:20 | 5:10 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | Bảo | 1:00 | 6:10 |
| 9 | The bench rig | Bảo | 1:20 | 7:30 |
| 10 | The prototype on the bench | Bảo | 0:30 | 8:00 |
| 11 | Signal wiring and power path | Bảo | 1:35 | 9:35 |
| 12 | Three signal lessons, each found by measuring | Bảo | 1:55 | 11:30 |
| 13 | 03 · MQTT protocol | Nhân | 0:10 | 11:40 |
| 14 | MQTT topic design | Nhân | 1:10 | 12:50 |
| 15 | Commands with acknowledgement | Nhân | 1:30 | 14:20 |
| 16 | 04 · Control and intelligence | Nhân | 0:10 | 14:30 |
| 17 | Level measurement pipeline | Nhân | 1:50 | 16:20 |
| 18 | Model bridging and flow from level | Nhân | 1:45 | 18:05 |
| 19 | Controller state machine | Nhân | 1:35 | 19:40 |
| 20 | Eleven fault rules | Nhân | 1:25 | 21:05 |
| 21 | Safety guard and overflow layers | Nhân | 1:15 | 22:20 |
| 22 | Pump health classification | Nhân | 1:25 | 23:45 |
| 23 | Rolling baseline and daily volume | Nhân | 1:25 | 25:10 |
| 24 | 05 · Backend, dashboard, security | Ngân | 0:10 | 25:20 |
| 25 | Backend and data model | Ngân | 1:05 | 26:25 |
| 26 | Dashboard and remote access | Ngân | 1:20 | 27:45 |
| 27 | Security model | Ngân | 1:40 | 29:25 |
| 28 | Behaviour during a network outage | Ngân | 1:35 | 31:00 |
| 29 | 06 · Experiments and results | Uyên | 0:10 | 31:10 |
| 30 | Experiment plan | Uyên | 1:05 | 32:15 |
| 31 | Level calibration against a ruler | Uyên | 1:30 | 33:45 |
| 32 | Control response and switching | Uyên | 1:30 | 35:15 |
| 33 | Volume estimation error | Uyên | 1:25 | 36:40 |
| 34 | Command latency | Uyên | 1:30 | 38:10 |
| 35 | Fault detection | Uyên | 1:45 | 39:55 |
| 36 | Network outage with the pump running | Uyên | 1:50 | 41:45 |
| 37 | Automated system test, 5/5 pass | Uyên | 1:05 | 42:50 |
| 38 | 07 · Lessons and conclusion | Dương | 0:10 | 43:00 |
| 39 | Bugs found on the bench | Dương | 2:10 | 45:10 |
| 40 | Limitations and next steps | Dương | 1:30 | 46:40 |
| 41 | Conclusion | Dương | 1:40 | 48:20 |
| 42 | Live demonstration | Dương | 1:35 | 49:55 |
| 43 | Thank you | Dương | 0:15 | 50:10 |

---

## 1. Smart Water Tank · 0:45

Chào thầy và các bạn. Nhóm em là nhóm [số nhóm], đề tài Project 08: Bồn nước thông minh. Nói gọn trong một câu: nhóm em làm một bồn nước thật, cỡ nhỏ. Bơm của bồn do ESP32 điều khiển. ESP32 đo mức nước và lượng nước dùng, phát hiện sự cố, và cho phép theo dõi bồn từ bất cứ đâu qua MQTT và một trang web.

Nếu chỉ nhớ một ý của bài này, xin hãy nhớ ý này: mọi quyết định điều khiển và mọi quyết định an toàn đều nằm trên chính con ESP32. Vì vậy bồn vẫn tự chạy đúng khi mất mạng. Bài trình bày có bảy phần, khoảng năm mươi phút, sau đó là demo trên bồn thật.

## 2. Seven parts, then the live demo · 0:30

Kế hoạch như sau. Phần một là bài toán và yêu cầu của đề. Phần hai là kiến trúc và phần cứng, gồm cả quyết định thiết kế quan trọng nhất. Phần ba là giao thức MQTT. Phần bốn là phần lõi: điều khiển và chẩn đoán trên ESP32. Phần năm là backend, dashboard và bảo mật. Phần sáu là thí nghiệm và kết quả đo. Phần bảy là các lỗi đã gặp, các hạn chế và kết luận. Cuối cùng nhóm chuyển sang bồn thật để demo.

## 3. 01 · Problem and requirements · 0:10

Phần một: bài toán và yêu cầu của đề.

## 4. What a smart tank must do · 1:05

Phần lớn nhà ở Việt Nam vẫn dùng phao cơ trong bồn chứa. Phao rẻ và bền, nhưng chỉ trả lời đúng một câu hỏi: bồn đầy chưa. Chủ nhà không biết mình đã dùng bao nhiêu nước. Họ không thấy rò rỉ cho tới khi hóa đơn tăng, và cũng không biết bơm đang chạy khô cho tới khi bơm cháy.

Đề bài yêu cầu sáu việc như trên slide:
- **Đo** mức và lưu lượng, có đối chiếu với chuẩn vật lý.
- **Điều khiển** mức nước trong một dải để bơm không bật tắt liên tục.
- **Bảo vệ** để bồn không bao giờ tràn, kể cả khi cảm biến hỏng hay có người ra lệnh tay ẩu.
- **Chẩn đoán** sự cố, ví dụ bơm chạy mà không bơm được nước.
- **Giám sát** từ xa.
- Ô màu tối là yêu cầu bao trùm tất cả: mất mạng thì bồn vẫn phải tự điều khiển.

Chính yêu cầu cuối cùng này quyết định toàn bộ kiến trúc của nhóm.

## 5. Requirements and where they are met · 1:10

Bảng này lấy từng yêu cầu của đề, nói nhóm đáp ứng thế nào và trạng thái thật; nó trùng với Bảng 1.3.1 trong báo cáo. Hầu hết các dòng đã đạt và có chứng minh. Có hai điểm nhóm nói thẳng.

Thứ nhất là cảm biến lưu lượng. Hai cảm biến đều có lắp, có đấu dây, có đọc. Từ lần đi lại dây ngày 5/10, cảm biến đầu vào đếm được nước thật và đã hiệu chuẩn theo thước. Nhưng điều khiển và bộ đếm thể tích vẫn dùng lưu lượng suy từ mức nước, còn cảm biến đầu ra thì dưới ngưỡng khởi động ở lưu lượng xả của bàn thử. Vì vậy nhóm ghi là "đã kiểm chứng một phần", không ghi đơn giản là "đạt".

Thứ hai là phần nâng cao: nhóm làm cả hai lựa chọn của đề, đường nền trượt và bộ phân loại dựa trên dòng điện với lưu lượng. Bộ phân loại chạy tốt và sẽ demo; đường nền cần ba ngày dữ liệu cho mỗi khe nửa giờ mới được báo động, số ngày thử của nhóm chưa đủ.

## 6. 02 · Architecture and hardware · 0:10

Phần hai: kiến trúc và phần cứng.

## 7. System architecture · 1:20

Đây là toàn bộ hệ thống trên một slide, giống hệt Hình 2.1.1 trong báo cáo. Bên trong khung xanh nét đứt là nút biên: cảm biến đưa dữ liệu vào firmware ESP32, firmware chạy vòng điều khiển kín mỗi 200 mili giây và đóng cắt rơ-le, bơm trực tiếp. Vòng này vẫn chạy khi mọi thứ bên dưới khung đã mất.

ESP32 gửi telemetry, trạng thái, lỗi và last will lên broker Mosquitto trên laptop, và nhận lệnh ở topic cmd, mỗi lệnh được trả lời ở cmd/ack. Backend FastAPI subscribe, lưu vào sáu bảng SQLite và phục vụ dashboard qua REST API; lệnh từ dashboard đi ngược lại bằng POST. Cuối cùng, đường hầm Cloudflare đưa dashboard ra một địa chỉ HTTPS công khai, ai cũng xem được mà không phải mở cổng vào mạng phòng lab.

So với mô hình năm lớp: lớp cảm nhận là ESP32 và cảm biến, lớp truyền là MQTT qua Wi-Fi, lớp xử lý là FastAPI và SQLite, lớp ứng dụng là dashboard. Chú ý ô màu tối: đường duy nhất tới bơm đi qua nó. Máy chủ chỉ hỏi, không bao giờ tự đóng cắt bơm.

## 8. The server never switches the pump. It asks, and the ESP32 decides. · 1:00

Đây là quyết định quan trọng nhất của đồ án. Bài giảng có một quy tắc đặt chức năng: thao tác nào cần dừng an toàn ngay lập tức thì phải tính ở biên. Nhóm áp dụng quy tắc này triệt để.

Những gì cần chu kỳ ổn định hoặc phải sống sót khi mất mạng đều nằm trên ESP32: lọc tín hiệu, máy trạng thái, cả mười một luật lỗi, hàm rào an toàn, bộ phân loại sức khỏe bơm, và phép cộng dồn thể tích.

Máy chủ chỉ giữ những việc vi điều khiển làm không tốt: lịch sử dài hạn, báo cáo theo ngày, và đường nền rò rỉ cần nhớ dữ liệu nhiều ngày.

Kết quả là hệ thống xuống cấp có kiểm soát. Mất mạng thì người dùng mất khả năng xem và ra lệnh từ xa. Nhưng bồn vẫn tự bơm đầy đúng ngưỡng và mọi luật an toàn vẫn chạy. Yêu cầu chạy khi mất mạng trở thành hệ quả của kiến trúc, chứ không phải tính năng gắn thêm vào sau.

## 9. The bench rig · 1:20

Bồn của nhóm cố ý làm nhỏ: đáy 10 nhân 10 cm, nên một centimet mực nước đúng bằng 0,1 lít. Mặt cảm biến cách đáy 15,88 cm, đo bằng thước là sáu và một phần tư inch. Dải làm việc là 12,4 cm dưới cùng, tức 1,24 lít, chừa 3,5 cm không khí dưới cảm biến khi bồn đầy. Một chu kỳ bơm đầy rồi xả hết chỉ mất vài phút, nên mỗi buổi thử quan sát được nhiều chu kỳ.

Con số màu cam là quan trọng nhất trên slide. Datasheet hứa 1,67 lít mỗi phút. Nhóm đo thật bằng cách đóng van xả và bấm giờ: 1,2 cm trong 20 giây, tức 0,36 lít mỗi phút, ít hơn gần năm lần do cột áp và ống. Và con số đó cũng không cố định: ngày 5/10, khi xô nguồn thấp hơn, cùng con bơm chỉ cho 0,24. Mọi ngưỡng thời gian trong firmware đều lấy từ số đo, không lấy từ datasheet. Bảng bên dưới liệt kê cảm biến và cách xử lý tín hiệu để vào chân 3,3 V của ESP32.

## 10. The prototype on the bench · 0:30

[Điền khi có ảnh.] Bên trái là mô hình đã lắp: bồn trong suốt với cảm biến siêu âm trên giá, bơm chìm, hai cảm biến lưu lượng và bo mạch điều khiển. Bên phải là cách bố trí thí nghiệm, một vòng nước kín: xô nguồn, bơm, bồn, van xả đóng vai người dùng, và laptop chạy broker cùng backend.

## 11. Signal wiring and power path · 1:35

Slide này khớp với Hình 2.4.1 và 2.4.2 trong báo cáo. Bên trái là từng chân của ESP32 và cách xử lý tín hiệu. Chân echo của cảm biến siêu âm là xung 5 V nên đi qua cầu chia 10k và 20k; chân trigger nối thẳng. Hai cảm biến lưu lượng là ngõ ra cực thu hở, có điện trở kéo lên 4,7 kΩ lên 3,3 V; nhờ vậy hết bắt nhiễu 50 Hz như khi dùng điện trở kéo lên nội bộ yếu. Ngõ ra cảm biến dòng qua cầu chia một một vào GPIO 34, chân chỉ đọc vào.

Rơ-le chuyển sang GPIO 10 từ ngày 5/10. Chân này bình thường nối với chip flash, chỉ dùng được khi flash chạy chế độ DIO hai đường, nên bản build đặt chế độ đó; nhóm đã thử trên bàn là rơ-le đóng cắt mà chip không bị reset.

Bên phải là đường nguồn. 12 V qua cầu chì 2 A vào bộ hạ áp 5 V 5 A. Mỗi tải đi một dây riêng từ cọc ra của bộ hạ áp, kiểu hình sao. Đường bơm đi qua tiếp điểm rơ-le trước, rồi tới cảm biến dòng, rồi mới tới bơm, nên dòng đo được đúng là dòng của bơm. Bơm có diode chống ngược và tụ 100 nF ở hai cực, còn tụ 470 µF trên bo điều khiển đỡ dòng khởi động.

## 12. Three signal lessons, each found by measuring · 1:55

Ba bài học về tín hiệu, bài nào cũng tìm ra bằng cách đo chứ không phải đọc code. Thứ nhất, chân echo ban đầu đi qua bộ chuyển mức tự động TXS0108E. Linh kiện này làm cho bus cực máng hở, điện trở kéo lên và mạch tăng tốc sườn của nó làm méo xung echo, mà độ rộng xung chính là phép đo. Nhóm mất khoảng ba mươi phần trăm tiếng dội. Thay bằng cầu chia điện trở là hết: không trượt lần nào trong 179 lần đo.

Thứ hai, cảm biến lưu lượng báo có nước khi van đang đóng. Tần số đúng 50,00 Hz, tần số điện lưới: điện trở kéo lên nội bộ quá yếu biến dây thành ăng-ten. Điện trở kéo lên ngoài 4,7 kΩ đưa về không.

Thứ ba, khi bơm chạy, hai dây lưu lượng đọc khoảng 1500 Hz cho tới ngày 22/9, trong khi nước thật chỉ cho 35 Hz. Diode và việc tách dây không đổi được gì, nên nhiễu đi theo đường nguồn và đất. Ngày 5/10 nhóm làm lại tầng công suất: rơ-le đứng trước cảm biến dòng, diode và tụ ở bơm, tụ 470 µF trên bo, đổi chỗ hai chân lưu lượng. Từ đó cảm biến đầu vào đọc không khi bơm tắt và 20 đến 30 Hz nước thật khi bơm chạy. Nhóm đã hiệu chuẩn theo thước, 85,5 xung cho mỗi lít trên phút, và nó đang cấp dữ liệu cho mô hình bồn.

Nhưng nhóm nói chính xác: yêu cầu cảm biến đã thực hiện và mới kiểm chứng một phần. Điều khiển và bộ đếm thể tích vẫn dùng lưu lượng suy từ mức nước, vì cảm biến mới được kiểm chứng trong một lần hiệu chuẩn, chưa qua nhiều chu kỳ bơm đầy. Cảm biến đầu ra thì dưới ngưỡng khởi động ở lưu lượng xả của nhóm.

## 13. 03 · MQTT protocol · 0:10

Slide này khớp với Hình 2.4.1 và 2.4.2 trong báo cáo. Bên trái là từng chân của ESP32 và cách xử lý tín hiệu. Chân echo của cảm biến siêu âm là xung 5 V nên đi qua cầu chia 10k và 20k; chân trigger nối thẳng. Hai cảm biến lưu lượng là ngõ ra cực thu hở, có điện trở kéo lên 4,7 kΩ lên 3,3 V; nhờ vậy hết bắt nhiễu 50 Hz như khi dùng điện trở kéo lên nội bộ yếu. Ngõ ra cảm biến dòng qua cầu chia một một vào GPIO 34, chân chỉ đọc vào.

Rơ-le chuyển sang GPIO 10 từ ngày 5/10. Chân này bình thường nối với chip flash, chỉ dùng được khi flash chạy chế độ DIO hai đường, nên bản build đặt chế độ đó; nhóm đã thử trên bàn là rơ-le đóng cắt mà chip không bị reset.

Bên phải là đường nguồn. 12 V qua cầu chì 2 A vào bộ hạ áp 5 V 5 A. Mỗi tải đi một dây riêng từ cọc ra của bộ hạ áp, kiểu hình sao. Đường bơm đi qua tiếp điểm rơ-le trước, rồi tới cảm biến dòng, rồi mới tới bơm, nên dòng đo được đúng là dòng của bơm. Bơm có diode chống ngược và tụ 100 nF ở hai cực, còn tụ 470 µF trên bo điều khiển đỡ dòng khởi động.

## 14. MQTT topic design · 1:10

Ba bài học về tín hiệu, bài nào cũng tìm ra bằng cách đo chứ không phải đọc code. Thứ nhất, chân echo ban đầu đi qua bộ chuyển mức tự động TXS0108E. Linh kiện này làm cho bus cực máng hở, điện trở kéo lên và mạch tăng tốc sườn của nó làm méo xung echo, mà độ rộng xung chính là phép đo. Nhóm mất khoảng ba mươi phần trăm tiếng dội. Thay bằng cầu chia điện trở là hết: không trượt lần nào trong 179 lần đo.

Thứ hai, cảm biến lưu lượng báo có nước khi van đang đóng. Tần số đúng 50,00 Hz, tần số điện lưới: điện trở kéo lên nội bộ quá yếu biến dây thành ăng-ten. Điện trở kéo lên ngoài 4,7 kΩ đưa về không.

Thứ ba, khi bơm chạy, hai dây lưu lượng đọc khoảng 1500 Hz cho tới ngày 22/9, trong khi nước thật chỉ cho 35 Hz. Diode và việc tách dây không đổi được gì, nên nhiễu đi theo đường nguồn và đất. Ngày 5/10 nhóm làm lại tầng công suất: rơ-le đứng trước cảm biến dòng, diode và tụ ở bơm, tụ 470 µF trên bo, đổi chỗ hai chân lưu lượng. Từ đó cảm biến đầu vào đọc không khi bơm tắt và 20 đến 30 Hz nước thật khi bơm chạy. Nhóm đã hiệu chuẩn theo thước, 85,5 xung cho mỗi lít trên phút, và nó đang cấp dữ liệu cho mô hình bồn.

Nhưng nhóm nói chính xác: yêu cầu cảm biến đã thực hiện và mới kiểm chứng một phần. Điều khiển và bộ đếm thể tích vẫn dùng lưu lượng suy từ mức nước, vì cảm biến mới được kiểm chứng trong một lần hiệu chuẩn, chưa qua nhiều chu kỳ bơm đầy. Cảm biến đầu ra thì dưới ngưỡng khởi động ở lưu lượng xả của nhóm.

## 15. Commands with acknowledgement · 1:30

Phần ba: giao thức truyền thông.

## 16. 04 · Control and intelligence · 0:10

Cây chủ đề đi từ rộng tới hẹp: bồn nước, địa điểm, thiết bị. Mức QoS chọn riêng cho từng chủ đề bằng một câu hỏi: mất một bản tin thì thiệt hại gì?

- **Telemetry** là dòng dữ liệu 1 Hz. Mất một mẫu không đổi kết luận nào, nên dùng QoS 0.
- **Trạng thái bơm** chỉ gửi khi đổi, nên dùng QoS 1 và bật retain. Dashboard mở sau vẫn nhận ngay trạng thái hiện tại.
- **Sự cố, lệnh và xác nhận** dùng QoS 1, vì mất một bản tin nào trong số đó đều là lỗi chức năng hoặc lỗi an toàn.
- **Status** mang bản tin di chúc (last will). ESP32 biến mất mà không ngắt kết nối đàng hoàng thì chính broker tự công bố "offline".

Nhóm không dùng QoS 2. Lệnh trùng đã được xử lý bằng mã lệnh duy nhất: thiết bị chỉ gửi lại đúng xác nhận cũ, không làm lại hành động.

Mọi bản tin có số thứ tự và dấu thời gian mili giây, nhờ vậy đếm được chính xác số bản tin mất. Broker có danh sách phân quyền (ACL): kể cả ESP32 bị chiếm quyền cũng không gửi được lệnh bơm.

## 17. Level measurement pipeline · 1:50

Giờ tới phần lõi. Cảm biến siêu âm đặt cách đáy 15,88 cm, nên mực nước bằng 15,88 trừ khoảng cách đo được. Nghe đơn giản, nhưng chùm sóng tỏa khoảng mười lăm độ và khi tới mặt nước đã rộng hơn bồn 10 cm, nên một phần tiếng dội về từ thành hoặc đáy. Vì vậy nhóm không bao giờ tin một lần đo đơn lẻ. Mỗi số đo đi qua năm cổng.

Giữ cửa sổ mười lăm lần đo gần nhất, và bỏ mọi khoảng cách xa hơn đáy quá 1 cm, vì chỉ tiếng dội lạc mới tạo ra số đó. Nếu khoảng tứ phân vị của cửa sổ lớn hơn 3,5 cm thì cảm biến đang nhảy giữa hai bề mặt, bỏ cả cửa sổ. Nếu không, lấy phân vị 25 chứ không lấy trung vị: tiếng dội yếu bị bắt trễ một hoặc vài chu kỳ 40 kHz, mỗi chu kỳ khoảng 0,43 cm, nên sai số chỉ làm khoảng cách dài ra, số ngắn hơn là số đáng tin. Sau đó một phép kiểm vật lý bỏ những thay đổi nhanh hơn mức bơm có thể gây ra, hoặc mức thấp hơn mức đã lọc trong lúc đang bơm. Cuối cùng bộ lọc alpha-beta theo dõi cả mức và tốc độ thay đổi, không bị trễ như trung bình trượt.

Mỗi cổng có bộ đếm số lần loại riêng trong telemetry. Nhờ vậy nhóm thấy chín mươi tám phần trăm số lần loại đến từ một cổng dùng lớn nhất trừ nhỏ nhất; đổi sang khoảng tứ phân vị nâng tỉ lệ nhận từ sáu mươi lên chín mươi ba phần trăm. Và riêng cho an toàn, phép đo khoảng cách thô bỏ qua toàn bộ chuỗi lọc: ba lần đo gần hơn 4,5 cm là kích hoạt chống tràn, bất kể bộ lọc nghĩ gì.

## 18. Model bridging and flow from level · 1:45

Nhóm còn chạy một mô hình đơn giản của bồn song song với phép đo. Mực nước thay đổi bằng lưu lượng vào trừ lưu lượng ra, chia cho diện tích đáy. Từ ngày 5/10, lưu lượng vào lấy từ cảm biến đầu vào mỗi khi nó đo được, còn không thì dùng hằng số đã hiệu chuẩn. Điều này quan trọng vì lưu lượng bơm thay đổi theo mực nước trong xô nguồn, từ 0,36 xuống 0,24 lít mỗi phút, mà một mô hình bị trôi thì không phán đoán được số đo mới có đáng tin hay không. Mô hình luôn được kéo nhẹ về mỗi số đo được chấp nhận.

Công dụng thứ nhất là bắc cầu: khi cảm biến không cho số đo nào được nhận, chẳng hạn trong vùng mù quanh sáu mươi phần trăm, máy trạng thái điều khiển theo mô hình, tối đa chín mươi giây; quá thời gian đó thì dừng bơm và báo LEVEL_LOST. Số đo quay lại mà lệch mô hình quá 2,5 cm thì bị loại. Firmware đầu tiên dừng mọi lần bơm như vậy ở khoảng năm mươi phần trăm.

Công dụng thứ hai là lưu lượng hiển thị trên dashboard. Để cộng thể tích, lưu lượng vào vẫn là hằng số đã hiệu chuẩn khi bơm chạy, lưu lượng ra là tốc độ tụt mức nhân diện tích. Một chi tiết: bản đầu tính lưu lượng ra bằng lưu lượng bơm trừ tốc độ dâng ở mọi thời điểm; số hạng bơm đổi tức thời còn ước lượng tốc độ thì trễ, nên mỗi lần bơm bật là đường lưu lượng ra nhảy vọt. Thực tế van mới quyết định lượng xả, không phải bơm, nên nhóm chỉ đo lưu lượng ra khi bơm đã tắt hai mươi giây, rồi giữ giá trị đó trong lúc bơm.

## 19. Controller state machine · 1:35

Bộ điều khiển là máy trạng thái bảy trạng thái, đúng như Bảng 3.3.1 trong báo cáo. Khi khởi động, lệnh đầu tiên là ngắt rơ-le, và thiết bị ở trạng thái BOOT cho tới khi có một số đo mức đầu tiên được nhận. Nhờ vậy giá trị không mà biến mức nước giữ lúc mới cấp điện không bao giờ bị hiểu là bồn cạn. Sau đó thiết bị chờ ở IDLE.

Ở chế độ tự động, khi mực xuống dưới ba mươi phần trăm, hoặc phao dưới đóng, bơm bật và vào FILLING; trên bảy mươi phần trăm, sau ít nhất ba giây, bơm tắt và về IDLE. Mũi tên phía trên là đường tay: lệnh của người vận hành đưa vào MANUAL_ON, nhưng chỉ khi hàm bảo vệ cho phép. Dải ba mươi tới bảy mươi chính là trễ, cùng thời gian chạy tối thiểu ba giây và nghỉ tối thiểu hai mươi giây để bơm không bật tắt liên tục.

Bất kỳ luật lỗi nào, từ FILLING hay MANUAL_ON, cũng dừng bơm và chuyển sang một trong ba trạng thái lỗi. Vì sao dùng máy trạng thái mà không dùng một câu if? Vì trạng thái lỗi phải giữ lại. Lỗi cảm biến tự xóa sau hai giây rưỡi tín hiệu tốt, là mũi tên nét đứt. Lỗi bơm hay khóa chống tràn phải chờ người vận hành, nếu không thì một con bơm đang hút gió sẽ tự chạy lại ngay khi ống chạm nước.

## 20. Eleven fault rules · 1:25

Đề yêu cầu ít nhất một luật lỗi; nhóm làm mười một luật, đúng danh sách Bảng 3.4.1 trong báo cáo. Các luật chạy mỗi hai trăm mili giây theo thứ tự ưu tiên, luật nào kích hoạt thì dừng bơm ngay và gửi bản tin lỗi. Ba luật đầu liên quan tới tín hiệu và tự xóa khi tín hiệu trở lại. Các luật còn lại khóa bơm cho tới khi người vận hành xóa, trừ cảnh báo rò rỉ.

Hai dòng được tô màu. OVERFLOW có ba nguồn kích hoạt độc lập: phao cơ, khoảng cách thô, hoặc mức đã lọc. NO_PROGRESS là dạng vật lý của luật ví dụ trong đề, "bơm chạy mà gần như không có dòng chảy". Dạng chữ nghĩa của nó là DRY_RUN, dùng tín hiệu tuabin; vì điều khiển không dùng tuabin nên DRY_RUN tắt khi lưu lượng suy từ mức nước, vì một lớp bảo vệ không bao giờ kích hoạt được còn tệ hơn không có. NO_PROGRESS dùng đại lượng nhóm đo được: bơm có dòng điện mà mực nước không lên.

Thời gian chờ cảm biến cố ý dài: đi qua vùng mù mất tới bảy mươi lăm giây và mô hình bắc cầu khoảng đó, nên giới hạn là chín mươi giây khi mô hình còn hợp lệ, bốn mươi lăm giây khi không. Bản đầu tiên dùng bốn giây và lần bơm nào cũng dừng sớm.

## 21. Safety guard and overflow layers · 1:15

Bộ điều khiển là máy trạng thái bảy trạng thái, đúng như Bảng 3.3.1 trong báo cáo. Khi khởi động, lệnh đầu tiên là ngắt rơ-le, và thiết bị ở trạng thái BOOT cho tới khi có một số đo mức đầu tiên được nhận. Nhờ vậy giá trị không mà biến mức nước giữ lúc mới cấp điện không bao giờ bị hiểu là bồn cạn. Sau đó thiết bị chờ ở IDLE.

Ở chế độ tự động, khi mực xuống dưới ba mươi phần trăm, hoặc phao dưới đóng, bơm bật và vào FILLING; trên bảy mươi phần trăm, sau ít nhất ba giây, bơm tắt và về IDLE. Mũi tên phía trên là đường tay: lệnh của người vận hành đưa vào MANUAL_ON, nhưng chỉ khi hàm bảo vệ cho phép. Dải ba mươi tới bảy mươi chính là trễ, cùng thời gian chạy tối thiểu ba giây và nghỉ tối thiểu hai mươi giây để bơm không bật tắt liên tục.

Bất kỳ luật lỗi nào, từ FILLING hay MANUAL_ON, cũng dừng bơm và chuyển sang một trong ba trạng thái lỗi. Vì sao dùng máy trạng thái mà không dùng một câu if? Vì trạng thái lỗi phải giữ lại. Lỗi cảm biến tự xóa sau hai giây rưỡi tín hiệu tốt, là mũi tên nét đứt. Lỗi bơm hay khóa chống tràn phải chờ người vận hành, nếu không thì một con bơm đang hút gió sẽ tự chạy lại ngay khi ống chạm nước.

## 22. Pump health classification · 1:25

Đề yêu cầu ít nhất một luật lỗi; nhóm làm mười một luật, đúng danh sách Bảng 3.4.1 trong báo cáo. Các luật chạy mỗi hai trăm mili giây theo thứ tự ưu tiên, luật nào kích hoạt thì dừng bơm ngay và gửi bản tin lỗi. Ba luật đầu liên quan tới tín hiệu và tự xóa khi tín hiệu trở lại. Các luật còn lại khóa bơm cho tới khi người vận hành xóa, trừ cảnh báo rò rỉ.

Hai dòng được tô màu. OVERFLOW có ba nguồn kích hoạt độc lập: phao cơ, khoảng cách thô, hoặc mức đã lọc. NO_PROGRESS là dạng vật lý của luật ví dụ trong đề, "bơm chạy mà gần như không có dòng chảy". Dạng chữ nghĩa của nó là DRY_RUN, dùng tín hiệu tuabin; vì điều khiển không dùng tuabin nên DRY_RUN tắt khi lưu lượng suy từ mức nước, vì một lớp bảo vệ không bao giờ kích hoạt được còn tệ hơn không có. NO_PROGRESS dùng đại lượng nhóm đo được: bơm có dòng điện mà mực nước không lên.

Thời gian chờ cảm biến cố ý dài: đi qua vùng mù mất tới bảy mươi lăm giây và mô hình bắc cầu khoảng đó, nên giới hạn là chín mươi giây khi mô hình còn hợp lệ, bốn mươi lăm giây khi không. Bản đầu tiên dùng bốn giây và lần bơm nào cũng dừng sớm.

## 23. Rolling baseline and daily volume · 1:25

Đề yêu cầu chế độ tay không được vượt rào an toàn. Nhóm giải quyết bằng cấu trúc chương trình, không trông vào sự cẩn thận của người viết code.

Mọi đường có thể bật bơm, tự động hay bằng tay, đều đi qua một hàm duy nhất, pumpBlockReason. Hàm này trả về rỗng, hoặc lý do từ chối: đang có lỗi, phao trên đóng, nước quá gần cảm biến, vượt ngưỡng tràn, mức không tin được, hoặc chưa hết thời gian nghỉ. Trong lúc bơm tay đang chạy, hàm này được gọi lại mỗi hai trăm mili giây, nên nhấc phao lên là bơm dừng trong một chu kỳ. Muốn kiểm tra an toàn, người duyệt chỉ cần đọc một hàm thay vì cả chương trình.

Bên phải là các lớp chống tràn độc lập:
- Hai lớp đầu dựa vào mức đã lọc, nên hỏng cùng nhau nếu cảm biến hỏng.
- Kiểm tra khoảng cách thô đi vòng qua bộ lọc.
- Luật tiến độ bắt được số đo bị đứng yên.
- Giới hạn thể tích và thời gian vẫn chạy khi mất hết cảm biến mức, dù nói thật là chúng chỉ giới hạn thiệt hại.
- Phao cơ không cần phép đo nào.
- Rơ-le được ngắt ngay lệnh đầu tiên khi khởi động.

Không có một hỏng hóc đơn lẻ nào làm mất toàn bộ khả năng chống tràn.

## 24. 05 · Backend, dashboard, security · 0:10

Đề cho hai lựa chọn phần nâng cao, nhóm làm cả hai. Lựa chọn thứ nhất là phân loại sức khỏe bơm, dựa trên sự nhất quán giữa dòng điện bơm và dòng nước.

- Bơm bật, có dòng điện, nước đang chuyển động: nhãn là **ok**.
- Có dòng điện nhưng sau hai mươi lăm giây nước không chuyển động: nhãn là **no_flow**. Nó chỉ về phía thủy lực: đầu hút khô, ống tắc hoặc tuột ống.
- Rơ-le đóng mà không có dòng điện: nhãn là **no_current**. Nó chỉ về phía điện: đứt dây, hỏng rơ-le hoặc chết động cơ.

Cùng một triệu chứng là bồn không đầy, được tách thành hai nguyên nhân, đưa người kỹ thuật tới hai chỗ khác nhau.

Bằng chứng có nước là mức dâng nhanh hơn một phần trăm centimet mỗi giây. Tốc độ dâng thật khoảng 0,036, nên hai mươi lăm giây bơm cho gần một centimet, cao hẳn so với nhiễu của bộ lọc.

Nhãn này chỉ để chẩn đoán; việc ngắt bơm vẫn do các luật lỗi làm. Nhưng nhãn được chụp lại đúng lúc xảy ra mỗi sự cố và lưu cùng sự cố đó. Trong demo, nhóm sẽ nhấc đầu hút ra khỏi nước để thầy xem nhãn chuyển sang no_flow.

## 25. Backend and data model · 1:05

Lựa chọn nâng cao thứ hai là phát hiện tiêu thụ bất thường bằng đường nền trượt. Phần này chạy trên backend vì cần nhớ dữ liệu nhiều ngày.

Lượng nước dùng trong gia đình phụ thuộc mạnh vào giờ trong ngày. Một ngưỡng cố định sẽ quá nhạy ban đêm và quá lỏng buổi tối. Nhóm chia ngày thành bốn mươi tám khe nửa giờ. Mỗi khe tự học mức dùng bình thường của nó bằng trung bình và phương sai có trọng số mũ. Báo động khi lượng dùng vượt trung bình cộng ba độ lệch chuẩn trong hai khe liên tiếp, để không báo nhầm những lần dùng một lần như rửa xe.

Có hai lớp bảo vệ nhóm học được sau khi vấp. Số liệu lưu lượng nhiễu hồi đầu đã cộng dồn ra hàng nghìn lít ảo, nên khe nào có bộ đếm bị đặt lại hoặc lượng nước vô lý về vật lý thì bỏ qua. Và đường nền được dựng lại từ cơ sở dữ liệu mỗi lần backend khởi động.

Trạng thái nói thật: mỗi khe cần ba ngày dữ liệu mới được phép báo động. Sau bốn ngày thử trên bàn, mỗi ngày một khung giờ khác nhau, chưa khe nào đủ, nên nhóm trình bày được cơ chế nhưng chưa có lần phát hiện thật.

Bên phải là báo cáo thể tích theo ngày từ API, gồm nước bơm vào và nước dùng ra. Truy vấn cộng các bước tăng giữa hai dòng liên tiếp, nên khi bộ đếm bị đặt lại thì không cộng gì, thay vì trừ một khoảng âm rất lớn.

## 26. Dashboard and remote access · 1:20

Phần năm: nền tảng xung quanh thiết bị, gồm lưu trữ, dashboard, bảo mật và hành vi khi mất mạng.

## 27. Security model · 1:40

Mô hình ITU trong bài giảng coi bảo mật là năng lực cắt ngang mọi lớp, nên nhóm phân tích mối đe dọa theo từng lớp. Ở broker, kết nối ẩn danh bị tắt, danh sách quyền theo nguyên tắc quyền tối thiểu: tài khoản thiết bị không được gửi lệnh, nên dù ESP32 bị chiếm thì cũng không dùng nó để điều khiển bơm được. Ở API, mọi endpoint tác động tới thế giới thật đều cần phiên đăng nhập. Mật khẩu nằm trong một file ngoài repo, so sánh thời gian hằng; đúng mật khẩu thì sinh token ngẫu nhiên 256 bit trong cookie HttpOnly, SameSite Strict, Secure; sai năm lần trong năm phút thì khóa địa chỉ đó.

Dòng màu cam là lỗ hổng nhóm nói thẳng: đường MQTT trong lab không mã hóa. Broker nghe cổng 1883, chỉ trong mạng phòng lab, còn đường hầm công khai chỉ mang dashboard qua HTTPS và không bao giờ lộ broker. Nhưng ai bắt được gói Wi-Fi trong lab thì đọc được telemetry và mật khẩu broker. Vì vậy nhóm không nói hệ thống bảo mật đầu cuối bằng TLS; MQTTS cổng 8883 là bước khi triển khai thật, đổi lại ESP32 phải làm bắt tay khóa công khai.

Hai dòng xanh là chỗ thú vị, vì chính thiết kế điều khiển chặn chúng: telemetry giả không gây tràn được vì phao và phép đo khoảng cách thô nằm trên thiết bị; gửi lệnh liên tục không làm hỏng bơm được vì thời gian chạy và nghỉ tối thiểu nằm trong firmware.

## 28. Behaviour during a network outage · 1:35

Khi mạng hỏng thì sao? Có bốn cơ chế. Thứ nhất, vòng điều khiển không bao giờ chờ mạng. Bài giảng cảnh báo không đặt logic kết nối trong vòng lặp chặn. Mở socket TCP tới broker không tới được vẫn chặn trong ngăn xếp mạng, mặc định ba giây, nên nhóm giới hạn còn nửa giây. Bài thử mất mạng còn tìm ra một bẫy thứ hai: ghi vào socket mà đầu kia đã biến mất làm vòng lặp đứng tới mười giây. Giờ firmware kiểm tra, không chờ, xem socket có nhận được dữ liệu không trước mỗi lần gửi, và đóng socket sau ba giây bị từ chối. Thiết bị gửi chu kỳ điều khiển dài nhất trong bản tin đầu tiên sau khi nối lại, nên ai cũng kiểm tra được: 0,88 giây trong bài thử cuối.

Thứ hai, không quên gì: khi mất mạng, telemetry vào bộ đệm vòng 240 mẫu, bốn phút ở 1 Hz, phát lại đúng thứ tự khi có mạng. Thể tích được lưu vào flash mỗi phút. Thứ ba, ai cũng biết: broker gửi last will sau một lần rưỡi keep alive, 21,8 giây trong bài thử, và dashboard chuyển đỏ sau năm giây không có dữ liệu. Thứ tư, thiết bị tự tìm đường về: lùi thời gian thử theo hàm mũ, mỗi lần vào Wi-Fi cách nhau ít nhất mười giây, vì gọi lại mỗi giây thì không bao giờ kịp vào mạng, tối đa ba mạng, và tìm lại broker theo tên mDNS.

## 29. 06 · Experiments and results · 0:10

Mô hình ITU trong bài giảng coi bảo mật là năng lực cắt ngang mọi lớp, nên nhóm phân tích mối đe dọa theo từng lớp. Ở broker, kết nối ẩn danh bị tắt, danh sách quyền theo nguyên tắc quyền tối thiểu: tài khoản thiết bị không được gửi lệnh, nên dù ESP32 bị chiếm thì cũng không dùng nó để điều khiển bơm được. Ở API, mọi endpoint tác động tới thế giới thật đều cần phiên đăng nhập. Mật khẩu nằm trong một file ngoài repo, so sánh thời gian hằng; đúng mật khẩu thì sinh token ngẫu nhiên 256 bit trong cookie HttpOnly, SameSite Strict, Secure; sai năm lần trong năm phút thì khóa địa chỉ đó.

Dòng màu cam là lỗ hổng nhóm nói thẳng: đường MQTT trong lab không mã hóa. Broker nghe cổng 1883, chỉ trong mạng phòng lab, còn đường hầm công khai chỉ mang dashboard qua HTTPS và không bao giờ lộ broker. Nhưng ai bắt được gói Wi-Fi trong lab thì đọc được telemetry và mật khẩu broker. Vì vậy nhóm không nói hệ thống bảo mật đầu cuối bằng TLS; MQTTS cổng 8883 là bước khi triển khai thật, đổi lại ESP32 phải làm bắt tay khóa công khai.

Hai dòng xanh là chỗ thú vị, vì chính thiết kế điều khiển chặn chúng: telemetry giả không gây tràn được vì phao và phép đo khoảng cách thô nằm trên thiết bị; gửi lệnh liên tục không làm hỏng bơm được vì thời gian chạy và nghỉ tối thiểu nằm trong firmware.

## 30. Experiment plan · 1:05

Quy tắc thí nghiệm của nhóm rất đơn giản: phép đo nào cũng phải có chuẩn đối chiếu độc lập với hệ đang đo, và mọi con số phải lấy từ cơ sở dữ liệu hoặc nhật ký bằng một script ai cũng chạy lại được. Các script nằm trong repo: analyze_experiments cho chu kỳ, tần suất đóng cắt và độ trễ; bench cho bài thử mất mạng; chương trình hiệu chuẩn theo bậc; và systest cho bài kiểm thử nghiệm thu tự động.

Có hai buổi được phân tích. Buổi điều khiển là ngày 23/9, một giờ hai mươi bốn phút: 4870 bản tin, 99,5 phần trăm có mức đáng tin, không một lỗi nào. Buổi hiệu chuẩn và nghiệm thu là ngày 5/10, trên firmware cuối và hình học cảm biến cuối: hiệu chuẩn theo thước, mất mạng khi bơm đang chạy, và kiểm thử hệ thống. Một ghi chú thật về E5: luật mất dòng điện chưa được gây lỗi thật; slide E5 sẽ nói nhóm đã đo được gì.

## 31. Level calibration against a ruler · 1:30

Thí nghiệm một, hiệu chuẩn mực nước, làm ngày 5/10 trên hình học cuối. Van xả đóng, bơm chạy từng bậc hai mươi giây; sau mỗi bậc bơm dừng, nước lắng mười hai giây, rồi ghi bốn mươi lần đo thô. Cảm biến đầu vào đếm từng lít đã bơm, nên mực chuẩn sau mỗi bậc là một lần đọc thước, 4,9 cm, cộng thể tích đã bơm chia cho đáy 100 cm vuông. Nói chính xác điều này chứng minh gì: hệ số tuabin được đặt từ chính lượt đầu, nên các điểm thấp kiểm tra độ tuyến tính; lượt thứ hai dùng lại hệ số đó không chỉnh.

Ngoài một dải hẹp, cảm biến đo tốt: mười điểm, sai số trung bình bình phương 0,68 cm, sai số tuyệt đối trung bình 0,53 cm. Trong dải đó, khi mặt nước cách đầu dò 7,9 đến 8,7 cm, cả bốn mươi lần đo đều trả về cùng một giá trị sai, một tiếng dội lạc: 1,0 cm khi thật là 7,2, và một khoảng cách xa hơn cả đáy khi thật là 8,0. Vì nó ổn định nên không thể lọc như nhiễu. Đây là lý do mấy hôm trước bơm cứ dừng quanh sáu mươi phần trăm. Giờ cổng hình học bỏ mọi số xa hơn đáy, cổng mô hình loại phần còn lại, và mô hình bồn bắc cầu qua dải đó, tối đa chín mươi giây.

## 32. Control response and switching · 1:30

Phần sáu: thí nghiệm và kết quả.

## 33. Volume estimation error · 1:25

Thể tích cộng dồn chính xác tới đâu? Với mỗi lần bơm tự động, nhóm so thể tích thiết bị tính với một chuẩn chỉ dựng từ mực nước và hình học bồn: độ dâng nhân diện tích đáy, cộng lượng đã xả trong lúc bơm. Tốc độ xả đo độc lập, từ độ dốc của mực nước khi bơm tắt, chín mươi giây trước và sau lần bơm, lấy trung bình, vì theo định luật Torricelli bồn càng đầy xả càng nhanh.

Qua sáu lần bơm đầy ngày 23/9, sai số trung bình là âm 5,2 phần trăm, từ âm 15 tới dương 8. Quy luật rất rõ: ba sai số âm lớn nhất trùng ba lần xả nhanh nhất. Đó đúng là điều mà giả thiết lưu lượng bơm không đổi dự đoán, vì lưu lượng thật thay đổi theo mực nước trong xô nguồn, còn mô hình giữ 0,36. Ngày 5/10 xác nhận điều đó: xô thấp hơn thì cùng con bơm chỉ cho 0,24 lít mỗi phút, ít hơn một phần ba. Vậy sai số này thuộc về giả thiết lưu lượng không đổi. Cảm biến đầu vào giờ đã chạy và đã hiệu chuẩn; kiểm chứng nó qua các lần bơm đầy rồi chuyển bộ đếm thể tích sang nó là bước tiếp theo.

## 34. Command latency · 1:30

Về độ trễ, con số chính của nhóm là thời gian khứ hồi của lệnh, không phải độ trễ một chiều của telemetry, và lý do là phương pháp. Độ trễ một chiều lấy hiệu của hai đồng hồ, mà đồng bộ thời gian trên vi điều khiển chỉ chính xác tới vài chục mili giây, cùng cỡ với đại lượng đo; có lần ngay sau khi khởi động lại, số một chiều đọc 1100 mili giây trong khi khứ hồi lúc đó là 187. Khứ hồi được máy chủ đóng dấu ở cả hai đầu nên độ lệch đồng hồ tự triệt tiêu.

Biểu đồ cho thấy nó cải thiện ra sao. Firmware đầu có trung vị 226 mili giây. Bước lớn nhất là sóng radio: lõi Arduino của ESP32 bật chế độ ngủ modem mặc định, nên máy thu ngủ giữa các beacon và gói tin phải chờ; tắt nó đi giảm phân vị 95 từ 884 xuống 315, đổi lại dòng radio gần gấp đôi, không sao với thiết bị cắm điện. Ngày 23/9 trung vị là 42 và phân vị 95 là 138 trên 75 lệnh. Trên firmware cuối, trong bài kiểm thử ngày 5/10, bốn mươi lệnh cho trung vị 29 mili giây và phân vị 95 là 48,5, không mất lệnh nào. Các buổi này dùng điểm phát khác nhau, nên nhóm không gán bước cuối cho một nguyên nhân duy nhất.

## 35. Fault detection · 1:45

Phát hiện lỗi. Các dòng này lấy từ nhật ký lỗi, nhật ký bơm và nhật ký lệnh ngày 21 tới 23/9. Rút dây echo thì SENSOR_TIMEOUT báo sau bốn mươi tới bốn mươi mốt giây kể từ bản tin đáng tin cuối cùng, khớp với thiết kế bốn mươi lăm giây của firmware lúc đó khi trừ năm giây trước khi số đo bị đánh dấu không tin. Firmware cuối nâng giới hạn lên chín mươi giây khi mô hình bồn còn hợp lệ, vì đi qua vùng mù mất tới bảy mươi lăm giây; con số này chưa đo lại.

Trong cả mười tám sự kiện chống tràn và không tiến triển khi bơm đang chạy, bản tin bơm tắt đi ra trong cùng chu kỳ hai trăm mili giây với bản tin lỗi. Mọi lần bật tay khi đang có lỗi đều bị từ chối đúng lý do.

Nhóm nói thẳng về dòng màu cam: nhóm chưa rút dây bơm để gây NO_CURRENT, nên hai giây là giá trị thiết kế, không phải số đo. Cái nhóm đo được là tín hiệu mà luật dựa vào: rơ-le đóng thì dòng đọc 471 tới 487 mA ở mọi bản tin, mở thì về 0, lớn hơn ngưỡng nhiễu khoảng mười một lần. Và NO_PROGRESS đã kích hoạt thật hai lần ngày 5/10: van xả ra đúng bằng lượng bơm vào ở mức bốn mươi tám phần trăm, mực nước đứng yên chín mươi giây, và luật dừng bơm với nhãn no_flow. Nhóm sẽ rút dây bơm ngay trong demo.

## 36. Network outage with the pump running · 1:50

Đây là yêu cầu đề coi trọng nhất, thử trên firmware cuối ngày 5/10 với bơm đang chạy đúng lúc cắt mạng. Nhóm chặn toàn bộ lưu lượng của thiết bị ở tường lửa laptop trong 120 giây, còn broker và backend vẫn chạy và nối với nhau. Dải xám là lúc mất mạng. Bồn lúc đó ở chín phần trăm.

Màu cam là những gì ESP32 ghi lại khi mất mạng: nó vẫn bơm, từ chín lên năm mươi mốt phần trăm, không đổi trạng thái bơm lần nào, và các mẫu này được đệm rồi phát lại đúng thứ tự khi có mạng, 125 mẫu. Đoạn xám nét đứt sau đó là vùng mù của cảm biến: số đo bị loại, mô hình bắc cầu. Rồi thiết bị tự dừng bơm ở 70,7 phần trăm.

Mất bốn trên 129 bản tin, và nhóm giải thích chứ không giấu: chúng được ghi vào một socket mà thiết bị vẫn tưởng còn mở. Bộ đệm chỉ bảo vệ những mẫu sinh ra sau khi firmware xác định mất mạng. Firmware đầu chờ keep alive mười lăm giây và mất mười bản tin; firmware cuối đóng socket sau ba giây bị từ chối nên chỉ còn mất bốn. Vòng điều khiển không lần nào đứng quá 0,88 giây, trong khi trước khi sửa là tới mười giây. Broker gửi last will sau 21,8 giây, đúng một lần rưỡi keep alive như đặc tả MQTT cho phép, và thiết bị trực tuyến lại mười giây sau khi có mạng.

## 37. Automated system test, 5/5 pass · 1:05

Trước buổi bảo vệ, nhóm muốn có một bài thử ai cũng chạy lại được mà không cần người thao tác, nên viết systest. Nó chạy trên firmware cuối ngày 5/10 và cả năm bài đều đạt. T1 nghe mười giây telemetry và kiểm tra tần số, khoảng hụt số thứ tự, đủ trường và trạng thái giữ lại. T2 kiểm tra mô hình bảo mật: ai cũng xem được, nhưng lệnh không có phiên bị từ chối với mã 401, sai mật khẩu cũng vậy. T3 gửi bốn mươi lệnh đổi chế độ cách nhau một giây và đo khứ hồi: không mất lệnh nào, trung vị 29,1 mili giây. T4 kiểm tra thiết bị từ chối đúng những gì phải từ chối, kèm đúng lý do: lệnh bơm ở chế độ tự động, bật lại trong hai mươi giây nghỉ, và xóa lỗi khi không có lỗi. T5 cho bơm chạy tay tám giây và đọc dòng: 471 tới 487 mA khi chạy, về 0 sau khi tắt.

## 38. 07 · Lessons and conclusion · 0:10

Về độ trễ, con số chính của nhóm là thời gian khứ hồi của lệnh, không phải độ trễ một chiều của telemetry, và lý do là phương pháp. Độ trễ một chiều lấy hiệu của hai đồng hồ, mà đồng bộ thời gian trên vi điều khiển chỉ chính xác tới vài chục mili giây, cùng cỡ với đại lượng đo; có lần ngay sau khi khởi động lại, số một chiều đọc 1100 mili giây trong khi khứ hồi lúc đó là 187. Khứ hồi được máy chủ đóng dấu ở cả hai đầu nên độ lệch đồng hồ tự triệt tiêu.

Biểu đồ cho thấy nó cải thiện ra sao. Firmware đầu có trung vị 226 mili giây. Bước lớn nhất là sóng radio: lõi Arduino của ESP32 bật chế độ ngủ modem mặc định, nên máy thu ngủ giữa các beacon và gói tin phải chờ; tắt nó đi giảm phân vị 95 từ 884 xuống 315, đổi lại dòng radio gần gấp đôi, không sao với thiết bị cắm điện. Ngày 23/9 trung vị là 42 và phân vị 95 là 138 trên 75 lệnh. Trên firmware cuối, trong bài kiểm thử ngày 5/10, bốn mươi lệnh cho trung vị 29 mili giây và phân vị 95 là 48,5, không mất lệnh nào. Các buổi này dùng điểm phát khác nhau, nên nhóm không gán bước cuối cho một nguyên nhân duy nhất.

## 39. Bugs found on the bench · 2:10

Bốn lỗi dạy nhóm nhiều nhất, cả bốn đều có trong Phụ lục A của báo cáo. Lỗi nào cũng vô hình trong code và được tìm ra bằng cách đếm một thứ gì đó.

Thứ nhất, rơ-le bị đảo: cấu hình cho rằng module kích mức thấp, thật ra kích mức cao, nên mọi lệnh dừng lại làm bơm chạy trong khi telemetry báo tắt. Nhóm chứng minh trạng thái thật bằng độ gợn của dòng động cơ, hai mươi bảy mili vôn khi chạy so với bốn rưỡi khi dừng.

Thứ hai, bơm tự bật sau mỗi lần khởi động lại. Lúc khởi động, firmware đặt thời điểm số đo hợp lệ cuối cùng bằng hiện tại để đếm thời gian chờ từ lúc cấp điện, nhưng cờ hợp lệ lại suy ra từ chính mốc đó, nên trong vài giây mức nước được coi là hợp lệ ở không phần trăm và bộ điều khiển thấy bồn cạn. Nhóm tìm ra vì các lần tự bật ở 38 tới 51 phần trăm đều trùng lúc số thứ tự quay về một. Giờ thiết bị ở BOOT cho tới khi có số đo đầu tiên. Ngày 5/10 firmware cuối khởi động lại khi bồn ở ba mươi ba phần trăm: mức đầu tiên gửi lên là ba mươi hai, không phải không, và bơm vẫn tắt cho tới khi nước xả xuống dưới ba mươi. Nhóm sẽ làm lại trực tiếp.

Thứ ba, một tiếng dội lặp lại không phải là mặt nước: khi hiệu chuẩn, với mặt nước cách cảm biến 7,9 tới 8,7 cm, cả bốn mươi lần đo cho cùng một giá trị sai. Một luật nhóm thêm vào, rằng giá trị lặp lại nhiều lần thì phải là thật, đã nhận nó như bồn cạn và bật bơm mỗi bốn mươi giây. Lặp lại chỉ chứng minh có một vật phản xạ, không chứng minh đúng vật; luật đó đã bị bỏ.

Thứ tư, bản tin bị cắt âm thầm: telemetry dài hơn bộ đệm, thư viện JSON cắt mà không báo lỗi, thư viện MQTT nuốt luôn lỗi phân tích. Mọi thứ trông vẫn sống, mà không có gì được lưu.

## 40. Limitations and next steps · 1:30

Nhóm cho rằng chỉ đúng chỗ còn thiếu cũng có giá trị như khoe phần chạy tốt; đây là Mục 5.7 của báo cáo. Vấn đề lớn nhất là đo lưu lượng, dòng tô màu. Phần cứng đã lắp, cảm biến đầu vào giờ đếm được nước và đã hiệu chuẩn, nhưng mới kiểm chứng trong một lần hiệu chuẩn, nên điều khiển và bộ đếm thể tích vẫn dùng lưu lượng từ mức nước. Bước tiếp theo là kiểm chứng nó qua các lần bơm đầy rồi chuyển bộ đếm thể tích sang, và thay cảm biến đầu ra bằng loại đo được từ 0,05 lít mỗi phút.

Thứ hai, hệ số dòng điện: nhóm đọc 471 tới 487 mA cho một con bơm định mức 100 tới 200, nên hoặc cầu chia hoặc loại cảm biến khác với giả định; một lần đo bằng đồng hồ vạn năng là rõ, và cách dùng dòng điện kiểu có hay không vẫn đúng. Thứ ba, luật mất dòng điện chưa được gây lỗi thật; nhóm sẽ rút dây bơm trong demo. Thứ tư, vùng mù siêu âm: dán mút quanh đầu dò, dùng ống lặng sóng hoặc nâng giá cảm biến sẽ hết tiếng dội lạc. Đường nền trượt cần nhiều ngày mới hội tụ. Và khi triển khai thật: MQTTS cổng 8883 vì broker trong lab không mã hóa, đường hầm có tên cố định, tài khoản riêng cho từng người, và cập nhật firmware qua mạng.

## 41. Conclusion · 1:40

Để kết luận, ba ý. Thứ nhất, nhóm đặt toàn bộ luật điều khiển và mọi luật an toàn trên ESP32, theo nguyên tắc cái gì cần dừng an toàn tức thì thì phải nằm ở biên; nhờ vậy yêu cầu chạy khi mất mạng là tính chất của kiến trúc: trong bài thử cuối, bồn vẫn bơm suốt hai phút mất mạng và tự dừng ở 70,7 phần trăm.

Thứ hai, an toàn nằm trong cấu trúc: mọi đường có thể bật bơm đều đi qua một hàm bảo vệ, và chống tràn có nhiều lớp độc lập, vài lớp không phụ thuộc mức đã lọc.

Thứ ba, nhóm tin số đo hơn datasheet và hơn cả dự đoán của mình: bơm chỉ cho một phần năm lưu lượng định mức và cũng không ổn định, cảm biến lưu lượng đếm nhiễu cho tới khi làm lại tầng công suất, cảm biến mức có một vùng mù mà chỉ hiệu chuẩn theo bậc mới lộ ra, và chính dữ liệu của nhóm đã chỉ ra lỗi bật bơm sau mỗi lần khởi động lại.

Trên bàn thử, mọi lần bơm tự động của buổi điều khiển dừng trong khoảng 70,1 tới 70,8 phần trăm, không báo động nhầm lần nào, mức nước sai lệch 0,68 cm so với thước ngoài vùng mù, và lệnh được xác nhận trong 29 mili giây. Những điểm còn mở được nói rõ: cảm biến lưu lượng mới kiểm chứng một phần, luật mất dòng chưa gây lỗi thật, và đường MQTT trong lab chưa mã hóa. Bây giờ xin mời xem bồn thật.

## 42. Live demonstration · 1:35

Bây giờ là demo, tám bước trên bồn thật. Một: mở van xả, mực nước tụt trên máy chiếu và trên một điện thoại dùng 4G, để chứng minh truy cập từ xa. Hai: khi mực xuống dưới dải, bơm tự bật và tự dừng ở bảy mươi phần trăm. Ba: gửi lệnh tay từ dashboard; màn hình hiện "đang gửi" rồi mới hiện câu trả lời đã xác nhận của thiết bị; bật lại trong thời gian nghỉ thì bị từ chối kèm đếm ngược. Bốn: bật bơm tay rồi nhấc phao trên. Bơm dừng ngay, báo lỗi tràn, và xóa lỗi bị từ chối khi phao còn nhấc.

Năm: nhấc đầu hút của bơm ra khỏi nước, nhãn sức khỏe bơm chuyển sang no_flow. Sáu: tắt điểm phát Wi-Fi khi bơm đang chạy. Dashboard chuyển đỏ, nhưng bơm vẫn tự dừng ở bảy mươi phần trăm; bật lại điểm phát thì dữ liệu đệm lấp kín khoảng trống trên biểu đồ. Bảy: rút một dây bơm, NO_CURRENT báo với nhãn lỗi điện. Tám: bấm nút reset của ESP32 khi bồn trên ba mươi phần trăm, bơm vẫn tắt. Nếu phần cứng có trục trặc, nhóm có sẵn video dự phòng đúng trình tự này.

## 43. Thank you · 0:15

Bốn lỗi dạy nhóm nhiều nhất, cả bốn đều có trong Phụ lục A của báo cáo. Lỗi nào cũng vô hình trong code và được tìm ra bằng cách đếm một thứ gì đó.

Thứ nhất, rơ-le bị đảo: cấu hình cho rằng module kích mức thấp, thật ra kích mức cao, nên mọi lệnh dừng lại làm bơm chạy trong khi telemetry báo tắt. Nhóm chứng minh trạng thái thật bằng độ gợn của dòng động cơ, hai mươi bảy mili vôn khi chạy so với bốn rưỡi khi dừng.

Thứ hai, bơm tự bật sau mỗi lần khởi động lại. Lúc khởi động, firmware đặt thời điểm số đo hợp lệ cuối cùng bằng hiện tại để đếm thời gian chờ từ lúc cấp điện, nhưng cờ hợp lệ lại suy ra từ chính mốc đó, nên trong vài giây mức nước được coi là hợp lệ ở không phần trăm và bộ điều khiển thấy bồn cạn. Nhóm tìm ra vì các lần tự bật ở 38 tới 51 phần trăm đều trùng lúc số thứ tự quay về một. Giờ thiết bị ở BOOT cho tới khi có số đo đầu tiên. Ngày 5/10 firmware cuối khởi động lại khi bồn ở ba mươi ba phần trăm: mức đầu tiên gửi lên là ba mươi hai, không phải không, và bơm vẫn tắt cho tới khi nước xả xuống dưới ba mươi. Nhóm sẽ làm lại trực tiếp.

Thứ ba, một tiếng dội lặp lại không phải là mặt nước: khi hiệu chuẩn, với mặt nước cách cảm biến 7,9 tới 8,7 cm, cả bốn mươi lần đo cho cùng một giá trị sai. Một luật nhóm thêm vào, rằng giá trị lặp lại nhiều lần thì phải là thật, đã nhận nó như bồn cạn và bật bơm mỗi bốn mươi giây. Lặp lại chỉ chứng minh có một vật phản xạ, không chứng minh đúng vật; luật đó đã bị bỏ.

Thứ tư, bản tin bị cắt âm thầm: telemetry dài hơn bộ đệm, thư viện JSON cắt mà không báo lỗi, thư viện MQTT nuốt luôn lỗi phân tích. Mọi thứ trông vẫn sống, mà không có gì được lưu.

## Câu hỏi có thể gặp và cách trả lời ngắn

**Yêu cầu cảm biến lưu lượng có thật sự đạt không?**
Không nói đơn giản là "đạt". Hai cảm biến YF-S401 có lắp, có kéo lên, đọc bằng ngắt và có gửi lên. Tới 22/9 bơm gây khoảng 1500 Hz nhiễu dẫn; sau khi đi lại dây ngày 5/10, cảm biến đầu vào đếm được nước thật (0 Hz khi tắt, 20–30 Hz khi chạy) và đã hiệu chuẩn theo thước, K = 85,5, đang cấp cho mô hình bồn. Nhưng điều khiển và bộ đếm thể tích vẫn dùng lưu lượng suy từ mức nước, vì tuabin mới kiểm chứng trong một lần hiệu chuẩn, và lưu lượng xả dưới ngưỡng 0,3 L/phút của cảm biến. Tức là: đã thực hiện, kiểm chứng một phần, có ghi rõ hạn chế.

**NO_CURRENT đã thử chưa?**
Chưa gây lỗi thật; 2 giây là giá trị thiết kế. Nhóm đã đo tín hiệu mà luật dựa vào: rơ-le đóng 471–487 mA, mở 0 mA, gấp khoảng mười một lần ngưỡng nhiễu. Sẽ rút dây bơm trong demo.

**Hệ thống có bảo mật bằng TLS không?**
Không. Trong lab broker chạy cổng 1883 không mã hóa, có mật khẩu và ACL; đường hầm công khai chỉ mang dashboard qua HTTPS. MQTTS cổng 8883 là bước khi triển khai. Nhóm không nói là bảo mật đầu cuối bằng TLS.

**Có lần bơm tự bật sau khi khởi động lại, đã sửa chưa?**
Rồi. Thiết bị ở BOOT cho tới khi có số đo đầu tiên. Ngày 5/10 nó khởi động lại ở 33 %: mức đầu tiên gửi lên là 32 %, không phải 0, bơm vẫn tắt. Sẽ làm lại trực tiếp.

**Vì sao bài mất mạng vẫn mất 4 bản tin?**
Bộ đệm chỉ giữ các mẫu sinh ra sau khi firmware xác định mất mạng. Firmware cuối đóng socket sau 3 giây bị từ chối nên mất khoảng 3–4 mẫu; firmware đầu chờ keep alive 15 giây và mất 10.

**Vì sao dùng rơ-le mà không dùng MOSFET?**
Bơm chỉ có hai trạng thái và đóng cắt vài lần mỗi giờ, nên tốc độ đóng cắt của MOSFET không mang lại gì. Module rơ-le cấp tải từ nguồn riêng, và có diode 1N4007 chống xung ngược.

**Vì sao SQLite mà không dùng cơ sở dữ liệu chuỗi thời gian?**
Hệ ghi một dòng mỗi giây từ một thiết bị, thấp hơn ba bậc so với mức mà cơ sở dữ liệu quan hệ bắt đầu gặp khó. Nhóm vẫn theo nguyên tắc: chỉ đánh chỉ mục dấu thời gian.

**Vì sao hỏi định kỳ (polling) mà không dùng WebSocket?**
Thiết bị gửi 1 Hz, nên hỏi 1 Hz không phí yêu cầu nào. WebSocket thêm một kênh có trạng thái với các kiểu lỗi riêng. Trên khoảng mười thiết bị hoặc 5 Hz thì nhóm sẽ chuyển.

**Nếu chính ESP32 bị treo thì sao?**
Rơ-le được ngắt ngay lệnh đầu tiên sau mọi lần khởi động lại, nên watchdog reset sẽ dừng bơm. Với hệ thật, bước tiếp theo là nối một phao cắt cứng nối tiếp với bơm.

**Vì sao thời gian chờ cảm biến là 90 giây?**
Đi qua vùng mù siêu âm mất tới 75 giây khi bơm, và mô hình bồn bắc cầu khoảng đó; 4 giây thì lần bơm nào cũng dừng sớm, 45 giây vẫn dừng trong vùng mù. Trong lúc chờ, phao, phép đo khoảng cách thô và giới hạn thời gian chạy vẫn bảo vệ bồn.
