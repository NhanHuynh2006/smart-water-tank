# Kịch bản thuyết trình — Bồn nước thông minh (Project 08)

Mỗi slide: ý chính và lý do, tổng khoảng 18 phút, sau đó demo. Lên thuyết trình thì nói theo bản tiếng Anh `SCRIPT_EN.md` (cũng là ghi chú người nói trong slide); bản này để hiểu và tập.

| # | Slide | Người nói | Thời gian | Hết lúc |
|---|---|---|---|---|
| 1 | Smart Water Tank | Bảo | 0:30 | 0:30 |
| 2 | Seven parts, then the live demo | Bảo | 0:15 | 0:45 |
| 3 | 01 · Problem and requirements | Bảo | 0:05 | 0:50 |
| 4 | What a smart tank must do | Bảo | 0:25 | 1:15 |
| 5 | Requirements and where they are met | Bảo | 0:30 | 1:45 |
| 6 | 02 · Architecture and hardware | Bảo | 0:05 | 1:50 |
| 7 | System architecture | Bảo | 0:35 | 2:25 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | Bảo | 0:35 | 3:00 |
| 9 | The bench rig | Bảo | 0:30 | 3:30 |
| 10 | The prototype on the bench | Bảo | 0:25 | 3:55 |
| 11 | Controller board schematic | Bảo | 0:35 | 4:30 |
| 12 | Three signal lessons, each found by measuring | Bảo | 0:35 | 5:05 |
| 13 | 03 · MQTT protocol | Nhân | 0:05 | 5:10 |
| 14 | MQTT topic design | Nhân | 0:30 | 5:40 |
| 15 | Commands with acknowledgement | Nhân | 0:30 | 6:10 |
| 16 | 04 · Control and intelligence | Nhân | 0:05 | 6:15 |
| 17 | Level measurement pipeline | Nhân | 0:35 | 6:50 |
| 18 | Model bridging and flow from level | Nhân | 0:35 | 7:25 |
| 19 | Controller state machine | Nhân | 0:35 | 8:00 |
| 20 | Eleven fault rules | Nhân | 0:30 | 8:30 |
| 21 | Safety guard and overflow layers | Nhân | 0:30 | 9:00 |
| 22 | Pump health classification | Nhân | 0:30 | 9:30 |
| 23 | Rolling baseline and daily volume | Nhân | 0:30 | 10:00 |
| 24 | 05 · Backend, dashboard, security | Ngân | 0:05 | 10:05 |
| 25 | Backend and data model | Ngân | 0:25 | 10:30 |
| 26 | Dashboard and remote access | Ngân | 0:30 | 11:00 |
| 27 | Security model | Ngân | 0:35 | 11:35 |
| 28 | Behaviour during a network outage | Ngân | 0:30 | 12:05 |
| 29 | 06 · Experiments and results | Uyên | 0:05 | 12:10 |
| 30 | Experiment plan | Uyên | 0:20 | 12:30 |
| 31 | Level calibration against a ruler | Uyên | 0:35 | 13:05 |
| 32 | Control response and switching | Uyên | 0:20 | 13:25 |
| 33 | Volume estimation error | Uyên | 0:30 | 13:55 |
| 34 | Command latency | Uyên | 0:25 | 14:20 |
| 35 | Fault detection | Uyên | 0:30 | 14:50 |
| 36 | Network outage with the pump running | Uyên | 0:30 | 15:20 |
| 37 | Automated system test, 5/5 pass | Uyên | 0:15 | 15:35 |
| 38 | 07 · Lessons and conclusion | Dương | 0:05 | 15:40 |
| 39 | Bugs found on the bench | Dương | 0:30 | 16:10 |
| 40 | Limitations and next steps | Dương | 0:25 | 16:35 |
| 41 | Conclusion | Dương | 0:25 | 17:00 |
| 42 | Live demonstration | Dương | 0:35 | 17:35 |
| 43 | Thank you | Dương | 0:05 | 17:40 |

