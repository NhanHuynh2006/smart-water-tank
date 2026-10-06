# Kịch bản thuyết trình — Bồn nước thông minh (Project 08)

Bản rút gọn: mỗi slide chỉ nói ý chính, khoảng 7 phút, sau đó demo. Lên thuyết trình thì nói theo bản tiếng Anh `SCRIPT_EN.md` (cũng là ghi chú người nói trong slide); bản này để hiểu và tập.

| # | Slide | Người nói | Thời gian | Hết lúc |
|---|---|---|---|---|
| 1 | Smart Water Tank | Bảo | 0:15 | 0:15 |
| 2 | Seven parts, then the live demo | Bảo | 0:05 | 0:20 |
| 3 | 01 · Problem and requirements | Bảo | 0:05 | 0:25 |
| 4 | What a smart tank must do | Bảo | 0:10 | 0:35 |
| 5 | Requirements and where they are met | Bảo | 0:15 | 0:50 |
| 6 | 02 · Architecture and hardware | Bảo | 0:05 | 0:55 |
| 7 | System architecture | Bảo | 0:20 | 1:15 |
| 8 | The server never switches the pump. It asks, and the ESP32 decides. | Bảo | 0:15 | 1:30 |
| 9 | The bench rig | Bảo | 0:15 | 1:45 |
| 10 | The prototype on the bench | Bảo | 0:15 | 2:00 |
| 11 | Controller board schematic | Bảo | 0:15 | 2:15 |
| 12 | Three signal lessons, each found by measuring | Bảo | 0:15 | 2:30 |
| 13 | 03 · MQTT protocol | Nhân | 0:05 | 2:35 |
| 14 | MQTT topic design | Nhân | 0:15 | 2:50 |
| 15 | Commands with acknowledgement | Nhân | 0:10 | 3:00 |
| 16 | 04 · Control and intelligence | Nhân | 0:05 | 3:05 |
| 17 | Level measurement pipeline | Nhân | 0:10 | 3:15 |
| 18 | Model bridging and flow from level | Nhân | 0:10 | 3:25 |
| 19 | Controller state machine | Nhân | 0:10 | 3:35 |
| 20 | Eleven fault rules | Nhân | 0:10 | 3:45 |
| 21 | Safety guard and overflow layers | Nhân | 0:10 | 3:55 |
| 22 | Pump health classification | Nhân | 0:10 | 4:05 |
| 23 | Rolling baseline and daily volume | Nhân | 0:15 | 4:20 |
| 24 | 05 · Backend, dashboard, security | Ngân | 0:05 | 4:25 |
| 25 | Backend and data model | Ngân | 0:05 | 4:30 |
| 26 | Dashboard and remote access | Ngân | 0:10 | 4:40 |
| 27 | Security model | Ngân | 0:10 | 4:50 |
| 28 | Behaviour during a network outage | Ngân | 0:10 | 5:00 |
| 29 | 06 · Experiments and results | Uyên | 0:05 | 5:05 |
| 30 | Experiment plan | Uyên | 0:10 | 5:15 |
| 31 | Level calibration against a ruler | Uyên | 0:15 | 5:30 |
| 32 | Control response and switching | Uyên | 0:10 | 5:40 |
| 33 | Volume estimation error | Uyên | 0:10 | 5:50 |
| 34 | Command latency | Uyên | 0:05 | 5:55 |
| 35 | Fault detection | Uyên | 0:10 | 6:05 |
| 36 | Network outage with the pump running | Uyên | 0:10 | 6:15 |
| 37 | Automated system test, 5/5 pass | Uyên | 0:05 | 6:20 |
| 38 | 07 · Lessons and conclusion | Dương | 0:05 | 6:25 |
| 39 | Bugs found on the bench | Dương | 0:10 | 6:35 |
| 40 | Limitations and next steps | Dương | 0:10 | 6:45 |
| 41 | Conclusion | Dương | 0:10 | 6:55 |
| 42 | Live demonstration | Dương | 0:15 | 7:10 |
| 43 | Thank you | Dương | 0:05 | 7:15 |

**1. Smart Water Tank** (Bảo)  
Chào thầy và các bạn. Nhóm em là nhóm [số], đề tài Project 08: Bồn nước thông minh. Ý chính: mọi quyết định điều khiển và an toàn chạy trên ESP32, nên mất mạng bồn vẫn tự chạy.

**2. Seven parts, then the live demo** (Bảo)  
Bảy phần, sau đó demo trên bồn thật.

**3. 01 · Problem and requirements** (Bảo)  
Phần một: bài toán.

