# Kịch bản thuyết trình — Bồn nước thông minh (Project 08)

Bản tiếng Việt của kịch bản, đi theo đúng 39 slide tiếng Anh. Dùng để **hiểu và tập**. Khi lên thuyết trình bằng tiếng Anh thì đọc bản `SCRIPT_EN.md` (cũng chính là phần ghi chú người nói trong từng slide).

- Phần kỹ thuật khoảng **38 phút** nếu nói chậm, rõ (khoảng 145 từ tiếng Anh mỗi phút). Sau đó là **10–15 phút demo**.
- Slide 8 và slide 33 là hai slide nhấn mạnh. Nói chậm lại và dừng một nhịp.
- Slide mở đầu mỗi phần (nền tối, số to) chỉ nói một câu rồi chuyển.
- Chỗ nào có ngoặc vuông như [số nhóm] thì tự điền.

| # | Slide | Thời gian | Hết lúc |
|---|---|---|---|
| 1 | Smart Water Tank | 0:45 | 0:45 |
| 2 | Seven parts, then the live demo | 0:30 | 1:15 |
| 3 | 01 · Problem and requirements | 0:10 | 1:25 |
| 4 | What a smart tank must do | 1:05 | 2:30 |
| 5 | Requirements and where they are met | 0:50 | 3:20 |
| 6 | 02 · Architecture and hardware | 0:10 | 3:30 |
| 7 | System architecture | 1:05 | 4:35 |
| 8 | The server never switches the pump | 1:00 | 5:35 |
| 9 | The bench rig | 1:00 | 6:35 |
| 10 | Three wiring lessons | 1:25 | 8:00 |
| 11 | 03 · MQTT protocol | 0:10 | 8:10 |
| 12 | MQTT topic design | 1:10 | 9:20 |
| 13 | Commands with acknowledgement | 1:30 | 10:50 |
| 14 | 04 · Control and intelligence | 0:10 | 11:00 |
| 15 | Level measurement pipeline | 1:35 | 12:35 |
| 16 | Model bridging and flow from level | 1:30 | 14:05 |
| 17 | Controller state machine | 1:20 | 15:25 |
| 18 | Eleven fault rules | 1:20 | 16:45 |
| 19 | Safety guard and overflow layers | 1:15 | 18:00 |
| 20 | Pump health classification | 1:25 | 19:25 |
| 21 | Rolling baseline and daily volume | 1:25 | 20:50 |
| 22 | 05 · Backend, dashboard, security | 0:10 | 21:00 |
| 23 | Backend and data model | 1:05 | 22:05 |
| 24 | Dashboard and remote access | 1:20 | 23:25 |
| 25 | Security model | 1:20 | 24:45 |
| 26 | Behaviour during a network outage | 1:20 | 26:05 |
| 27 | 06 · Experiments and results | 0:10 | 26:15 |
| 28 | Experiment plan | 0:55 | 27:10 |
| 29 | Control response and switching | 1:30 | 28:40 |
| 30 | Volume estimation error | 1:10 | 29:50 |
| 31 | Command latency | 1:20 | 31:10 |
| 32 | Fault detection and network outage | 1:25 | 32:35 |
| 33 | The result to remember: 70.4 % | 0:25 | 33:00 |
| 34 | 07 · Lessons and conclusion | 0:10 | 33:10 |
| 35 | Bugs found on the bench | 1:20 | 34:30 |
| 36 | Limitations and next steps | 1:20 | 35:50 |
| 37 | Conclusion | 1:10 | 37:00 |
| 38 | Live demonstration | 1:20 | 38:20 |
| 39 | Thank you | 0:15 | 38:35 |

---

## 1. Smart Water Tank · 0:45

Chào thầy và các bạn. Nhóm em là nhóm [số nhóm], đề tài Project 08: Bồn nước thông minh. Nói gọn trong một câu: nhóm em làm một bồn nước thật, cỡ nhỏ. Bơm của bồn do ESP32 điều khiển. ESP32 đo mức nước và lượng nước dùng, phát hiện sự cố, và cho phép theo dõi bồn từ bất cứ đâu qua MQTT và một trang web.

Nếu chỉ nhớ một ý của bài này, xin hãy nhớ ý này: mọi quyết định điều khiển và mọi quyết định an toàn đều nằm trên chính con ESP32. Vì vậy bồn vẫn tự chạy đúng khi mất mạng. Bài trình bày có bảy phần, khoảng bốn mươi phút, sau đó là demo trên bồn thật.

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

## 5. Requirements and where they are met · 0:50

Bảng này đi qua từng yêu cầu của đề: nhóm làm bằng cách nào, và trạng thái thật. Hầu hết đã đạt và có minh chứng. Có hai điểm nhóm xin nói thẳng từ đầu, vì lát nữa sẽ quay lại cả hai với số đo cụ thể.

Thứ nhất, cảm biến lưu lượng có lắp và có đấu dây. Nhưng trong hệ này số liệu của chúng không tin được, nên lưu lượng dùng để điều khiển được suy ra từ mức nước.

Thứ hai, phần nâng cao: đề cho hai lựa chọn và nhóm làm cả hai. Một là đường nền trượt để phát hiện tiêu thụ bất thường. Hai là phân loại lỗi bơm dựa trên dòng điện so với dòng chảy. Phần phân loại chạy tốt và sẽ được demo. Phần đường nền cần vài ngày dữ liệu rồi mới được phép báo động.

## 6. 02 · Architecture and hardware · 0:10

Phần hai: kiến trúc và phần cứng.

## 7. System architecture · 1:05

Đây là toàn bộ hệ thống trên một slide.