**1. Smart Water Tank** (Bảo)  
Chào thầy và các bạn. Nhóm em là nhóm [số], đề tài Project 08: Bồn nước thông minh. Nhóm làm một bồn nước thật cỡ nhỏ, bơm do ESP32 điều khiển: đo mức và lưu lượng, phát hiện lỗi, và theo dõi được từ bất cứ đâu. Nếu chỉ nhớ một ý thì là: mọi quyết định điều khiển và an toàn đều chạy trên ESP32, nên mất mạng bồn vẫn tự chạy đúng.

**2. Seven parts, then the live demo** (Bảo)  
Bài có bảy phần: bài toán, kiến trúc và phần cứng, giao thức MQTT, phần điều khiển, nền tảng phía máy chủ, thí nghiệm, và bài học. Sau đó chuyển sang demo trên bồn thật.

**3. 01 · Problem and requirements** (Bảo)  
Phần một: bài toán và yêu cầu.

**4. What a smart tank must do** (Bảo)  
Phần lớn nhà vẫn dùng phao cơ: rẻ nhưng chỉ biết bồn đầy hay chưa. Đề yêu cầu sáu việc: đo mức và lưu lượng có chuẩn đối chiếu, giữ mức không bật tắt liên tục, không bao giờ tràn, chẩn đoán lỗi bơm, giám sát từ xa, và vẫn chạy khi mất mạng. Yêu cầu cuối, ô màu tối, quyết định toàn bộ kiến trúc.

**5. Requirements and where they are met** (Bảo)  
Bảng này là Bảng 1.3.1 trong báo cáo. Gần như mọi yêu cầu đều xong và có chứng minh. Nhóm nói thẳng hai chỗ. Cảm biến lưu lượng có lắp, đầu vào đã hiệu chuẩn, nhưng điều khiển vẫn dùng lưu lượng suy từ mức nước, nên ghi là kiểm chứng một phần. Và đường nền rò rỉ đã làm, nhưng mỗi khe nửa giờ cần ba ngày dữ liệu mới được báo động.

**6. 02 · Architecture and hardware** (Bảo)  
Phần hai: kiến trúc và phần cứng.

**7. System architecture** (Bảo)  
Hệ thống gồm bốn khối nối tiếp. Trong khung xanh, cảm biến đưa dữ liệu vào ESP32, ESP32 khép vòng điều khiển mỗi 200 ms và tự đóng cắt bơm. ESP32 nói chuyện MQTT với broker Mosquitto; FastAPI lưu mọi thứ vào SQLite và phục vụ dashboard, đường hầm Cloudflare đưa ra công khai qua HTTPS. Điểm chính là ô màu tối: đường duy nhất tới bơm là qua ESP32. Máy chủ chỉ được hỏi, không bao giờ tự bật bơm.

**8. The server never switches the pump. It asks, and the ESP32 decides.** (Bảo)  
Đây là quyết định thiết kế trung tâm. Bài giảng nói cái gì cần dừng an toàn tức thì thì phải chạy ở biên, nên nhóm đặt bộ lọc, máy trạng thái, mọi luật lỗi và hàm bảo vệ trên ESP32. Máy chủ chỉ giữ những thứ cần nhớ nhiều ngày: lịch sử, thể tích theo ngày và đường nền rò rỉ. Vì vậy khi mất mạng, ta chỉ mất phần xem và lệnh từ xa; bồn vẫn bơm đúng ngưỡng và mọi luật an toàn vẫn chạy.

**9. The bench rig** (Bảo)  
Bồn cố ý làm nhỏ: đáy 10 nhân 10 cm nên một centimet đúng 0,1 lít, dải làm việc 12,4 cm, tức 1,24 lít. Con số màu cam quan trọng nhất: datasheet hứa 1,67 lít/phút, nhóm đo được 0,36, và chỉ 0,24 khi xô nguồn thấp. Mọi ngưỡng thời gian trong firmware đều lấy theo số đo, không lấy theo datasheet.

