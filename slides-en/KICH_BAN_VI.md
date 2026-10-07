# Kịch bản thuyết trình — Bồn nước thông minh (Project 08)

Bản đầy đủ, khoảng 36 phút nếu nói chậm rõ, sau đó 10–15 phút demo. Lên thuyết trình thì nói theo bản tiếng Anh `SCRIPT_EN.md` (cũng là ghi chú người nói trong slide); bản này để hiểu ý và tập. Chỗ có ngoặc vuông như [số] thì tự điền.

| # | Slide | Người nói | Thời gian | Hết lúc |
|---|---|---|---|---|
| 1 | Smart Water Tank | Bảo | 0:40 | 0:40 |
| 2 | Seven parts, then the live demo | Bảo | 0:30 | 1:10 |
| 3 | 01 · Problem and requirements | Bảo | 0:05 | 1:15 |
| 4 | What a smart tank must do | Bảo | 1:00 | 2:15 |
| 5 | Requirements and where they are met | Bảo | 1:05 | 3:20 |
| 6 | 02 · Architecture and hardware | Bảo | 0:05 | 3:25 |
| 7 | System architecture | Bảo | 1:10 | 4:35 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | Bảo | 1:00 | 5:35 |
| 9 | The bench rig | Bảo | 1:05 | 6:40 |
| 10 | The prototype on the bench | Bảo | 0:45 | 7:25 |
| 11 | Controller board schematic | Bảo | 1:05 | 8:30 |
| 12 | Three signal lessons, each found by measuring | Bảo | 1:10 | 9:40 |
| 13 | 03 · MQTT protocol | Nhân | 0:05 | 9:45 |
| 14 | MQTT topic design | Nhân | 1:10 | 10:55 |
| 15 | Commands with acknowledgement | Nhân | 1:05 | 12:00 |
| 16 | 04 · Control and intelligence | Nhân | 0:05 | 12:05 |
| 17 | Level measurement pipeline | Nhân | 1:20 | 13:25 |
| 18 | Model bridging and flow from level | Nhân | 1:00 | 14:25 |
| 19 | Controller state machine | Nhân | 1:20 | 15:45 |
| 20 | Eleven fault rules | Nhân | 1:00 | 16:45 |
| 21 | Safety guard and overflow layers | Nhân | 1:05 | 17:50 |
| 22 | Pump health classification | Nhân | 1:00 | 18:50 |
| 23 | Rolling baseline and daily volume | Nhân | 1:00 | 19:50 |
| 24 | 05 · Backend, dashboard, security | Ngân | 0:10 | 20:00 |
| 25 | Backend and data model | Ngân | 1:00 | 21:00 |
| 26 | Dashboard and remote access | Ngân | 1:05 | 22:05 |
| 27 | Security model | Ngân | 1:00 | 23:05 |
| 28 | Behaviour during a network outage | Ngân | 1:00 | 24:05 |
| 29 | 06 · Experiments and results | Uyên | 0:05 | 24:10 |
| 30 | Experiment plan | Uyên | 0:50 | 25:00 |
| 31 | Level calibration against a ruler | Uyên | 0:55 | 25:55 |
| 32 | Control response and switching | Uyên | 0:50 | 26:45 |
| 33 | Volume estimation error | Uyên | 1:00 | 27:45 |
| 34 | Command latency | Uyên | 0:55 | 28:40 |
| 35 | Fault detection | Uyên | 1:00 | 29:40 |
| 36 | Network outage with the pump running | Uyên | 1:05 | 30:45 |
| 37 | Automated system test, 5/5 pass | Uyên | 0:50 | 31:35 |
| 38 | 07 · Lessons and conclusion | Dương | 0:05 | 31:40 |
| 39 | Bugs found on the bench | Dương | 1:00 | 32:40 |
| 40 | Limitations and next steps | Dương | 0:55 | 33:35 |
| 41 | Conclusion | Dương | 0:55 | 34:30 |
| 42 | Live demonstration | Dương | 1:15 | 35:45 |
| 43 | Thank you | Dương | 0:05 | 35:50 |

## 1. Smart Water Tank · Bảo · 0:40

Chào thầy và các bạn. Nhóm em là nhóm [số], đề tài Project 08: Bồn nước thông minh. Nhóm đã làm một bồn nước thật cỡ nhỏ, bơm do ESP32 điều khiển. Hệ thống đo mức nước và lưu lượng, tự quyết định khi nào bơm, phát hiện và phân loại lỗi, và ai cũng có thể theo dõi trực tiếp bằng điện thoại hay máy tính. Nếu chỉ nhớ một ý của bài, xin nhớ ý này: mọi quyết định điều khiển và an toàn đều chạy ngay trên ESP32. Vì vậy khi mất mạng, bồn vẫn tự chạy đúng.

## 2. Seven parts, then the live demo · Bảo · 0:30

Bài có bảy phần. Một, bài toán và yêu cầu của đề. Hai, kiến trúc và phần cứng. Ba, giao thức MQTT. Bốn, phần cốt lõi: điều khiển và trí tuệ trên ESP32. Năm, backend, dashboard và bảo mật. Sáu, thí nghiệm và kết quả đo. Bảy, các lỗi đã gặp, hạn chế và kết luận. Sau đó nhóm chuyển sang demo trên bồn thật.

## 3. 01 · Problem and requirements · Bảo · 0:05

Phần một: bài toán và yêu cầu của đề.

## 4. What a smart tank must do · Bảo · 1:00