- Bên trái, trong khung "edge node", cảm biến cấp số liệu cho firmware ESP32. Firmware chạy vòng điều khiển kín mỗi 200 mili giây và trực tiếp đóng ngắt rơ-le và bơm.
- ESP32 gửi bản tin qua MQTT tới broker Mosquitto trên laptop.
- Backend FastAPI đăng ký nhận bản tin, lưu tất cả vào SQLite và phục vụ dashboard qua REST API.
- Cuối cùng, đường hầm Cloudflare đưa dashboard ra một địa chỉ HTTPS công khai. Ai có link cũng xem được từ bất cứ đâu, mà không cần mở cổng nào vào mạng của nhóm.

Đối chiếu với mô hình năm tầng trong bài giảng: tầng cảm nhận là ESP32 và cảm biến, tầng truyền tải là MQTT qua Wi-Fi, tầng xử lý là FastAPI và SQLite, tầng ứng dụng là dashboard.

Thầy và các bạn để ý: đường duy nhất từ máy chủ tới bơm phải đi qua chính ESP32. Máy chủ chỉ được yêu cầu, không bao giờ được ép.

## 8. The server never switches the pump · 1:00

Đây là quyết định quan trọng nhất của đồ án. Bài giảng có một quy tắc đặt chức năng: thao tác nào cần dừng an toàn ngay lập tức thì phải tính ở biên. Nhóm áp dụng quy tắc này triệt để.

Những gì cần chu kỳ ổn định hoặc phải sống sót khi mất mạng đều nằm trên ESP32: lọc tín hiệu, máy trạng thái, cả mười một luật lỗi, hàm rào an toàn, bộ phân loại sức khỏe bơm, và phép cộng dồn thể tích.

Máy chủ chỉ giữ những việc vi điều khiển làm không tốt: lịch sử dài hạn, báo cáo theo ngày, và đường nền rò rỉ cần nhớ dữ liệu nhiều ngày.

Kết quả là hệ thống xuống cấp có kiểm soát. Mất mạng thì người dùng mất khả năng xem và ra lệnh từ xa. Nhưng bồn vẫn tự bơm đầy đúng ngưỡng và mọi luật an toàn vẫn chạy. Yêu cầu chạy khi mất mạng trở thành hệ quả của kiến trúc, chứ không phải tính năng gắn thêm vào sau.

## 9. The bench rig · 1:00

Bồn của nhóm cố tình làm nhỏ. Đáy 10 nhân 10 cm, nên mỗi centimet mức nước đúng bằng 0,1 lít. Dải làm việc 14 cm, tức 1,4 lít. Một chu kỳ bơm đầy rồi xả cạn chỉ mất khoảng bốn phút, nên một buổi đo xem được hàng chục chu kỳ.

Con số màu cam là số đo quan trọng nhất trên slide này. Datasheet của bơm hứa 1,67 lít mỗi phút. Nhóm đo lưu lượng thật bằng cách đóng van xả rồi bấm giờ theo mức nước: lên 1,2 cm trong 20 giây, tức 0,36 lít mỗi phút. Thấp hơn gần năm lần, do cột áp đẩy nước lên và do sức cản của ống. Mọi ngưỡng thời gian trong firmware đều tính từ con số đo được này, không lấy từ datasheet.

Bảng bên dưới liệt kê các cảm biến và cách xử lý từng tín hiệu để đưa vào chân 3,3 V của ESP32.

## 10. Three wiring lessons · 1:25

Ba bài học đấu dây. Bài nào cũng tìm ra bằng cách đo, không phải bằng cách đọc code.

**Một là chân Echo.** Ban đầu chân Echo của cảm biến siêu âm đi qua mạch chuyển mức tự động TXS0108E. Linh kiện này thiết kế cho bus open-drain. Điện trở kéo lên và mạch tăng tốc sườn của nó làm méo xung Echo, trong khi độ rộng xung chính là phép đo. Kết quả là mất khoảng ba mươi phần trăm tiếng dội. Thay bằng một cầu chia áp hai điện trở thì hết hẳn: không mất lần nào trong 179 lần phát.

**Hai là đường lưu lượng lúc tắt bơm.** Van đã đóng mà đầu vào lưu lượng vẫn báo có nước chảy. Tần số xung đúng 50,00 Hz, tức tần số điện lưới: điện trở kéo lên bên trong quá yếu nên sợi dây thành ăng-ten. Gắn thêm điện trở kéo lên ngoài 4,7 kΩ thì về 0.

**Ba là đường lưu lượng lúc bơm chạy**, và cái này nhóm chưa xử lý kịp. Bơm chạy thì cả hai dây lưu lượng đọc khoảng 1500 Hz, trong khi nước thật ở 0,36 lít mỗi phút chỉ cho khoảng 35 Hz. Gắn diode và kéo dây ra xa đều không thay đổi gì. Vậy nhiễu đi theo đường nguồn và đất chung, không phải bức xạ. Thêm nữa, lưu lượng xả của bồn thấp hơn ngưỡng tối thiểu 0,3 lít mỗi phút của cảm biến.

Vì vậy, lưu lượng dùng cho điều khiển và tính toán được suy ra từ mức nước, và dashboard ghi rõ điều đó.

## 11. 03 · MQTT protocol · 0:10

Phần ba: giao thức truyền thông.

## 12. MQTT topic design · 1:10

Cây chủ đề đi từ rộng tới hẹp: bồn nước, địa điểm, thiết bị. Mức QoS chọn riêng cho từng chủ đề bằng một câu hỏi: mất một bản tin thì thiệt hại gì?

- **Telemetry** là dòng dữ liệu 1 Hz. Mất một mẫu không đổi kết luận nào, nên dùng QoS 0.
- **Trạng thái bơm** chỉ gửi khi đổi, nên dùng QoS 1 và bật retain. Dashboard mở sau vẫn nhận ngay trạng thái hiện tại.
- **Sự cố, lệnh và xác nhận** dùng QoS 1, vì mất một bản tin nào trong số đó đều là lỗi chức năng hoặc lỗi an toàn.
- **Status** mang bản tin di chúc (last will). ESP32 biến mất mà không ngắt kết nối đàng hoàng thì chính broker tự công bố "offline".