**10. The prototype on the bench** (Bảo)  
Đây là mô hình thật. Bồn chính có cảm biến siêu âm và phao trên ở nắp. Nước chạy vòng: từ xô nguồn qua bơm và cảm biến lưu lượng đầu vào vào bồn, rồi ra qua van xả và cảm biến lưu lượng đầu ra. Bo điều khiển đặt trên nắp xô nguồn.

**11. Controller board schematic** (Bảo)  
Đây là bo mạch của nhóm, theo sơ đồ KiCad. Vào 12 V, hai mạch hạ áp tạo nguồn 5 V và 3,3 V. Mọi ngõ ra 5 V của cảm biến, gồm echo, hai cảm biến lưu lượng và cảm biến dòng, đều qua cầu chia 10k và 20k để ESP32 chỉ thấy tối đa 3,3 V. Module rơ-le, hai phao và nút reset dùng nguồn 3,3 V; rơ-le nối GPIO 10, chân này cần flash chạy chế độ DIO.

**12. Three signal lessons, each found by measuring** (Bảo)  
Ba bài học đi dây, đều nhờ đo mà ra chứ không phải đọc code. Bộ chuyển mức làm méo xung echo, mất 30 phần trăm số đo; cầu chia điện trở sửa được. Chân lưu lượng thả nổi bắt đúng 50 Hz điện lưới dù van đóng. Và bơm gây 1500 Hz nhiễu vào dây lưu lượng cho tới khi làm lại tầng công suất ngày 5/10; từ đó cảm biến đầu vào đếm được nước thật và đã hiệu chuẩn, dù điều khiển vẫn dùng lưu lượng suy từ mức nước.

**13. 03 · MQTT protocol** (Nhân)  
Phần ba: giao thức MQTT.

**14. MQTT topic design** (Nhân)  
Mỗi topic làm một việc, và QoS được chọn bằng cách hỏi: mất một tin thì tốn gì. Telemetry 1 Hz dùng QoS 0 vì mất một mẫu không đổi gì; lỗi, lệnh và xác nhận dùng QoS 1. Topic trạng thái mang last will, nên broker tự báo khi thiết bị biến mất. Mỗi bản tin có số thứ tự để đếm mất mát, và ACL làm tài khoản thiết bị không gửi được lệnh.

**15. Commands with acknowledgement** (Nhân)  
Đây là đường đi của một lần bấm nút bơm. Dashboard gửi lệnh có mã riêng, ESP32 kiểm tra hàm bảo vệ rồi trả lời ở cmd/ack kèm kết quả, lý do và trạng thái bơm thật. Nút không tự đổi; màn hình chỉ hiện điều thiết bị đã xác nhận. Nếu thiết bị từ chối, ví dụ trong 20 giây nghỉ, người dùng thấy lý do và đếm ngược.

**16. 04 · Control and intelligence** (Nhân)  
Phần bốn: điều khiển và trí tuệ trên ESP32.

**17. Level measurement pipeline** (Nhân)  
Trong bồn hẹp, một số tiếng dội về từ thành hoặc đáy, nên không bao giờ tin một lần đo. Mỗi số đo qua năm bước: cửa sổ 15 lần đo, kiểm tra độ phân tán, lấy phân vị 25, kiểm tra hợp lý vật lý, và bộ lọc alpha-beta theo dõi mức và tốc độ không bị trễ. Đếm số lần loại cho thấy một thống kê sai đang vứt gần hết số đo; đổi sang khoảng tứ phân vị nâng tỉ lệ nhận từ 60 lên 93 phần trăm. Song song, phép đo khoảng cách thô có thể tự kích hoạt chống tràn.