**4. What a smart tank must do** (Bảo)  
Đề yêu cầu sáu việc: đo, điều khiển, bảo vệ, chẩn đoán, giám sát, và vẫn chạy khi mất mạng. Yêu cầu cuối quyết định toàn bộ thiết kế.

**5. Requirements and where they are met** (Bảo)  
Mọi yêu cầu đều xong, trừ hai chỗ nói thẳng: cảm biến lưu lượng có lắp và đầu vào đã hiệu chuẩn nhưng điều khiển vẫn dùng lưu lượng suy từ mức nước; đường nền rò rỉ cần thêm ngày dữ liệu.

**6. 02 · Architecture and hardware** (Bảo)  
Phần hai: kiến trúc và phần cứng.

**7. System architecture** (Bảo)  
Cảm biến vào ESP32, ESP32 khép vòng điều khiển mỗi 200 ms. MQTT đưa dữ liệu lên broker và lệnh xuống. FastAPI lưu SQLite và phục vụ dashboard, đường hầm đưa ra công khai. Đường duy nhất tới bơm là qua ESP32.

**8. The server never switches the pump. It asks, and the ESP32 decides.** (Bảo)  
Quyết định chính: cái gì cần dừng an toàn thì chạy trên ESP32. Máy chủ chỉ giữ lịch sử và phân tích chậm. Mất mạng chỉ mất phần xem, không mất điều khiển.

**9. The bench rig** (Bảo)  
Bồn nhỏ 10 nhân 10 cm, 1,24 lít. Bơm thật chỉ cho 0,36 lít/phút, kém datasheet năm lần, và mọi ngưỡng đều lấy theo số đo này.

**10. The prototype on the bench** (Bảo)  
Đây là mô hình thật: bồn có cảm biến và phao trên nắp, vòng nước có hai cảm biến lưu lượng và van xả, bo điều khiển đặt trên xô nguồn.

**11. Controller board schematic** (Bảo)  
Bo mạch của nhóm. Vào 12 V, hai mạch hạ áp ra 5 V và 3,3 V. Mọi tín hiệu 5 V của cảm biến qua cầu chia 10k/20k; rơ-le, phao và nút reset nằm trên nguồn 3,3 V.

**12. Three signal lessons, each found by measuring** (Bảo)  
Ba bài học đi dây, đều nhờ đo mà ra: bộ chuyển mức làm mất tiếng dội, chân thả nổi bắt nhiễu điện lưới, và nhiễu bơm chỉ hết sau khi làm lại tầng công suất.

**13. 03 · MQTT protocol** (Nhân)  
Phần ba: giao thức MQTT.

**14. MQTT topic design** (Nhân)  
Mỗi topic một việc, QoS chọn theo cái giá khi mất tin. Trạng thái dùng last will, mỗi bản tin có số thứ tự, và tài khoản thiết bị không gửi được lệnh.

**15. Commands with acknowledgement** (Nhân)  
Bấm nút thành một lệnh có mã riêng. Màn hình chỉ đổi khi ESP32 xác nhận, kèm lý do nếu từ chối.

**16. 04 · Control and intelligence** (Nhân)  
Phần bốn: điều khiển và trí tuệ.

**17. Level measurement pipeline** (Nhân)  
Không bao giờ tin một lần đo. Mỗi số đo qua năm bước, từ cửa sổ 15 lần đo tới bộ lọc alpha-beta. Song song, phép đo khoảng cách thô có thể tự kích hoạt chống tràn.

**18. Model bridging and flow from level** (Nhân)  
Một mô hình bồn chạy song song với cảm biến. Khi cảm biến mù, mô hình điều khiển tối đa 90 giây; quá thì dừng bơm.

**19. Controller state machine** (Nhân)  
Bảy trạng thái, dải 30 tới 70 phần trăm, có thời gian chạy và nghỉ tối thiểu. Trạng thái lỗi được giữ lại, nên bơm lỗi không tự chạy lại.

**20. Eleven fault rules** (Nhân)  
Mười một luật lỗi, xét mỗi 200 ms. Luật nào kích hoạt cũng dừng bơm. NO_PROGRESS là luật "bơm chạy mà không có dòng chảy" của đề, đo bằng mực nước.

**21. Safety guard and overflow layers** (Nhân)  
Mọi lần bật bơm, tự động hay tay, đều qua một hàm bảo vệ. Chống tràn có bảy lớp độc lập, từ ngưỡng 70 phần trăm tới phao cơ.

**22. Pump health classification** (Nhân)  
Dòng điện cộng với việc nước có chảy hay không phân biệt hai lỗi: có dòng mà không có nước là lỗi thủy lực; không có dòng là lỗi điện.