Phần lớn nhà ở Việt Nam vẫn dùng phao cơ. Phao rẻ và bền, nhưng chỉ trả lời được một câu: bồn đầy chưa? Chủ nhà không biết đã dùng bao nhiêu nước, không phát hiện được rò rỉ chậm, và không biết bơm đang chạy khô cho tới khi nó cháy. Đề yêu cầu sáu việc trên slide. Đo mức và lưu lượng có chuẩn đối chiếu thật. Giữ mức nước trong một dải để bơm không bật tắt liên tục. Bảo vệ bồn không bao giờ tràn, kể cả khi cảm biến hỏng hay người dùng ra lệnh ẩu. Chẩn đoán lỗi, ví dụ bơm chạy mà không đẩy được nước. Giám sát từ xa. Và ô màu tối là quan trọng nhất: vẫn điều khiển được bồn khi mất mạng. Yêu cầu cuối này quyết định toàn bộ kiến trúc của nhóm.

## 5. Requirements and where they are met · Bảo · 1:05

Bảng này lấy từng yêu cầu của đề, nói nhóm đáp ứng thế nào và trạng thái thật; nó giống Bảng 1.3.1 trong báo cáo. Hầu hết các dòng đã xong và nhóm sẽ đưa bằng chứng cho từng dòng. Có hai dòng nhóm nói thẳng. Thứ nhất là cảm biến lưu lượng. Hai cảm biến YF-S401 đều có lắp, có đấu dây, có đọc, và từ ngày 5/10 cảm biến đầu vào đếm được nước thật và đã hiệu chuẩn. Nhưng điều khiển và bộ đếm thể tích vẫn dùng lưu lượng suy từ mực nước, nên nhóm ghi là kiểm chứng một phần chứ không ghi xong. Thứ hai là phần nâng cao: nhóm làm cả hai lựa chọn của đề. Bộ phân loại sức khỏe bơm chạy tốt và sẽ demo; đường nền rò rỉ đã làm, nhưng mỗi khe nửa giờ cần ba ngày dữ liệu mới được báo động.

## 6. 02 · Architecture and hardware · Bảo · 0:05

Phần hai: kiến trúc và phần cứng.

## 7. System architecture · Bảo · 1:10

Đây là toàn bộ hệ thống trên một slide, giống sơ đồ trong báo cáo. Hệ thống gồm bốn khối nối tiếp. Trong khung xanh nét đứt là nút biên: cảm biến đưa dữ liệu vào ESP32, ESP32 chạy vòng điều khiển kín mỗi 200 mili giây và tự đóng cắt rơ-le và bơm. ESP32 gửi dữ liệu, trạng thái bơm, lỗi và last will lên broker Mosquitto trên laptop, đồng thời nhận lệnh ở topic cmd, mỗi lệnh đều được trả lời ở cmd/ack. Backend FastAPI nhận dữ liệu từ broker, lưu vào sáu bảng SQLite và phục vụ dashboard qua REST API. Cuối cùng, đường hầm Cloudflare đưa dashboard ra một địa chỉ HTTPS công khai, ai cũng xem được mà không phải mở cổng vào mạng của nhóm. Xin chú ý ô màu tối: đường duy nhất tới bơm là đi qua ESP32. Máy chủ chỉ được hỏi, không bao giờ tự bật bơm.

## 8. The server never switches the pump. It asks, and the ESP32 decides. · Bảo · 1:00

Đây là quyết định quan trọng nhất của dự án. Bài giảng có một quy tắc: thao tác nào cần dừng an toàn ngay lập tức thì phải tính ở biên, sát phần cứng. Nhóm áp dụng triệt để quy tắc đó. Mọi thứ cần chu kỳ cố định hoặc phải sống sót khi mất mạng đều nằm trên ESP32: bộ lọc mức nước, máy trạng thái, cả mười một luật lỗi, hàm bảo vệ, bộ phân loại sức khỏe bơm và bộ đếm thể tích. Máy chủ chỉ giữ những gì vi điều khiển làm không tốt: lịch sử dài, báo cáo thể tích theo ngày, và đường nền rò rỉ cần nhớ nhiều ngày. Kết quả là hệ thống xuống cấp nhẹ nhàng. Khi mất mạng, người dùng mất phần xem và nút bấm từ xa, nhưng bồn vẫn bơm đúng ngưỡng và mọi luật an toàn vẫn chạy.

## 9. The bench rig · Bảo · 1:05

Bồn của nhóm cố ý làm nhỏ. Đáy 10 nhân 10 cm, nên một centimet nước đúng bằng 0,1 lít, rất dễ kiểm thể tích. Cảm biến đặt cao 15,88 cm so với đáy, dải làm việc 12,4 cm, nên bồn chứa 1,24 lít và mỗi chu kỳ bơm đầy rồi xả chỉ mất vài phút. Con số màu cam là phép đo quan trọng nhất. Datasheet hứa 1,67 lít/phút. Nhóm đo thật bằng cách đóng van xả và bấm giờ mực nước: chỉ 0,36 lít/phút, ít hơn năm lần vì độ cao và đường ống. Ngày 5/10, khi xô nguồn thấp, còn chỉ 0,24. Vì vậy mọi ngưỡng thời gian trong firmware đều dựa trên số đo, không bao giờ dựa trên datasheet. Bảng bên dưới liệt kê từng cảm biến và cách xử lý tín hiệu để vào chân 3,3 V của ESP32.

## 10. The prototype on the bench · Bảo · 0:45