**18. Model bridging and flow from level** (Nhân)  
Bên cạnh cảm biến, nhóm chạy một mô hình bồn đơn giản: lưu lượng vào trừ ra chia diện tích đáy, luôn được kéo về mỗi số đo tốt. Khi cảm biến mù, mô hình nắm điều khiển tối đa 90 giây, quá thì dừng bơm và báo LEVEL_LOST. Nhờ vậy hết cảnh bơm dừng ở khoảng 50 phần trăm. Mô hình cũng cho lưu lượng ra trên dashboard, chỉ đo khi bơm tắt nên không nhảy khi bơm bật.

**19. Controller state machine** (Nhân)  
Bộ điều khiển có bảy trạng thái. Bơm bật dưới 30 phần trăm và tắt trên 70, có thời gian chạy tối thiểu 3 giây và nghỉ 20 giây nên không bật tắt liên tục. Bật tay chỉ vào MANUAL_ON khi hàm bảo vệ cho phép. Luật lỗi nào kích hoạt cũng dừng bơm và vào trạng thái lỗi, và trạng thái lỗi được giữ lại: lỗi bơm chờ người vận hành, nên bơm chạy khô không tự chạy lại.

**20. Eleven fault rules** (Nhân)  
Đề yêu cầu một luật lỗi; nhóm có mười một luật, xét mỗi 200 ms, luật nào kích hoạt cũng dừng bơm ngay. OVERFLOW có ba nguồn độc lập: phao, khoảng cách thô và mức đã lọc. NO_PROGRESS là luật ví dụ của đề, bơm chạy mà không có dòng chảy, đo bằng việc mực nước không lên. Thời gian chờ cảm biến cố ý dài, 90 giây khi mô hình còn hợp lệ, vì ngưỡng ngắn hơn làm dừng các lần bơm bình thường.

**21. Safety guard and overflow layers** (Nhân)  
Chế độ tay không được vượt an toàn, nên mọi lần bật bơm, tự động hay tay, đều đi qua một hàm bảo vệ duy nhất. Hàm này được xét lại mỗi chu kỳ khi bơm tay đang chạy, nên nhấc phao là bơm dừng trong 200 ms. Chống tràn có bảy lớp độc lập, từ ngưỡng 70 phần trăm tới phao cơ và việc ngắt rơ-le ngay khi khởi động. Không một hỏng hóc đơn lẻ nào tắt được tất cả.

**22. Pump health classification** (Nhân)  
Đây là lựa chọn nâng cao A: phân loại lỗi bơm từ dòng điện và việc nước có chảy. Có dòng và nước chảy là bơm khỏe. Có dòng mà sau 25 giây nước không chảy là lỗi thủy lực: hút khô hoặc tắc ống. Không có dòng là lỗi điện: đứt dây, hỏng rơ-le hoặc động cơ. Cùng một triệu chứng bồn không đầy, giờ chỉ đúng chỗ cần sửa.

**23. Rolling baseline and daily volume** (Nhân)  
Đây là lựa chọn nâng cao B, chạy trên backend vì cần nhớ nhiều ngày. Một ngày chia 48 khe nửa giờ, mỗi khe học mức dùng bình thường, và báo động khi mức dùng vượt trung bình cộng ba độ lệch chuẩn trong hai khe liền. Nói thật là mỗi khe cần ba ngày dữ liệu mới được báo, nhóm chưa đủ, nên chỉ trình bày được cơ chế.

**24. 05 · Backend, dashboard, security** (Ngân)  
Phần năm: backend, dashboard và bảo mật.

**25. Backend and data model** (Ngân)  
Backend là một tiến trình Python: nhận MQTT, ghi sáu bảng SQLite và phục vụ REST API. Chọn SQLite vì một dòng mỗi giây còn rất xa mức cơ sở dữ liệu quan hệ gặp khó. Đọc dùng GET, cấu hình dùng PUT, lệnh dùng POST; mọi thứ tác động tới thế giới thật đều cần phiên quản trị.