Nhóm không dùng QoS 2. Lệnh trùng đã được xử lý bằng mã lệnh duy nhất: thiết bị chỉ gửi lại đúng xác nhận cũ, không làm lại hành động.

Mọi bản tin có số thứ tự và dấu thời gian mili giây, nhờ vậy đếm được chính xác số bản tin mất. Broker có danh sách phân quyền (ACL): kể cả ESP32 bị chiếm quyền cũng không gửi được lệnh bơm.

## 13. Commands with acknowledgement · 1:30

Sơ đồ này theo đúng một lần bấm nút Bật bơm.

1. Dashboard gửi lệnh lên backend kèm cookie phiên quản trị.
2. Backend ghi lệnh với một mã mới, phát lệnh lên chủ đề cmd, rồi trả lời ngay "đang chờ". Lúc này nút bấm chưa đổi gì; màn hình chỉ ghi "đang gửi".
3. ESP32 nhận lệnh rồi tự quyết: có đang ở chế độ tay không, rào an toàn có cho bật không, đã hết thời gian nghỉ tối thiểu chưa.
4. ESP32 gửi lại bản tin xác nhận, gồm kết quả, lý do và trạng thái bơm thật.
5. Dashboard hỏi lại nhật ký lệnh và báo kết quả bằng một câu dễ hiểu. Biểu tượng bơm thì chỉ đi theo telemetry.

Đây là nguyên tắc đồng bộ phản hồi (feedback sync) trong bài giảng.

Đường đi này dạy nhóm hai bài học. Một: có lần người dùng báo chế độ tay không chạy. Thật ra thiết bị đang từ chối đúng, vì lệnh bật đến trong hai mươi giây nghỉ, còn giao diện thì im lặng. Giờ giao diện hiện lý do và đếm ngược. Hai: lệnh bơm có thực hiện nhưng không bao giờ được xác nhận. Thư viện MQTT dùng chung một bộ đệm cho bản tin vào và ra, còn thư viện JSON đọc thẳng trên bộ đệm đó. Vì vậy khi gửi trạng thái bơm, mã lệnh bị ghi đè. Nay firmware chép bản tin ra trước rồi mới phân tích.

## 14. 04 · Control and intelligence · 0:10

Phần bốn, phần lõi của đồ án: điều khiển và chẩn đoán trên ESP32.

## 15. Level measurement pipeline · 1:35

Cảm biến siêu âm đặt cao 17,5 cm so với đáy, nên mức nước bằng 17,5 trừ khoảng cách đo được. Nghe thì đơn giản. Nhưng chùm sóng tỏa khoảng mười lăm độ, tới mặt nước đã rộng hơn lòng bồn 10 cm, nên một phần tiếng dội dội về từ thành bồn hoặc đáy. Vì vậy nhóm không bao giờ tin một lần đo đơn lẻ. Mỗi số đo phải qua năm cửa:

1. **Cửa sổ:** giữ mười lăm lần đo gần nhất, nằm trong hình học của bồn.
2. **Độ trải:** nếu khoảng tứ phân vị (IQR) của cửa sổ lớn hơn 3,5 cm thì cảm biến đang nhảy giữa hai bề mặt, bỏ cả cửa sổ.
3. **Trung vị:** còn lại thì lấy trung vị.
4. **Vật lý:** loại thay đổi nhanh hơn mức bơm có thể gây ra, và loại mức tụt xuống trong lúc đang bơm.
5. **Bộ lọc alpha-beta:** theo dõi cả mức lẫn tốc độ thay đổi, không bị trễ như trung bình trượt.

Mỗi cửa có bộ đếm số lần loại riêng, gửi trong telemetry. Nhờ vậy nhóm phát hiện chín mươi tám phần trăm số lần loại đến từ một cửa. Cửa đó dùng "lớn nhất trừ nhỏ nhất", một thống kê mà chỉ một tiếng dội lạc đã đủ chi phối. Đổi sang khoảng tứ phân vị thì tỉ lệ chấp nhận tăng từ sáu mươi lên chín mươi ba phần trăm.

Riêng cho an toàn, có một phép kiểm tra khoảng cách thô đi vòng qua toàn bộ bộ lọc: ba lần đo liên tiếp gần hơn 4,5 cm là bật chống tràn, bộ lọc nghĩ gì cũng mặc.

## 16. Model bridging and flow from level · 1:30

Nhóm còn chạy song song một mô hình đơn giản của bồn. Tốc độ đổi mức bằng lưu lượng vào trừ lưu lượng ra, chia diện tích đáy. Mô hình được kéo nhẹ về mỗi số đo hợp lệ nên không trôi xa.

**Công dụng thứ nhất là bắc cầu.** Trong lúc bơm, cảm biến siêu âm đôi khi không có số đo hợp lệ nào suốt vài chục giây. Firmware đầu tiên vì thế dừng mọi lần bơm ở khoảng năm mươi phần trăm. Giờ trong khoảng trống đó, máy trạng thái điều khiển theo mô hình, tối đa bốn mươi lăm giây. Quá thời gian đó thì ngắt bơm và báo LEVEL_LOST. Mô hình không bao giờ thay cảm biến lâu dài.

**Công dụng thứ hai là lưu lượng.** Vì cảm biến cánh quạt không dùng được ở đây, lưu lượng vào bằng lưu lượng bơm đã hiệu chuẩn khi bơm chạy. Lưu lượng ra bằng tốc độ hạ mức nhân diện tích.

Có một chi tiết tinh tế. Bản đầu tính lưu lượng ra bằng lưu lượng bơm trừ tốc độ dâng ròng tại từng thời điểm. Thành phần bơm đổi tức thì, còn tốc độ ước lượng thì trễ, nên mỗi lần bơm bật, đường lưu lượng ra trên dashboard nhảy lên như thể người ta dùng nước theo bơm. Về vật lý, van xả quyết định lượng nước ra, không phải bơm. Nên giờ nhóm chỉ đo lưu lượng ra khi bơm đã tắt hai mươi giây, rồi giữ nguyên giá trị đó trong lúc bơm.