Đây là mô hình thật trên bàn thử. Bồn chính trong suốt có cảm biến siêu âm và phao trên gắn ở nắp. Nước chạy theo vòng kín. Bơm đặt trong xô nguồn, đẩy nước qua cảm biến lưu lượng đầu vào lên bồn chính. Nước ra khỏi bồn qua một van tay, đóng vai việc dùng nước trong nhà, rồi qua cảm biến lưu lượng đầu ra quay về xô. Bo điều khiển có ESP32 đặt trên nắp xô nguồn. Mọi thứ trong phần demo đều diễn ra trên mô hình này.

## 11. Controller board schematic · Bảo · 1:05

Đây là sơ đồ bo điều khiển của nhóm, vẽ bằng KiCad. Nguồn 12 V vào qua cọc vít, hai mạch hạ áp tạo ra hai đường nguồn: 5 V và 3,3 V. Nguồn 5 V cấp cho ESP32, cảm biến siêu âm, hai cảm biến lưu lượng và cảm biến dòng, có tụ 470 µF để hấp thụ dòng khởi động của động cơ. Các cảm biến trên nguồn này đều cho tín hiệu 5 V, mà ESP32 chỉ chịu 3,3 V, nên mỗi tín hiệu đi qua một cầu chia 10k và 20k. Nguồn 3,3 V cấp cho module rơ-le, hai phao và nút reset. Một chi tiết: rơ-le nối GPIO 10, chân này bình thường dùng cho bộ nhớ flash, nên nhóm chuyển flash sang chế độ DIO để giải phóng nó, và đã thử trên bàn là chạy được mà chip không bị reset.

## 12. Three signal lessons, each found by measuring · Bảo · 1:10

Đây là ba bài học về đi dây, bài nào cũng tìm ra nhờ đo chứ không phải đọc code. Thứ nhất, chân echo ban đầu đi qua một bộ chuyển mức tự động. Linh kiện này làm cho bus hai chiều, nên nó làm méo xung echo, mà độ rộng xung chính là phép đo. Nhóm mất khoảng 30 phần trăm số đo; một cầu chia điện trở đơn giản sửa được hoàn toàn. Thứ hai, chân lưu lượng báo có nước chảy trong khi van đang đóng. Tần số đúng 50 Hz, tần số điện lưới: chân bị thả nổi và hoạt động như ăng-ten. Thứ ba, khi bơm chạy, hai dây lưu lượng có khoảng 1500 Hz nhiễu, gấp bốn mươi lần tín hiệu thật. Gắn diode và tách dây đều không đỡ, cho thấy nhiễu đi theo đường nguồn. Chỉ sau khi làm lại tầng công suất ngày 5/10, cảm biến đầu vào mới đếm được nước thật. Giờ nó đã được hiệu chuẩn, nhưng nhóm vẫn nói rõ điều khiển dùng lưu lượng suy từ mực nước.

## 13. 03 · MQTT protocol · Nhân · 0:05

Phần ba: giao thức MQTT.

## 14. MQTT topic design · Nhân · 1:10

Cây topic đi từ chung tới riêng: bồn nước, địa điểm, thiết bị. Với mỗi topic, nhóm chọn QoS bằng một câu hỏi: mất một bản tin thì tốn gì? Telemetry là dòng dữ liệu một tin mỗi giây, mất một mẫu không đổi gì, nên dùng QoS 0. Trạng thái bơm chỉ gửi khi thay đổi, nên dùng QoS 1 và giữ lại trên broker, dashboard mở sau vẫn nhận ngay trạng thái hiện tại. Lỗi, lệnh và xác nhận cũng dùng QoS 1, vì mất một tin là hỏng chức năng thật. Topic trạng thái mang last will: nếu ESP32 biến mất không báo trước, broker tự thông báo nó mất kết nối. Mỗi bản tin còn có số thứ tự, nhờ đó nhóm đếm chính xác số tin bị mất. Cuối cùng, broker có danh sách phân quyền, nên dù ai đó lấy được tài khoản của thiết bị thì cũng không gửi được lệnh bơm.

## 15. Commands with acknowledgement · Nhân · 1:05

Sơ đồ này đi theo một lần bấm nút bật bơm. Dashboard gửi lệnh lên backend kèm phiên đăng nhập của quản trị. Backend gán cho lệnh một mã riêng, gửi lên broker và trả lời "đang chờ". Lúc này nút chưa hề đổi; màn hình chỉ ghi "đang gửi". ESP32 nhận lệnh rồi tự quyết: có đang ở chế độ tay không, hàm bảo vệ có cho phép không, đã hết thời gian nghỉ tối thiểu chưa? Sau đó nó gửi xác nhận kèm kết quả, lý do và trạng thái bơm thật. Dashboard hiện câu trả lời đó thành một câu dễ hiểu, còn biểu tượng bơm chỉ đi theo điều thiết bị báo. Đây là nguyên tắc phản hồi trong bài giảng: màn hình hiện điều máy đã xác nhận, không phải điều người dùng mong muốn. Khi thiết bị từ chối, ví dụ trong 20 giây nghỉ, người dùng thấy lý do và đồng hồ đếm ngược, chứ không phải một nút bấm như bị hỏng.

## 16. 04 · Control and intelligence · Nhân · 0:05

Phần bốn, phần cốt lõi của dự án: điều khiển và trí tuệ trên ESP32.

## 17. Level measurement pipeline · Nhân · 1:20