**26. Dashboard and remote access** (Ngân)  
Dashboard là một trang web dùng được cả trên máy tính và điện thoại; trên điện thoại các khung tự xếp thành một cột. Ai có link công khai cũng xem được, nhưng nút điều khiển chỉ hiện sau khi đăng nhập mật khẩu quản trị. Nhóm từng thử cho phép điều khiển theo địa chỉ IP rồi bỏ, vì qua đường hầm mọi yêu cầu đều trông như đến từ chính laptop.

**27. Security model** (Ngân)  
Bảo mật xét theo từng lớp. Broker cần mật khẩu và ACL, API cần cookie phiên, sai mật khẩu năm lần thì khóa địa chỉ. Hai kiểu tấn công bị chặn bởi chính thiết kế điều khiển: dữ liệu giả không gây tràn được vì phao nằm trên thiết bị, gửi lệnh liên tục không làm hỏng bơm vì có thời gian tối thiểu. Lỗ hổng còn lại nói thẳng: MQTT trong lab chưa mã hóa; TLS cổng 8883 là bước khi triển khai.

**28. Behaviour during a network outage** (Ngân)  
Khi mất mạng, bốn cơ chế giữ hệ thống đúng. Vòng điều khiển không bao giờ chờ mạng; mọi lệnh mạng đều có giới hạn thời gian và socket chết bị đóng sau 3 giây. Telemetry vào bộ đệm vòng 4 phút và được phát lại đúng thứ tự. Broker gửi last will để ai cũng biết thiết bị mất kết nối, và thiết bị tự nối lại, tự tìm lại broker theo tên.

**29. 06 · Experiments and results** (Uyên)  
Phần sáu: thí nghiệm và kết quả.

**30. Experiment plan** (Uyên)  
Quy tắc của nhóm: mỗi kết quả phải có chuẩn đối chiếu độc lập với hệ thống, và mọi con số lấy từ cơ sở dữ liệu bằng script ai cũng chạy lại được. Có hai buổi được phân tích: 23/9 cho chất lượng điều khiển, và 5/10 trên firmware cuối cho hiệu chuẩn, mất mạng và kiểm thử hệ thống.

**31. Level calibration against a ruler** (Uyên)  
Nhóm hiệu chuẩn mực nước khi đóng van, bơm từng bậc 20 giây, lấy chuẩn là một lần đọc thước cộng thể tích đã bơm. Ngoài một dải hẹp, sai số là 0,68 cm. Trong dải đó, khi mặt nước cách cảm biến khoảng 8 cm, cả 40 lần đo cho cùng một tiếng dội sai. Vì nó ổn định nên không lọc như nhiễu được; firmware loại nó và đi qua dải đó bằng mô hình bồn.

**32. Control response and switching** (Uyên)  
Đây là 76 phút vận hành thật. Mọi lần bơm tự động dừng trong khoảng 70,1 tới 70,8 phần trăm, vượt khoảng 1,4 phần trăm do nước còn trong ống. Bơm bật khoảng 13 lần mỗi giờ, nghỉ ngắn nhất 32 giây nên không bật tắt liên tục, và không luật lỗi nào báo nhầm.

**33. Volume estimation error** (Uyên)  
Để kiểm thể tích, nhóm so lượng thiết bị tính với chuẩn lấy từ độ dâng mực nước cộng lượng xả đo riêng. Sai số trung bình âm 5,2 phần trăm. Sai số lớn nhất trùng lúc xả nhanh nhất, cho thấy nguyên nhân: nhóm giả định bơm luôn cho 0,36 lít/phút, nhưng thực tế thay đổi theo xô nguồn. Dùng cảm biến đầu vào đã hiệu chuẩn để tính thể tích là cách sửa.

**34. Command latency** (Uyên)  
Độ trễ chính nhóm báo cáo là thời gian khứ hồi của lệnh, vì nó được đo bằng một đồng hồ của máy chủ nên lệch đồng hồ tự triệt tiêu. Firmware cuối xác nhận lệnh trong 29 ms trung vị, 48,5 ms ở phân vị 95, không mất lệnh nào. Bước cải thiện lớn nhất là tắt chế độ tiết kiệm điện Wi-Fi, giảm phân vị 95 từ 884 xuống 315 ms.