## 17. Controller state machine · 1:20

Bộ điều khiển là một máy trạng thái bảy trạng thái.

- Sau BOOT, lệnh đầu tiên là ngắt rơ-le, rồi thiết bị chờ ở IDLE.
- Ở chế độ tự động, khi mức dưới ba mươi phần trăm hoặc phao dưới đóng, bơm bật và chuyển sang FILLING. Trên bảy mươi phần trăm thì ngắt bơm và về IDLE.

Dải từ ba mươi tới bảy mươi chính là khoảng trễ (hysteresis). Cùng với thời gian chạy tối thiểu ba giây và nghỉ tối thiểu hai mươi giây, nó ngăn bơm bật tắt liên tục.

MANUAL_ON chỉ vào được bằng lệnh của người vận hành. Bất kỳ luật lỗi nào, từ FILLING hay MANUAL_ON, đều ngắt bơm ngay và chuyển sang một trong ba trạng thái lỗi.

Tại sao dùng máy trạng thái mà không dùng một câu lệnh if? Vì trạng thái lỗi phải "dính". Lỗi cảm biến tự xóa sau hai giây rưỡi có tín hiệu tốt, là mũi tên nét đứt. Còn chạy khô hay khóa tràn thì phải chờ người vận hành xóa. Nếu không, một con bơm vừa hút phải không khí sẽ chạy lại ngay khi ống chạm nước.

Có một bất biến được kiểm tra mỗi chu kỳ: đã có mã lỗi thì trạng thái phải là trạng thái lỗi.

## 18. Eleven fault rules · 1:20

Đề yêu cầu ít nhất một luật lỗi, nhóm làm mười một luật. Các luật chạy mỗi hai trăm mili giây, theo thứ tự ưu tiên. Luật nào bắn thì ngắt bơm ngay và gửi bản tin sự cố.

- Ba luật đầu liên quan đến tín hiệu, tự xóa khi tín hiệu trở lại.
- Các luật còn lại khóa bơm cho tới khi người vận hành xóa, trừ cảnh báo rò rỉ.

Hai dòng được tô màu. OVERFLOW có ba đường kích hoạt độc lập: phao cơ, khoảng cách thô, hoặc mức đã lọc. NO_PROGRESS là dạng vật lý của luật mẫu trong đề, "bơm chạy mà lưu lượng gần bằng không". Dạng nguyên văn của luật đó là DRY_RUN, dùng tín hiệu cảm biến cánh quạt. Vì nhóm không tin tín hiệu đó ở hệ này, DRY_RUN bị tắt khi lưu lượng suy từ mức nước: một luật canh gác không bao giờ bắn được còn tệ hơn là không có. NO_PROGRESS dùng đại lượng nhóm đo thật: bơm ăn điện mà mức nước không lên.

Thời gian chờ cảm biến bốn mươi lăm giây là cố ý để dài. Trong lúc bơm bình thường, cảm biến có thể mất tín hiệu vài chục giây và mô hình che khoảng đó. Bản đầu dùng bốn giây và dừng mọi lần bơm quá sớm.

## 19. Safety guard and overflow layers · 1:15

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

## 20. Pump health classification · 1:25

Đề cho hai lựa chọn phần nâng cao, nhóm làm cả hai. Lựa chọn thứ nhất là phân loại sức khỏe bơm, dựa trên sự nhất quán giữa dòng điện bơm và dòng nước.

- Bơm bật, có dòng điện, nước đang chuyển động: nhãn là **ok**.
- Có dòng điện nhưng sau hai mươi lăm giây nước không chuyển động: nhãn là **no_flow**. Nó chỉ về phía thủy lực: đầu hút khô, ống tắc hoặc tuột ống.
- Rơ-le đóng mà không có dòng điện: nhãn là **no_current**. Nó chỉ về phía điện: đứt dây, hỏng rơ-le hoặc chết động cơ.

Cùng một triệu chứng là bồn không đầy, được tách thành hai nguyên nhân, đưa người kỹ thuật tới hai chỗ khác nhau.

Bằng chứng có nước là mức dâng nhanh hơn một phần trăm centimet mỗi giây. Tốc độ dâng thật khoảng 0,036, nên hai mươi lăm giây bơm cho gần một centimet, cao hẳn so với nhiễu của bộ lọc.

Nhãn này chỉ để chẩn đoán; việc ngắt bơm vẫn do các luật lỗi làm. Nhưng nhãn được chụp lại đúng lúc xảy ra mỗi sự cố và lưu cùng sự cố đó. Trong demo, nhóm sẽ nhấc đầu hút ra khỏi nước để thầy xem nhãn chuyển sang no_flow.

## 21. Rolling baseline and daily volume · 1:25

Lựa chọn nâng cao thứ hai là phát hiện tiêu thụ bất thường bằng đường nền trượt. Phần này chạy trên backend vì cần nhớ dữ liệu nhiều ngày.

Lượng nước dùng trong gia đình phụ thuộc mạnh vào giờ trong ngày. Một ngưỡng cố định sẽ quá nhạy ban đêm và quá lỏng buổi tối. Nhóm chia ngày thành bốn mươi tám khe nửa giờ. Mỗi khe tự học mức dùng bình thường của nó bằng trung bình và phương sai có trọng số mũ. Báo động khi lượng dùng vượt trung bình cộng ba độ lệch chuẩn trong hai khe liên tiếp, để không báo nhầm những lần dùng một lần như rửa xe.

Có hai lớp bảo vệ nhóm học được sau khi vấp. Số liệu lưu lượng nhiễu hồi đầu đã cộng dồn ra hàng nghìn lít ảo, nên khe nào có bộ đếm bị đặt lại hoặc lượng nước vô lý về vật lý thì bỏ qua. Và đường nền được dựng lại từ cơ sở dữ liệu mỗi lần backend khởi động.