Đo mực nước nghe đơn giản: mực nước bằng độ cao cảm biến trừ khoảng cách đo được. Nhưng chùm sóng siêu âm rộng hơn bồn, nên một số tiếng dội về từ thành hoặc đáy. Vì vậy nhóm không bao giờ tin một lần đo. Mỗi số đo đi qua năm bước, từ trái sang phải. Giữ cửa sổ mười lăm lần đo gần nhất và bỏ những số xa hơn cả đáy. Nếu các số trong cửa sổ quá phân tán, cảm biến đang nhảy giữa hai bề mặt, nên bỏ cả cửa sổ. Nếu không, lấy phân vị 25, vì tiếng dội trễ chỉ làm khoảng cách dài ra. Sau đó kiểm tra vật lý để loại những thay đổi nhanh hơn mức bơm có thể gây ra. Cuối cùng bộ lọc alpha-beta theo dõi cả mực nước và tốc độ thay đổi, không bị trễ như trung bình trượt. Nhờ đếm số lần loại ở từng bước, nhóm phát hiện một thống kê chọn sai đang vứt gần hết số đo; thay nó nâng tỉ lệ nhận từ 60 lên 93 phần trăm. Song song ở hàng dưới, khoảng cách thô được kiểm tra mỗi lần đo: ba lần gần hơn 4,5 cm là kích hoạt chống tràn, bất kể bộ lọc nói gì.

## 18. Model bridging and flow from level · Nhân · 1:00

Bên cạnh cảm biến, ESP32 chạy một mô hình bồn đơn giản: mực nước thay đổi bằng lưu lượng vào trừ lưu lượng ra, chia cho diện tích đáy. Mô hình luôn được kéo nhẹ về mỗi số đo tốt, nên không bao giờ trôi xa. Việc đầu tiên của nó là bắc cầu khi mất số đo. Có lúc cảm biến không cho số đo đáng tin trong vài chục giây; firmware đầu tiên khi đó dừng mọi lần bơm ở khoảng 50 phần trăm. Giờ máy trạng thái điều khiển theo mô hình trong khoảng đó, nhưng tối đa 90 giây; quá thời gian thì dừng bơm và báo LEVEL_LOST, nên mô hình không bao giờ thay cảm biến lâu. Việc thứ hai là lưu lượng ra trên dashboard. Nhóm chỉ đo lưu lượng ra khi bơm đã tắt hai mươi giây và giữ giá trị đó khi bơm chạy, vì chính van chứ không phải bơm quyết định lượng nước được dùng.

## 19. Controller state machine · Nhân · 1:20

Bộ điều khiển là máy trạng thái bảy trạng thái. Khi khởi động, lệnh đầu tiên là ngắt rơ-le, và thiết bị chờ có số đo thật rồi mới làm gì tiếp. Sau đó nó chờ ở IDLE. Ở chế độ tự động, khi mực nước xuống dưới 30 phần trăm, hoặc phao dưới đóng, nó bật bơm và vào FILLING; trên 70 phần trăm thì tắt và về IDLE. Dải từ 30 tới 70 phần trăm chính là vùng trễ, cùng với thời gian chạy tối thiểu 3 giây và nghỉ tối thiểu 20 giây, giúp bơm không bật tắt liên tục. Đường phía trên là đường tay: lệnh của người vận hành đưa vào MANUAL_ON, nhưng chỉ khi hàm bảo vệ cho phép. Luật lỗi nào kích hoạt cũng dừng bơm và chuyển sang trạng thái lỗi. Vì sao dùng máy trạng thái mà không dùng một câu if? Vì lỗi phải được giữ lại. Lỗi cảm biến tự xóa khi tín hiệu tốt trở lại, nhưng lỗi bơm phải chờ người, nếu không thì bơm đang hút gió sẽ tự chạy lại ngay khi ống chạm nước.

## 20. Eleven fault rules · Nhân · 1:00

Đề yêu cầu ít nhất một luật lỗi; nhóm làm mười một luật. Các luật được xét mỗi 200 mili giây theo thứ tự ưu tiên, luật nào kích hoạt cũng dừng bơm ngay và gửi bản tin lỗi. Ba luật đầu liên quan tới tín hiệu và tự xóa khi tín hiệu trở lại. Các luật còn lại khóa bơm cho tới khi người vận hành xóa, trừ cảnh báo rò rỉ. Ba dòng được tô màu. OVERFLOW có ba nguồn kích hoạt độc lập: phao cơ, khoảng cách thô và mực nước đã lọc. DRY_RUN là luật ví dụ của đề, "bơm chạy mà gần như không có dòng chảy", đọc từ dòng điện bơm: bơm đang đẩy nước ăn khoảng 325 mA, nhưng nhấc khỏi nước chỉ còn khoảng 220 mA vì động cơ quay không tải. Dưới 270 mA liên tục ba giây thì dừng bơm. NO_PROGRESS là lớp kiểm tra thứ hai, độc lập: bơm chạy mà mực nước không lên. Thời gian chờ cảm biến cố ý dài, 90 giây khi mô hình còn hợp lệ, vì ngưỡng ngắn hơn làm dừng cả lần bơm bình thường; trong lúc chờ, phao và phép đo khoảng cách thô vẫn bảo vệ bồn.

## 21. Safety guard and overflow layers · Nhân · 1:05