**23. Rolling baseline and daily volume** (Nhân)  
Backend học mức dùng bình thường cho từng nửa giờ trong ngày và báo khi vượt kéo dài. Mỗi khe cần ba ngày dữ liệu, nhóm chưa đủ.

**24. 05 · Backend, dashboard, security** (Ngân)  
Phần năm: backend, dashboard và bảo mật.

**25. Backend and data model** (Ngân)  
Một tiến trình Python, sáu bảng SQLite, một REST API. Lệnh và cấu hình cần phiên quản trị.

**26. Dashboard and remote access** (Ngân)  
Ai có link cũng xem được; chỉ quản trị có mật khẩu mới điều khiển được. Hình bồn vẽ ngưỡng đúng độ cao thật.

**27. Security model** (Ngân)  
Mật khẩu và ACL ở broker, cookie phiên, khóa khi sai nhiều lần. Phao và thời gian đóng cắt tối thiểu chặn tấn công bằng chính thiết kế. Lỗ hổng còn lại: MQTT trong lab chưa mã hóa.

**28. Behaviour during a network outage** (Ngân)  
Khi mất mạng: vòng điều khiển không chờ, dữ liệu vào bộ đệm 4 phút, broker báo thiết bị mất kết nối, và thiết bị tự nối lại.

**29. 06 · Experiments and results** (Uyên)  
Phần sáu: thí nghiệm và kết quả.

**30. Experiment plan** (Uyên)  
Mỗi kết quả có chuẩn đối chiếu độc lập và lấy từ cơ sở dữ liệu bằng script. Hai buổi: 23/9 cho điều khiển, 5/10 cho firmware cuối.

**31. Level calibration against a ruler** (Uyên)  
So với thước, sai số mực nước 0,68 cm ngoài một vùng mù, nơi cảm biến trả về tiếng dội sai ổn định. Firmware loại nó và đi qua vùng đó bằng mô hình.

**32. Control response and switching** (Uyên)  
Mọi lần bơm tự động dừng trong khoảng 70,1 tới 70,8 phần trăm, khoảng 13 lần bật mỗi giờ, không bật tắt liên tục, không báo động nhầm trong 84 phút.

**33. Volume estimation error** (Uyên)  
Sai số thể tích trung bình âm 5,2 phần trăm, do giả định lưu lượng bơm không đổi. Cảm biến đầu vào đã hiệu chuẩn là cách sửa.

**34. Command latency** (Uyên)  
Lệnh được xác nhận trong 29 ms trung vị. Tắt chế độ tiết kiệm điện Wi-Fi là bước cải thiện lớn nhất.

**35. Fault detection** (Uyên)  
Lỗi dừng bơm ngay trong cùng chu kỳ 200 ms. Nhóm chưa gây lỗi NO_CURRENT thật; thay vào đó đo tín hiệu của nó, 353 tới 365 mA khi chạy và 0 khi tắt.

**36. Network outage with the pump running** (Uyên)  
Nhóm cắt mạng 120 giây khi đang bơm. Bồn vẫn bơm và tự dừng ở 70,7 phần trăm; 125 bản tin được phát lại, chỉ mất 4.

**37. Automated system test, 5/5 pass** (Uyên)  
Bài kiểm thử nghiệm thu tự động chạy không cần người: năm bài, đều đạt.

**38. 07 · Lessons and conclusion** (Dương)  
Phần bảy: bài học và kết luận.

**39. Bugs found on the bench** (Dương)  
Bốn lỗi, đều tìm ra nhờ đếm: rơ-le bị đảo, bơm tự bật sau mỗi lần khởi động, tiếng dội sai lặp lại, và bản tin bị cắt âm thầm.

**40. Limitations and next steps** (Dương)  
Còn mở: cảm biến lưu lượng mới kiểm chứng một phần, dòng điện chưa đo bằng đồng hồ, chưa gây lỗi NO_CURRENT thật, vùng mù siêu âm, và MQTT chưa có TLS.

**41. Conclusion** (Dương)  
Điều khiển nằm ở biên, an toàn nằm trong cấu trúc, và nhóm tin số đo hơn datasheet. Giờ xin mời xem bồn thật.

**42. Live demonstration** (Dương)  
Tám bước trên bồn thật: bơm tự động, lệnh tay, phao, hút khô, mất mạng, rút dây bơm và khởi động lại. Có sẵn video dự phòng.

**43. Thank you** (Dương)  
Cảm ơn thầy và các bạn. Nhóm xin nhận câu hỏi.

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