Trạng thái nói thật: mỗi khe cần ba ngày dữ liệu mới được phép báo động. Sau ba ngày thử trên bàn thì chưa khe nào đủ, nên nhóm trình bày được cơ chế nhưng chưa có lần phát hiện thật.

Bên phải là báo cáo thể tích theo ngày từ API, gồm nước bơm vào và nước dùng ra. Truy vấn cộng các bước tăng giữa hai dòng liên tiếp, nên khi bộ đếm bị đặt lại thì không cộng gì, thay vì trừ một khoảng âm rất lớn.

## 22. 05 · Backend, dashboard, security · 0:10

Phần năm: nền tảng xung quanh thiết bị, gồm lưu trữ, dashboard, bảo mật và hành vi khi mất mạng.

## 23. Backend and data model · 1:05

Backend là một tiến trình Python. Một client MQTT paho đăng ký sáu chủ đề và ghi mỗi chủ đề vào một bảng riêng: telemetry, sự kiện bơm, sự cố, khả dụng, lệnh, cộng thêm bảng cấu hình.

Nhóm chọn SQLite có chủ đích. Bài giảng có nói vì sao cơ sở dữ liệu quan hệ gặp khó với chuỗi thời gian tần suất cao. Nhưng chuyện đó xảy ra ở hàng nghìn lần ghi mỗi giây. Hệ này ghi một dòng mỗi giây từ một thiết bị, nên dùng một cơ sở dữ liệu chuỗi thời gian riêng chỉ thêm việc vận hành mà không được lợi gì đo được.

Nhóm vẫn theo nguyên tắc lược đồ của bài giảng: chỉ đánh chỉ mục dấu thời gian. Số thứ tự, vốn đổi ở mọi dòng, cố ý không đánh chỉ mục để tránh bùng nổ chỉ mục.

API REST ở bên phải:
- Đọc dữ liệu dùng GET vì an toàn.
- Cấu hình dùng PUT vì thay cả bộ ngưỡng là thao tác lũy đẳng.
- Lệnh dùng POST vì mỗi lần tạo một bản ghi mới.

Hai endpoint tô màu làm thay đổi thế giới thật, nên bắt buộc phải có phiên quản trị.

## 24. Dashboard and remote access · 1:20

Đây là dashboard: một trang HTML duy nhất, không dùng framework.

Phần trên là sơ đồ quá trình theo kiểu phòng điều khiển: bồn nguồn, bơm, ống và bồn chứa. Mực nước vẽ đúng độ cao đo được trên một thước đo. Các vạch ba mươi, bảy mươi và vạch tràn nằm đúng vị trí thật, nên ai nhìn cũng hiểu vì sao bơm vừa bật hay tắt. Bên dưới là các ô mức nước, lưu lượng, thể tích, dòng điện bơm kèm nhãn sức khỏe; bên cạnh là lịch sử mức nước. Có dòng báo dữ liệu đã cũ bao nhiêu giây.

Ảnh chụp này dùng dữ liệu thật đã ghi trên bàn thí nghiệm, phát lại vào một bản sao riêng của hệ thống, nên cơ sở dữ liệu thật không bị đụng tới.

Dashboard là công khai: đường hầm Cloudflare cho nó một địa chỉ HTTPS, xem được từ bất kỳ điện thoại nào mà không mở cổng vào mạng nhóm. Còn quyền điều khiển là riêng: các nút bị ẩn cho tới khi trình duyệt có phiên quản trị.

Lúc đầu nhóm thử cho phép điều khiển chỉ từ địa chỉ của máy chủ, rồi bỏ cách đó. Qua đường hầm, mọi yêu cầu đều đến từ 127.0.0.1, nên quy tắc đó thực ra trao quyền điều khiển cho cả Internet.

## 25. Security model · 1:20

Mô hình ITU trong bài giảng coi bảo mật là năng lực xuyên suốt mọi tầng, nên nhóm phân tích mối đe dọa theo từng tầng.

- **Ở broker:** tắt truy cập ẩn danh. Danh sách phân quyền theo nguyên tắc quyền tối thiểu: tài khoản thiết bị không được gửi lệnh. Kể cả ESP32 bị chiếm quyền cũng không dùng nó để điều khiển bơm được.
- **Ở API:** mọi endpoint làm thay đổi thế giới thật đều cần phiên đăng nhập. Mật khẩu nằm trong một tệp ngoài kho mã và được so sánh theo thời gian hằng. Mật khẩu đúng sinh ra một mã ngẫu nhiên 256 bit, đặt trong cookie HttpOnly, SameSite Strict, Secure. Sai năm lần trong năm phút thì khóa địa chỉ đó.

Hai dòng màu xanh là hai dòng đáng chú ý, vì chúng được chặn bởi chính thiết kế điều khiển. Telemetry giả không thể giả mức thấp để gây tràn, vì phao và phép đo khoảng cách thô nằm trên thiết bị. Spam lệnh cũng không làm mòn bơm, vì thời gian chạy và nghỉ tối thiểu được ép trong firmware.

Các điểm còn yếu ghi trong báo cáo: broker trong lab chưa có TLS, một mật khẩu quản trị dùng chung, phiên lưu trong bộ nhớ.

## 26. Behaviour during a network outage · 1:20

Mất mạng thì chuyện gì xảy ra? Có bốn cơ chế.

**Một, vòng điều khiển không bao giờ chờ mạng.** Bài giảng cảnh báo đừng đặt logic kết nối trong một vòng lặp chặn. Phần nối lại của nhóm không chặn, nhưng việc mở socket TCP tới một broker không tới được vẫn chặn bên trong ngăn xếp mạng, mặc định ba giây trên ESP32. Nhóm giới hạn còn nửa giây. Thiết bị gửi chu kỳ điều khiển dài nhất trong bản tin đầu tiên sau khi nối lại, nên điều này kiểm chứng được, không chỉ là lời khẳng định.