Đề nói chế độ tay không được vượt qua an toàn. Nhóm giải quyết bằng cấu trúc, không phải bằng sự cẩn thận. Mọi đoạn code có thể bật bơm, tự động hay tay, đều đi qua một hàm duy nhất, hàm này hoặc cho phép, hoặc trả về lý do từ chối: đang có lỗi, phao trên, nước quá gần cảm biến, vượt ngưỡng tràn, mực nước không đáng tin, hoặc chưa hết thời gian nghỉ. Khi bơm tay đang chạy, hàm này được xét lại mỗi chu kỳ, nên nhấc phao là bơm dừng trong 200 mili giây. Muốn kiểm tra an toàn, người xem chỉ cần đọc một hàm thay vì cả chương trình. Bên phải là bảy lớp chống tràn độc lập. Một số lớp dựa vào mực nước đã lọc, nhưng các lớp khác thì không: phép đo khoảng cách thô, giới hạn thời gian và thể tích, phao cơ, và ngắt rơ-le ngay khi khởi động. Không một hỏng hóc đơn lẻ nào tắt được tất cả.

## 22. Pump health classification · Nhân · 1:00

Đề có hai lựa chọn nâng cao và nhóm làm cả hai. Lựa chọn thứ nhất là phân loại lỗi bơm bằng cách so dòng điện với việc nước có chảy hay không, và slide này cho thấy bốn trường hợp. Dòng bình thường và nước chảy là bơm khỏe. Dòng thấp, khoảng 220 mA thay vì khoảng 325, là bơm đang chạy khô: đầu hút ra khỏi nước, động cơ quay không tải, nên nhóm dừng bơm trong khoảng năm giây. Dòng bình thường mà sau 25 giây nước không chảy thì nhãn là no_flow: lỗi thủy lực phía sau bơm, như ống tắc hoặc tuột. Còn không có dòng thì nhãn là no_current: lỗi điện, như đứt dây, hỏng rơ-le hay chết động cơ. Như vậy cùng một triệu chứng bồn không đầy, giờ chỉ đúng chỗ cần sửa. Nhãn được lưu kèm mỗi lỗi, và trong demo nhóm sẽ nhấc bơm khỏi nước để thấy trường hợp chạy khô.

## 23. Rolling baseline and daily volume · Nhân · 1:00

Lựa chọn nâng cao thứ hai là phát hiện mức dùng nước bất thường, và nó chạy trên backend vì cần nhớ nhiều ngày. Lượng nước dùng phụ thuộc mạnh vào giờ trong ngày, nên một ngưỡng cố định sẽ quá nhạy ban đêm và quá lỏng buổi tối. Nhóm chia ngày thành 48 khe nửa giờ, mỗi khe tự học mức dùng bình thường của nó. Hệ thống báo động khi mức dùng vượt trung bình cộng ba độ lệch chuẩn trong hai khe liền nhau, nhờ vậy bỏ qua những lần dùng một lần như rửa xe. Bên phải là báo cáo thể tích theo ngày, nước bơm vào so với nước dùng ra. Nói thật: mỗi khe cần ba ngày dữ liệu mới được báo động, nhóm chưa có đủ, nên chỉ trình bày được cơ chế chứ chưa có lần phát hiện thật.

## 24. 05 · Backend, dashboard, security · Ngân · 0:10

Phần năm: phần nền tảng xung quanh thiết bị, gồm lưu trữ, dashboard, bảo mật, và chuyện gì xảy ra khi mất mạng.

## 25. Backend and data model · Ngân · 1:00

Backend là một tiến trình Python duy nhất. Nó nhận các topic MQTT, ghi mỗi bản tin vào một trong sáu bảng SQLite, và phục vụ REST API cho dashboard. Nhóm cố ý chọn SQLite. Bài giảng giải thích cơ sở dữ liệu quan hệ gặp khó với dữ liệu chuỗi thời gian, nhưng chuyện đó chỉ xảy ra ở mức hàng nghìn lần ghi mỗi giây. Nhóm chỉ ghi một dòng mỗi giây từ một thiết bị, nên dùng cơ sở dữ liệu chuỗi thời gian riêng chỉ thêm việc. Nhóm vẫn theo đúng nguyên tắc thiết kế: chỉ đánh chỉ mục cột thời gian, không đánh chỉ mục số thứ tự. Với API, đọc dữ liệu dùng GET vì không thay đổi gì, cấu hình dùng PUT vì gửi hai lần vẫn ra cùng kết quả, còn lệnh dùng POST vì mỗi lệnh là một lệnh mới. Hai endpoint được tô màu tác động tới thế giới thật nên bắt buộc phải có phiên quản trị.

## 26. Dashboard and remote access · Ngân · 1:05

Đây là dashboard của nhóm, bên trái trên máy tính, bên phải trên điện thoại. Nó chỉ là một trang web: trên điện thoại các khung tự xếp thành một cột, nên một link dùng được ở mọi nơi. Phía trên là hình vẽ của hệ thống thật, nước được vẽ đúng độ cao đo được, các vạch 30 và 70 phần trăm ở đúng vị trí, nên ai nhìn cũng hiểu vì sao bơm vừa bật hay tắt. Bên dưới là các con số, biểu đồ lịch sử và nhật ký. Trang này công khai qua đường hầm Cloudflare, nên điện thoại dùng 4G cũng xem được. Nhưng quyền điều khiển là riêng: nút bấm chỉ hiện sau khi đăng nhập mật khẩu quản trị. Nhóm từng thử chỉ cho điều khiển từ địa chỉ của laptop rồi bỏ, vì qua đường hầm mọi yêu cầu đều trông như đến từ chính laptop, tức là vô tình cho cả Internet điều khiển.