**35. Fault detection** (Uyên)  
Khi một lỗi xảy ra, bơm dừng ngay trong cùng chu kỳ 200 ms; điều này đúng ở cả 18 sự kiện tràn và không tiến triển. Rút dây echo thì báo lỗi cảm biến đúng thiết kế, và mọi lần bật tay khi đang lỗi đều bị từ chối. Nhóm chưa gây lỗi NO_CURRENT thật, nên 2 giây là giá trị thiết kế; cái đo được là tín hiệu của nó, 353 tới 365 mA khi rơ-le đóng và 0 khi mở.

**36. Network outage with the pump running** (Uyên)  
Đây là bài thử đề quan tâm nhất. Nhóm cắt mạng thiết bị 120 giây khi bơm đang chạy ở 9 phần trăm. Bồn vẫn bơm lên 51 phần trăm không đổi trạng thái, rồi tự dừng ở 70,7 phần trăm. 125 bản tin đệm được phát lại đúng thứ tự, chỉ mất 4, là những tin gửi trong 3 giây trước khi thiết bị nhận ra đường truyền đã chết.

**37. Automated system test, 5/5 pass** (Uyên)  
Nhóm còn viết bài kiểm thử nghiệm thu tự động, ai cũng chạy được mà không cần người thao tác. Nó kiểm tra kết nối, bảo mật, 40 lệnh, các lần từ chối đúng lý do và dòng điện bơm. Trên firmware cuối cả năm bài đều đạt.

**38. 07 · Lessons and conclusion** (Dương)  
Phần bảy: bài học và kết luận.

**39. Bugs found on the bench** (Dương)  
Bốn lỗi dạy nhóm nhiều nhất, và lỗi nào cũng tìm ra nhờ đếm. Rơ-le bị đảo làm mọi lệnh dừng lại bật bơm. Bơm tự bật sau mỗi lần khởi động vì mức nước được coi là hợp lệ trước khi có số đo. Một tiếng dội sai lặp lại hoàn hảo đánh lừa luật tin vào sự lặp lại. Và bản tin bị cắt âm thầm do bộ đệm nhỏ, mọi thứ trông bình thường mà không có gì được lưu.

**40. Limitations and next steps** (Dương)  
Nhóm cho rằng nói rõ hạn chế cũng quan trọng như khoe kết quả. Cảm biến lưu lượng mới kiểm chứng một phần. Dòng điện bơm đọc cao hơn định mức, cần một lần đo bằng đồng hồ. NO_CURRENT chưa được gây lỗi thật. Cảm biến siêu âm có vùng mù. Và đường MQTT trong lab chưa có TLS.

**41. Conclusion** (Dương)  
Ba ý làm nên dự án. Điều khiển và an toàn nằm ở biên, nên bồn vẫn chạy khi mất mạng, như bài thử mất mạng đã cho thấy. An toàn nằm trong cấu trúc: một hàm bảo vệ và bảy lớp chống tràn độc lập. Và nhóm tin số đo hơn datasheet; mọi bất ngờ trong dự án đều nhờ đo mà ra. Giờ xin mời xem bồn thật.

**42. Live demonstration** (Dương)  
Tám bước trên bồn thật. Cho mực nước tụt, bơm tự bật và tự tắt; gửi lệnh tay và thấy câu trả lời đã xác nhận; nhấc phao thì bơm dừng; nhấc đầu hút thì nhãn sức khỏe chuyển sang không có dòng chảy; tắt Wi-Fi mà bơm vẫn dừng ở 70 phần trăm; rút một dây bơm; và khởi động lại ESP32 để thấy bơm vẫn tắt. Có sẵn video dự phòng.

**43. Thank you** (Dương)  
Cảm ơn thầy và các bạn đã lắng nghe. Nhóm xin nhận câu hỏi.

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