**Hai, không quên gì.** Khi mất mạng, telemetry vào bộ đệm vòng 240 mẫu, tức bốn phút ở 1 Hz. Khi có mạng lại, bộ đệm được phát lại đúng thứ tự, đánh dấu "replay" để loại khỏi thống kê độ trễ. Thể tích cộng dồn được lưu vào flash mỗi phút.

**Ba, ai cũng biết.** Broker công bố bản tin di chúc khi hết hạn keep-alive. Độc lập với đó, dashboard chuyển đỏ sau năm giây không có dữ liệu.

**Bốn, thiết bị tự tìm đường về.** Nối lại với thời gian chờ tăng dần, thử tới ba mạng Wi-Fi, và tìm lại broker theo tên qua mDNS, vì laptop đổi địa chỉ mỗi khi đổi mạng.

## 27. 06 · Experiments and results · 0:10

Phần sáu: thí nghiệm và kết quả.

## 28. Experiment plan · 0:55

Quy tắc thí nghiệm của nhóm rất đơn giản. Mỗi phép đo cần một chuẩn đối chiếu độc lập với hệ đang đo. Mỗi con số phải lấy từ cơ sở dữ liệu bằng một script mà ai cũng chạy lại được.

Các script nằm trong repo:
- analyze_experiments cho chu kỳ bơm, tần suất đóng cắt và độ trễ.
- bench cho hiệu chuẩn, gây lỗi có đánh dấu thời gian bằng phím Enter, và thử mất mạng.

Bảng liệt kê các thí nghiệm, chuẩn đối chiếu và trạng thái. Buổi chính được phân tích là ngày 23 tháng 9: một giờ hai mươi tư phút chạy trên cấu hình lọc cuối cùng, 4870 bản tin, 99,5 phần trăm có mức nước tin được, và không có một sự cố nào. Hiệu chuẩn mức bằng thước là thí nghiệm nhóm còn chạy lại trên bản lắp cuối trước buổi bảo vệ.

## 29. Control response and switching · 1:30

Đây là mức nước trong bảy mươi sáu phút trên bàn thí nghiệm. Dải xanh là lúc bơm chạy. Phần gạch chéo là chế độ tay, nhóm dùng để thử lệnh và để cố ý xả bồn xuống dưới dải.

Ở chế độ tự động, mọi lần bơm đều dừng trong khoảng 70,1 tới 70,8 phần trăm, so với ngưỡng bảy mươi. Sau đó mức còn lên thêm chút ít, trung bình 1,4 phần trăm và tối đa 1,85, vì nước còn trong ống tiếp tục chảy vào sau khi rơ-le ngắt. Đỉnh cao nhất 72,4 phần trăm vẫn cách vạch tràn màu cam hơn mười hai điểm. Bơm đầy từ đáy dải mất khoảng hai phút rưỡi.

Trong cả buổi, bơm bật 12,9 lần mỗi giờ, đã tính cả lần bật tay. Thời gian nghỉ ngắn nhất giữa hai lần là 32 giây, lớn hơn mức tối thiểu hai mươi giây, nên rơ-le không hề bật tắt dồn dập. Và không có luật lỗi nào báo nhầm.

Có một điều bất ngờ: các lần bơm tự động bắt đầu ở 32 tới 33 phần trăm, không phải 30. Lần nào cũng vậy, phao dưới đóng đúng trong giây đó. Phao gắn hơi cao hơn vạch ba mươi phần trăm, và vì đường nào cũng được phép bật bơm nên phao thắng. Ngưỡng thực tế được quyết định bởi một con vít trên thành bồn.

## 30. Volume estimation error · 1:10

Thể tích cộng dồn chính xác tới đâu? Với mỗi lần bơm tự động, nhóm so lượng nước thiết bị tính được với một giá trị chuẩn chỉ dựng từ mức nước và hình học của bồn. Giá trị chuẩn bằng độ dâng của mức nhân diện tích đáy, cộng lượng nước đã xả ra trong lúc bơm.

Tốc độ xả đo độc lập: lấy độ dốc của mức nước khi bơm tắt, chín mươi giây trước và sau lần bơm, rồi lấy trung bình. Phải lấy trung bình vì theo định luật Torricelli, bồn càng đầy thì xả càng nhanh.

Trên sáu lần bơm trọn vẹn, sai số trung bình là âm 5,2 phần trăm, dao động từ âm 15 tới dương 8. Quy luật của nó cho biết nhiều điều: ba sai số âm lớn nhất trùng đúng ba lần tốc độ xả lớn nhất. Đó đúng là điều mà giả thiết lưu lượng bơm không đổi dự đoán, vì lưu lượng bơm thật thay đổi theo mực nước trong bồn nguồn, còn mô hình thì giữ cố định 0,36. Vậy sai số này thuộc về giả thiết hằng số. Cách chữa là một cảm biến lưu lượng đầu vào dùng được, cũng là việc lớn nhất nhóm còn phải làm.

## 31. Command latency · 1:20

Về độ trễ, con số chính của nhóm là độ trễ khứ hồi của lệnh, không phải độ trễ một chiều của telemetry. Lý do nằm ở phương pháp đo.

Độ trễ một chiều lấy hiệu của hai đồng hồ: đồng hồ máy chủ và đồng hồ ESP32. Đồng bộ giờ qua mạng trên vi điều khiển chỉ chính xác tới vài chục mili giây, cùng cỡ với thứ đang đo. Có lần, ngay sau khi khởi động lại, độ trễ một chiều đọc ra 1100 mili giây trong khi khứ hồi lúc đó chỉ 187. Khứ hồi thì cả hai đầu đều do máy chủ đóng dấu, nên sai lệch đồng hồ triệt tiêu.