## 27. Security model · Ngân · 1:00

Nhóm phân tích bảo mật theo từng lớp như bài giảng gợi ý. Ở broker, tắt truy cập ẩn danh và có danh sách phân quyền cho từng tài khoản. Ở API, mọi endpoint làm bơm chạy đều cần phiên đăng nhập; mật khẩu lưu ngoài mã nguồn, cookie phiên không cho script đọc, và sai mật khẩu năm lần thì khóa địa chỉ đó. Hai dòng màu xanh đáng chú ý, vì chính thiết kế điều khiển chặn chúng: dữ liệu giả không gây tràn được vì phao hoạt động ngay trên thiết bị, gửi lệnh liên tục không làm hỏng bơm vì thời gian chạy và nghỉ tối thiểu nằm trong firmware. Dòng màu cam là lỗ hổng nhóm nói thẳng: MQTT trong lab chưa được mã hóa. Vì vậy nhóm không nói hệ thống bảo mật đầu cuối; TLS cổng 8883 là bước cần làm khi triển khai thật.

## 28. Behaviour during a network outage · Ngân · 1:00

Khi mất mạng thì sao? Có bốn cơ chế. Thứ nhất, vòng điều khiển không bao giờ chờ mạng. Mở kết nối tới một broker không trả lời bình thường sẽ treo vài giây, nên nhóm giới hạn còn nửa giây, và đóng kết nối chết sau ba giây; thiết bị còn báo thời gian vòng lặp dài nhất để ai cũng kiểm tra được. Thứ hai, không quên gì: khi mất mạng, dữ liệu vào bộ đệm vòng 240 mẫu, khoảng bốn phút, và được phát lại đúng thứ tự khi có mạng. Thứ ba, ai cũng biết: broker gửi last will, và dashboard chuyển đỏ sau năm giây không có dữ liệu. Thứ tư, thiết bị tự tìm đường về: thử lại với khoảng chờ tăng dần, dùng được tối đa ba mạng Wi-Fi, và tự tìm lại laptop theo tên.

## 29. 06 · Experiments and results · Uyên · 0:05

Phần sáu: thí nghiệm và kết quả.

## 30. Experiment plan · Uyên · 0:50

Quy tắc thí nghiệm của nhóm rất đơn giản. Phép đo nào cũng phải có chuẩn đối chiếu độc lập với hệ đang đo, và mọi con số phải lấy từ cơ sở dữ liệu bằng một script ai cũng chạy lại được. Bảng này liệt kê các thí nghiệm, chuẩn đối chiếu của từng cái và kết quả. Nhóm phân tích hai buổi. Buổi điều khiển ngày 23/9 kéo dài 84 phút với gần năm nghìn bản tin và không một lỗi báo nhầm. Buổi hiệu chuẩn và nghiệm thu ngày 5/10 chạy firmware cuối và vị trí cảm biến cuối. Một ghi chú thật: lỗi mất dòng điện chưa được gây ra bằng cách rút dây thật; nhóm sẽ giải thích đã đo gì thay vào đó ở slide E5.

## 31. Level calibration against a ruler · Uyên · 0:55

Thí nghiệm một là hiệu chuẩn mực nước. Đóng van xả, nhóm bơm nước từng bậc 20 giây. Sau mỗi bậc, chờ nước lặng rồi đo 40 lần. Cảm biến đầu vào đếm lượng nước đã bơm, nên mực nước thật sau mỗi bậc bằng một lần đọc thước cộng thể tích đã bơm chia diện tích đáy. Kết quả: ngoài một dải hẹp, cảm biến khớp với chuẩn trong khoảng 0,68 cm. Nhưng trong dải đó, khi mặt nước cách cảm biến khoảng 8 cm, cả 40 lần đo đều trả về cùng một giá trị sai, một tiếng dội lạc. Vì nó ổn định tuyệt đối nên không thể lọc như nhiễu. Điều này giải thích vì sao trước đây bơm hay dừng quanh 60 phần trăm. Giờ firmware loại các số đo này và đi qua dải đó bằng mô hình bồn.

## 32. Control response and switching · Uyên · 0:50

Biểu đồ này là 76 phút vận hành thật trên bàn thử. Dải xanh là lúc bơm chạy; vùng gạch chéo là chế độ tay, dùng để thử nghiệm. Ở chế độ tự động, mọi lần bơm đều dừng trong khoảng 70,1 tới 70,8 phần trăm so với ngưỡng 70. Sau đó mực nước còn lên thêm chút ít, trung bình 1,4 phần trăm, do nước còn trong ống tiếp tục chảy vào sau khi rơ-le ngắt; ngay cả đỉnh cao nhất vẫn cách vạch tràn 12 điểm. Bơm bật khoảng 13 lần mỗi giờ, lần nghỉ ngắn nhất là 32 giây, dài hơn mức tối thiểu 20 giây, nên rơ-le không bao giờ bật tắt liên tục. Và không một luật lỗi nào báo nhầm trong cả buổi.

## 33. Volume estimation error · Uyên · 1:00

Thể tích cộng dồn chính xác tới đâu? Với mỗi lần bơm tự động, nhóm so thể tích thiết bị tính được với một chuẩn chỉ dựa vào bồn: độ dâng mực nước nhân diện tích đáy, cộng lượng nước đã xả ra trong lúc bơm. Tốc độ xả được đo riêng từ độ dốc mực nước khi bơm tắt, trước và sau mỗi lần bơm. Qua sáu lần bơm, sai số trung bình là âm 5,2 phần trăm, từ âm 15 tới dương 8. Quy luật rất rõ: sai số lớn nhất rơi vào lúc xả nhanh nhất. Đó đúng là điều nhóm dự đoán, vì thiết bị giả định bơm luôn cho 0,36 lít/phút, trong khi lưu lượng thật thay đổi theo lượng nước trong xô nguồn. Cảm biến đầu vào đã hiệu chuẩn là cách sửa, và chuyển bộ đếm thể tích sang nó là bước tiếp theo.

## 34. Command latency · Uyên · 0:55

Về độ trễ, con số chính nhóm báo cáo là thời gian khứ hồi của lệnh, và lý do là cách đo. Độ trễ một chiều phải trừ hai đồng hồ, của máy chủ và của ESP32, mà đồng hồ trên vi điều khiển chỉ chính xác tới vài chục mili giây, bằng đúng cỡ đại lượng cần đo. Thời gian khứ hồi được máy chủ đo ở cả hai đầu, nên sai lệch đồng hồ tự triệt tiêu. Biểu đồ cho thấy nó đã cải thiện thế nào. Firmware đầu tiên có trung vị 226 mili giây. Bước lớn nhất là ở radio: mặc định ESP32 cho Wi-Fi ngủ giữa các nhịp beacon, nên bản tin phải chờ. Tắt chế độ đó giảm phân vị 95 từ 884 xuống 315 mili giây. Trên firmware cuối, bốn mươi lệnh được xác nhận với trung vị 29 mili giây và không mất lệnh nào.

## 35. Fault detection · Uyên · 1:00

Bảng này là phần phát hiện lỗi. Khi rút dây echo, lỗi mất cảm biến báo đúng như thiết kế. Trong cả mười tám sự kiện tràn và không tiến triển khi bơm đang chạy, bơm dừng ngay trong cùng chu kỳ 200 mili giây với lỗi. Và mọi lần bật tay khi đang có lỗi đều bị từ chối đúng lý do. Nhóm nói rõ về dòng màu cam: nhóm chưa rút dây bơm thật để gây lỗi NO_CURRENT, nên 2 giây là giá trị thiết kế chứ không phải số đo. Cái nhóm đã đo là tín hiệu mà luật này dựa vào: khi rơ-le đóng, dòng điện là 353 tới 365 mA ở mọi bản tin, và về 0 khi rơ-le mở. Nhóm cũng đã thấy NO_PROGRESS báo thật khi van xả mở to tới mức bơm không đẩy được mực nước lên.

## 36. Network outage with the pump running · Uyên · 1:05

Đây là bài thử đề quan tâm nhất, chạy trên firmware cuối khi bơm đang chạy. Nhóm chặn toàn bộ đường truyền của thiết bị trong 120 giây, trong khi broker và backend vẫn chạy. Dải xám là lúc mất mạng, và bồn đang ở 9 phần trăm khi bắt đầu. Đường màu cam là những gì ESP32 ghi lại khi mất mạng: nó vẫn bơm, từ 9 lên 51 phần trăm, không đổi trạng thái bơm lần nào. Khi có mạng lại, 125 bản tin đó được phát lại đúng thứ tự. Đoạn xám nét đứt là vùng mù của cảm biến, nơi mô hình nắm điều khiển. Sau đó thiết bị tự dừng bơm ở 70,7 phần trăm. Chỉ mất 4 trên 129 bản tin: những tin gửi trong ba giây trước khi thiết bị nhận ra kết nối đã chết. Vòng điều khiển không lần nào dừng quá 0,88 giây, và thiết bị trực tuyến trở lại 10 giây sau khi có mạng.

## 37. Automated system test, 5/5 pass · Uyên · 0:50

Trước buổi bảo vệ, nhóm muốn có một bài thử ai cũng chạy lại được mà không cần người thao tác, nên đã viết bài kiểm thử hệ thống tự động. Nó chạy trên firmware cuối và cả năm bài đều đạt. Bài một nghe mười giây và kiểm tra tần số bản tin, số thứ tự bị hụt và các trường bắt buộc. Bài hai kiểm tra bảo mật: ai cũng xem được, nhưng lệnh không có phiên hoặc sai mật khẩu đều bị từ chối. Bài ba gửi bốn mươi lệnh và đo khứ hồi; không mất lệnh nào. Bài bốn kiểm tra thiết bị từ chối đúng những gì phải từ chối, mỗi lần đúng lý do. Bài năm cho bơm chạy tay tám giây và đọc dòng điện: khoảng 360 mA khi chạy và về 0 sau đó.

## 38. 07 · Lessons and conclusion · Dương · 0:05

Phần bảy: bài học, hạn chế, kết luận và demo.

## 39. Bugs found on the bench · Dương · 1:00

Bốn lỗi dạy nhóm nhiều nhất. Lỗi nào cũng không nhìn thấy trong code và được tìm ra nhờ đếm một thứ gì đó. Thứ nhất, rơ-le bị đảo: cấu hình cho rằng rơ-le kích mức thấp, thật ra kích mức cao, nên mọi lệnh dừng lại làm bơm chạy trong khi màn hình báo tắt. Nhóm chứng minh trạng thái thật bằng độ gợn của dòng động cơ. Thứ hai, bơm tự bật sau mỗi lần khởi động lại, vì trong vài giây mực nước được coi là hợp lệ ở 0 phần trăm, bộ điều khiển tưởng bồn cạn. Nhóm đã sửa, và ngày 5/10 một lần khởi động lại ở 33 phần trăm vẫn giữ bơm tắt. Thứ ba, một tiếng dội sai lặp lại hoàn hảo đã đánh lừa một luật tin vào giá trị lặp lại; luật đó đã bị bỏ. Thứ tư, bản tin bị cắt âm thầm do bộ đệm quá nhỏ, mọi thứ trông vẫn chạy mà không có gì được lưu.