Biểu đồ cho thấy khứ hồi đã cải thiện thế nào:
- Firmware đầu tiên có trung vị 226 mili giây.
- Bước nhảy lớn nằm ở sóng vô tuyến. Lõi Arduino của ESP32 mặc định bật chế độ ngủ modem, nên bộ thu ngủ giữa các beacon và gói tin phải chờ. Tắt chế độ này làm phân vị 95 giảm từ 884 xuống 315 mili giây. Cái giá là dòng tiêu thụ của radio tăng khoảng gấp đôi, không đáng kể với thiết bị cắm điện lưới.
- Buổi cuối đạt trung vị 42 mili giây và phân vị 95 là 138, trên 75 lệnh.

Buổi đó cũng dùng một điểm phát khác, nên nhóm không gán bước cải thiện cuối cho một nguyên nhân duy nhất.

## 32. Fault detection and network outage · 1:25

Thử mất mạng chỉ cô lập riêng thiết bị: chặn lưu lượng của nó sáu mươi giây, trong khi broker và backend vẫn nối với nhau, vì đó đúng là sự cố mà đề mô tả. Kết quả: ba mươi hai bản tin được đệm và phát lại, và bơm không tự bật.

Có mười bản tin bị mất, và nhóm muốn giải thích thay vì giấu. Chúng được ghi vào một socket mà thiết bị vẫn tưởng còn mở. Kết nối TCP bị cắt lặng lẽ chỉ được phát hiện khi hết thời gian keep-alive mười lăm giây. Vì vậy khoảng mất còn lại bị giới hạn bởi keep-alive, không phải bởi kích thước bộ đệm.

Nhóm cũng học được rằng tắt broker không phải phép thử tương đương: thiết bị nối lại và phát lại trước khi backend kịp đăng ký lại, nên phần phát lại bị mất.

Về gây lỗi: lỗi mất cảm biến bắn sau bốn mươi tới bốn mươi mốt giây tính từ bản tin cuối có mức tin được. Con số này khớp thiết kế: mức bị đánh dấu không tin được năm giây sau lần đo tốt cuối, và bộ đếm bốn mươi lăm giây cũng tính từ chính lần đo đó. Các phép gây lỗi còn lại là phao, đầu hút khô và dây bơm. Nhóm chạy chúng bằng công cụ bench trên firmware cuối, và thầy sẽ thấy hai phép trong số đó ngay trong demo.

## 33. The result to remember: 70.4 % · 0:25

Nếu chỉ nhớ một con số trong phần kết quả, xin hãy nhớ con số này. Trên mọi lần bơm tự động của buổi được phân tích, bơm dừng trung bình ở 70,4 phần trăm so với ngưỡng bảy mươi, và không luật nào trong mười một luật báo nhầm. Đó là cái mà phép đo đã lọc, dải trễ và ràng buộc thời gian mang lại trên một cảm biến thật đầy nhiễu.

## 34. 07 · Lessons and conclusion · 0:10

Phần bảy: bài học, hạn chế, kết luận, rồi tới demo.

## 35. Bugs found on the bench · 1:20

Bốn lỗi dạy nhóm nhiều nhất. Lỗi nào cũng không nhìn thấy trong code, và đều được tìm ra bằng cách đếm một thứ gì đó.

**Một: rơ-le đảo cực.** Cấu hình cho rằng module kích mức thấp, thực tế nó kích mức cao. Vậy là mọi lệnh tắt lại bật bơm, trong khi telemetry báo "tắt". Nhóm chứng minh trạng thái thật bằng độ gợn của dòng động cơ: 27 milivolt khi chạy, 4,5 khi dừng.

**Hai, lỗi mới nhất: bơm tự bật sau mỗi lần khởi động lại.** Lúc khởi động, firmware đặt mốc "lần đo hợp lệ cuối" bằng thời điểm hiện tại để đếm thời gian chờ cảm biến từ lúc bật máy. Nhưng cờ hợp lệ lại suy ra từ chính mốc đó. Vì vậy trong vài giây đầu, mức nước được coi là "hợp lệ" ở 0 phần trăm, và bộ điều khiển tưởng bồn cạn. Nhóm tìm ra vì các lần bật tự động ở 38 tới 51 phần trăm đều trùng đúng lúc số thứ tự bản tin quay về 1.

**Ba: thống kê mong manh trong bộ lọc mức**, đã nói ở phần trước.

**Bốn: cắt cụt âm thầm.** Bản tin telemetry dài hơn bộ đệm. Thư viện JSON cắt mà không báo lỗi, còn thư viện MQTT nuốt luôn ngoại lệ khi phân tích. Mọi thứ trông vẫn sống, nhưng không có gì được lưu. Giờ cả hai đầu đều báo lỗi rõ ràng.

## 36. Limitations and next steps · 1:20

Nhóm nghĩ rằng chỉ đúng chỗ còn thiếu cũng có giá trị như khoe chỗ đã làm được.

- **Đo lưu lượng (tô màu)** là việc lớn nhất còn mở. Nguyên nhân đã rõ và cách chữa rẻ. Một xung thật ở lưu lượng của nhóm dài khoảng mười bốn mili giây, còn nhiễu dẫn ngắn hơn nhiều, nên một bộ lọc độ rộng xung tối thiểu sẽ tách được hai thứ. Cấp nguồn riêng cho cảm biến, không chung đường về với động cơ, sẽ loại bỏ tận gốc. Một cảm biến đo được từ 0,05 lít mỗi phút cũng sẽ đo được lưu lượng xả.
- **Hệ số dòng điện:** nhóm đọc được 360 tới 425 mili ampe cho một bơm định mức 100 tới 200. Hoặc mạch chia áp, hoặc loại cảm biến khác với giả định. Một lần đo bằng đồng hồ vạn năng là rõ. Còn việc dùng dòng điện theo kiểu có hoặc không thì không bị ảnh hưởng.
- **Chùm sóng siêu âm rộng hơn bồn.**
- **Cảm biến đôi khi bị treo** cho tới khi bị ngắt nguồn. Cấp nguồn cho nó qua một chân GPIO sẽ cho firmware tự làm việc đó.
- **Đường nền** cần chạy vài ngày.
- **Để triển khai thật:** đường hầm có tên cố định thay vì link đổi mỗi lần khởi động, tài khoản riêng cho từng người, TLS trên broker, và cập nhật firmware qua mạng.