## 40. Limitations and next steps · Dương · 0:55

Nhóm cho rằng nói rõ hạn chế cũng có giá trị như khoe kết quả. Vấn đề còn mở lớn nhất là đo lưu lượng: cảm biến đã lắp và đầu vào đã hiệu chuẩn, nhưng mới kiểm chứng trong một lần, nên điều khiển và bộ đếm thể tích vẫn dùng lưu lượng từ mực nước; bước tiếp theo là kiểm chứng qua nhiều chu kỳ rồi chuyển bộ đếm sang nó. Thứ hai, dòng điện bơm đọc cao hơn định mức, cần một lần đo bằng đồng hồ vạn năng. Thứ ba, lỗi mất dòng điện chưa được gây ra bằng tay; nhóm sẽ làm trong demo. Thứ tư, cảm biến siêu âm có vùng mù, dán mút quanh cảm biến hoặc nâng cao cảm biến sẽ khắc phục. Và khi triển khai thật, MQTT cần TLS, cần địa chỉ công khai cố định, tài khoản riêng cho từng người và cập nhật firmware qua mạng.

## 41. Conclusion · Dương · 0:55

Để kết luận, có ba ý làm nên dự án. Thứ nhất, toàn bộ luật điều khiển và mọi luật an toàn chạy trên ESP32, nên chạy được khi mất mạng là tính chất của kiến trúc chứ không phải tính năng gắn thêm; bài thử mất mạng cho thấy bồn tự bơm và tự dừng khi không có mạng. Thứ hai, an toàn nằm trong cấu trúc: một hàm bảo vệ mà mọi lần bật bơm phải đi qua, và bảy lớp chống tràn độc lập. Thứ ba, nhóm tin số đo hơn datasheet và hơn cả dự đoán của chính mình. Bơm chỉ cho một phần năm lưu lượng định mức, cảm biến lưu lượng đếm nhiễu cho tới khi làm lại tầng công suất, cảm biến mức có vùng mù, và chính dữ liệu của nhóm đã chỉ ra lỗi khi khởi động lại. Tất cả đều nhờ đo mà phát hiện. Bây giờ xin mời thầy và các bạn xem bồn thật.

## 42. Live demonstration · Dương · 1:15

Bây giờ là demo, tám bước trên bồn thật. Một: mở van xả, mực nước tụt trên máy chiếu và trên điện thoại dùng 4G. Hai: dưới 30 phần trăm bơm tự bật và dừng ở 70. Ba: gửi lệnh tay và xem chữ "đang gửi" chuyển thành câu trả lời đã xác nhận của thiết bị. Bốn: khi bơm đang chạy, nhấc phao trên; bơm dừng ngay và không xóa được lỗi khi phao còn nhấc. Năm: nhấc bơm ra khỏi nước; dòng điện tụt từ khoảng 325 xuống khoảng 220 mA, nhãn chuyển sang chạy khô, và bơm bị dừng với lỗi DRY_RUN trong khoảng năm giây. Sáu: tắt Wi-Fi khi đang bơm; dashboard chuyển đỏ, nhưng bơm vẫn tự dừng ở 70 phần trăm, và dữ liệu bị thiếu được lấp lại khi có mạng. Bảy: rút một dây bơm để thấy NO_CURRENT. Tám: khởi động lại ESP32 khi bồn trên 30 phần trăm, bơm vẫn tắt. Nếu phần cứng trục trặc, nhóm có sẵn video dự phòng các bước này.

## 43. Thank you · Dương · 0:05

Cảm ơn thầy và các bạn đã lắng nghe. Nhóm xin được nhận câu hỏi.

## Câu hỏi có thể gặp và cách trả lời ngắn

**Yêu cầu cảm biến lưu lượng có thật sự đạt không?**
Không nói đơn giản là "đạt". Hai cảm biến YF-S401 có lắp, có kéo lên, đọc bằng ngắt và có gửi lên. Tới 22/9 bơm gây khoảng 1500 Hz nhiễu dẫn; sau khi đi lại dây ngày 5/10, cảm biến đầu vào đếm được nước thật (0 Hz khi tắt, 20–30 Hz khi chạy) và đã hiệu chuẩn theo thước, K = 85,5, đang cấp cho mô hình bồn. Nhưng điều khiển và bộ đếm thể tích vẫn dùng lưu lượng suy từ mức nước, vì tuabin mới kiểm chứng trong một lần hiệu chuẩn, và lưu lượng xả dưới ngưỡng 0,3 L/phút của cảm biến. Tức là: đã thực hiện, kiểm chứng một phần, có ghi rõ hạn chế.

**NO_CURRENT đã thử chưa?**
Chưa gây lỗi thật; 2 giây là giá trị thiết kế. Nhóm đã đo tín hiệu mà luật dựa vào: rơ-le đóng 353–365 mA, mở 0 mA, gấp khoảng mười một lần ngưỡng nhiễu. Sẽ rút dây bơm trong demo.

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