## 37. Conclusion · 1:10

Để kết luận, có ba ý.

**Thứ nhất**, nhóm đặt toàn bộ luật điều khiển và mọi luật an toàn trên ESP32, theo quy tắc thao tác cần dừng an toàn ngay phải nằm ở biên. Nhờ vậy, yêu cầu chạy khi mất mạng là thuộc tính của kiến trúc chứ không phải tính năng gắn thêm.

**Thứ hai**, an toàn nằm ở cấu trúc: mọi đường bật bơm đi qua một hàm rào duy nhất, và chống tràn có nhiều lớp độc lập, trong đó vài lớp không phụ thuộc gì vào mức đã lọc.

**Thứ ba**, nhóm tin số đo hơn datasheet và hơn cả dự đoán của chính mình. Bơm chỉ cho một phần năm lưu lượng định mức. Cảm biến lưu lượng đếm nhiễu thay vì đếm nước. Và việc phân tích chính dữ liệu của nhóm đã tìm ra lỗi làm bơm bật sau mỗi lần khởi động lại.

Trên bàn thí nghiệm, bộ điều khiển dừng mọi lần bơm tự động trong khoảng 70,1 tới 70,8 phần trăm, không báo nhầm lần nào, ước lượng thể tích với sai số trung bình âm năm phần trăm, và xác nhận lệnh trong 42 mili giây. Bây giờ mời thầy và các bạn xem bồn thật.

## 38. Live demonstration · 1:20

Demo có sáu bước trên bồn thật.

1. Mở van xả. Mức nước giảm cả trên máy chiếu lẫn trên một điện thoại dùng 4G, để chứng minh xem từ xa được.
2. Khi mức xuống dưới dải, bơm tự bật, rồi tự dừng ở bảy mươi phần trăm.
3. Gửi một lệnh tay từ dashboard. Xin thầy để ý màn hình ghi "đang gửi" rồi mới hiện câu trả lời đã được thiết bị xác nhận.
4. Bật bơm bằng tay rồi nhấc phao trên lên. Bơm dừng ngay, báo lỗi tràn, và lệnh xóa lỗi bị từ chối khi phao vẫn đang nhấc.
5. Nhấc đầu hút bơm ra khỏi nước và xem nhãn sức khỏe bơm chuyển sang no_flow.
6. Tắt điểm phát Wi-Fi khi bơm đang chạy. Dashboard chuyển đỏ, nhưng bơm vẫn tự dừng ở bảy mươi phần trăm. Bật điểm phát lại thì dữ liệu đã đệm lấp kín khoảng trống trên biểu đồ.

Nếu phần cứng gặp trục trặc, nhóm có sẵn video dự phòng quay đúng trình tự này.

## 39. Thank you · 0:15

Cảm ơn thầy và các bạn đã lắng nghe. Nhóm sẵn sàng trả lời câu hỏi. Toàn bộ mã nguồn, báo cáo, tài liệu hướng dẫn đọc code và quy trình kiểm thử đều nằm trong repo trên màn hình.

---

## Câu hỏi có thể gặp và cách trả lời ngắn

**Vì sao dùng rơ-le mà không dùng MOSFET?**
Bơm chỉ có hai trạng thái và đóng cắt vài lần mỗi giờ, nên tốc độ đóng cắt của MOSFET không mang lại gì. Module rơ-le cấp tải từ nguồn riêng, và có diode 1N4007 chống xung ngược.

**Vì sao SQLite mà không dùng cơ sở dữ liệu chuỗi thời gian?**
Hệ ghi một dòng mỗi giây từ một thiết bị, thấp hơn ba bậc so với mức mà cơ sở dữ liệu quan hệ bắt đầu gặp khó. Nhóm vẫn theo nguyên tắc: chỉ đánh chỉ mục dấu thời gian.

**Vì sao hỏi định kỳ (polling) mà không dùng WebSocket?**
Thiết bị gửi 1 Hz, nên hỏi 1 Hz không phí yêu cầu nào. WebSocket thêm một kênh có trạng thái với các kiểu lỗi riêng. Trên khoảng mười thiết bị hoặc 5 Hz thì nhóm sẽ chuyển.

**Đề bắt buộc cảm biến lưu lượng, sao lại suy từ mức nước?**
Hai cảm biến YF-S401 có lắp và có đọc. Nhưng nhóm đo được khoảng 1500 Hz nhiễu dẫn khi bơm chạy, so với 35 Hz tín hiệu thật, và lưu lượng xả thấp hơn ngưỡng tối thiểu 0,3 lít mỗi phút. Nhóm chọn không điều khiển dựa trên số đã biết là sai, và ghi rõ trong báo cáo.

**Nếu chính ESP32 bị treo thì sao?**
Rơ-le được ngắt ngay lệnh đầu tiên sau mọi lần khởi động lại, nên watchdog reset sẽ dừng bơm. Với hệ thật, bước tiếp theo là nối một phao cắt cứng nối tiếp với bơm.

**Làm sao biết thử mất mạng chứng minh được điều khiển tại chỗ?**
Các mẫu phát lại mang trạng thái bơm và số thứ tự, nên việc bơm dừng ở 70 % hiện ra ngay trong lịch sử phát lại. Bản tin đầu tiên sau khi nối lại còn báo chu kỳ điều khiển dài nhất trong lúc mất mạng.

**Vì sao thời gian chờ cảm biến là 45 giây, không ngắn hơn?**
Cảm biến siêu âm mất tín hiệu vài chục giây trong lúc bơm bình thường. Với bốn giây thì lần bơm nào cũng dừng sớm. Trong khoảng đó, phao, phép đo khoảng cách thô và giới hạn thời gian chạy vẫn bảo vệ bồn.
