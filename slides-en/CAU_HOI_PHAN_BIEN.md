# Câu hỏi phản biện dự kiến — Nhóm 08, Bồn nước thông minh

Soạn theo kiểu câu hỏi thầy đã hỏi các nhóm khác: một tình huống cụ thể, hỏi "tại sao" và "chuyện gì xảy ra nếu". Mỗi câu có **ý trả lời** dựa đúng vào hệ thống và số đo của nhóm. Câu nào có **⚠** là chỗ dễ bị bắt bẻ, phải trả lời thật, không nói quá.

Ai trả lời: theo phần mình thuyết trình (Bảo: phần 1–2, Nhân: 3–4, Ngân: 5, Uyên: 6, Dương: 7 và demo).

---

## A. Điều khiển, hysteresis, máy trạng thái

**A1. Nhóm dùng dải trễ 30–70 % kèm thời gian chạy tối thiểu 3 s và nghỉ tối thiểu 20 s. Giải thích vì sao chỉ dùng một ngưỡng đơn, ví dụ 50 %, sẽ gây bật tắt liên tục (chattering), và hai giới hạn thời gian bảo vệ rơ-le và động cơ bơm thế nào?**
- Một ngưỡng: số đo luôn dao động quanh ngưỡng (nhiễu vài mm, gợn mặt nước khi bơm chạy), nên mỗi lần dao động qua ngưỡng là đổi trạng thái. Bơm vừa bật làm mặt nước gợn, số đo nhảy qua lại, rơ-le đóng cắt liên tục.
- Dải 30–70: một khi đã bật thì phải lên tới 70 mới tắt, đã tắt thì phải xuống 30 mới bật. Nhiễu vài mm không thể đẩy mực đi 40 % nên không chattering.
- Hai giới hạn thời gian chặn cả dao động thật (ví dụ xả mạnh đúng lúc mực ở sát ngưỡng). Mỗi lần bật, động cơ chổi than kéo dòng khởi động lớn và tiếp điểm rơ-le bị hồ quang; nghỉ tối thiểu 20 s giới hạn tối đa khoảng 3 lần bật mỗi phút, cho động cơ và tiếp điểm nguội.
- Số chứng minh: mô phỏng ngưỡng đơn 49–51 % bật tắt 472 lần/giờ, cấu hình 30–70 chỉ 8 lần/giờ (giảm 98,3 %). Trên phần cứng 12,9 lần/giờ, lần nghỉ ngắn nhất 32 s, không lần nào chattering.

**A2. Vì sao thời gian chạy tối thiểu chỉ 3 s mà không phải 10 s như bản nháp đầu?**
- Ở 0,36 L/phút, chạy ép 10 s thêm 0,06 L, gần 5 % bồn 1,24 L, trong khoảng đó máy trạng thái không có quyền dừng. Với bồn nhỏ, ép chạy lâu là nguy cơ tràn. 3 s đủ chống rơ-le rung mà dải trễ vẫn quyết định.
- Lưu ý: rào an toàn (phao, khoảng cách thô, lỗi) **không** bị thời gian tối thiểu chặn; chống tràn luôn thắng.

**A3. Đang bơm (FILLING) mà mực chưa lên 70 % nhưng bơm đã chạy rất lâu thì bộ điều khiển xử lý thế nào để vừa cấp nước vừa không làm hỏng bơm hay tràn?**
- Có ba mốc theo thời gian và thể tích, không phụ thuộc cảm biến mực: `NO_PROGRESS` (90 s mà mực không lên 1,5 cm), `FILL_TIMEOUT` (một lần chạy quá 240 s), `HARD_LIMIT` (quá 300 s, vô điều kiện), `VOLUME_LIMIT` (bơm quá 2,5 L trong một lần).
- Chạm mốc nào thì dừng bơm, báo lỗi và **khóa**, chờ người kiểm tra; không tự bơm lại vì nếu nguyên nhân còn (ống tuột, xả quá to) thì bơm lại chỉ hại thêm.
- Ví dụ thật: ngày 05/10 và 06/10, van xả mở to đến mức nước xả bằng nước bơm ở khoảng 48 %; `NO_PROGRESS` đã dừng bơm sau 90 s với nhãn `no_flow`.
- ⚠ Nếu thầy hỏi "sao không tự thử lại sau một lúc": nhóm chọn an toàn trước, vì bơm chìm chạy khô hoặc chạy không có tác dụng là hỏng thật. Tự thử lại có giới hạn số lần là hướng mở rộng.

**A4. Vì sao dùng máy trạng thái mà không dùng mấy câu if?**
- Lỗi phải được **giữ lại**. Một câu if sẽ cho bơm chạy lại ngay khi điều kiện lỗi biến mất, ví dụ đầu hút vừa chạm nước lại; máy trạng thái giữ ở `FAULT_DRYRUN` cho tới khi người bấm Xóa lỗi.
- Có bất biến kiểm tra mỗi chu kỳ: có mã lỗi thì trạng thái phải là trạng thái lỗi (bản cũ từng có lỗi "trông như IDLE mà từ chối mọi lệnh bật").

**A5. Trạng thái IDLE là gì, khác BOOT thế nào?**
- IDLE: bơm tắt, không lỗi, đang chờ. Ở chế độ tự động, chờ mực xuống dưới 30 % hoặc phao dưới báo thấp.
- BOOT: vừa khởi động, **chờ có số đo mực nước đầu tiên được chấp nhận** rồi mới vào IDLE. Đây là cách sửa lỗi "bơm tự bật sau mỗi lần khởi động" (mực 0 % bị coi là hợp lệ trước khi đo).

---

## B. An toàn và tình huống hỏng hóc

**B1. (Giống câu Fail-Secure) Rơ-le cấp nguồn cho bơm qua tiếp điểm thường mở (NO). Nếu mất điện đột ngột, đứt dây điều khiển, hay ESP32 chết, bơm ở trạng thái nào? Vì sao chọn như vậy cho bồn nước?**
- Mất điện cuộn rơ-le thì tiếp điểm NO mở ra, bơm **tắt**. Với bồn nước, mối nguy là **tràn**, nên trạng thái an toàn là bơm tắt (fail-safe theo hướng không bơm).
- Lệnh đầu tiên khi khởi động là ép chân rơ-le về mức tắt, trước cả Serial và cảm biến.
- ⚠ Điểm yếu nói thật: trong khoảng 300 ms bootloader chạy, chân GPIO 10 chưa được điều khiển; muốn chắc chắn tuyệt đối cần điện trở kéo về mức tắt bằng phần cứng.
- ⚠ Nếu ESP32 **treo** (không mất điện) thì rơ-le giữ trạng thái cuối. Nhóm không bật watchdog riêng; lớp chặn cuối cùng lúc đó là phao. Hướng sửa: bật task watchdog và nối phao trên nối tiếp cứng với bơm.

**B2. Chế độ thủ công có vượt được an toàn không? Giả sử đang bơm tay rồi nước chạm phao trên.**
- Không. Mọi đường bật bơm (tự động và tay) đi qua **một hàm duy nhất** `pumpBlockReason()`: có lỗi, phao trên, nước quá gần cảm biến, ≥ 85 %, mực không tin được, chưa hết thời gian nghỉ.
- Khi bơm tay đang chạy, hàm này được xét lại **mỗi 200 ms**, nên phao trên lên là bơm tắt trong một chu kỳ, báo `OVERFLOW`. Lúc phao còn nhấc, lệnh Xóa lỗi bị từ chối với lý do `condition_still_present`.

**B3. Nếu cảm biến siêu âm hỏng hoặc rút dây, hệ thống có thể bơm tràn không?**
- Không. Có bảy lớp chống tràn độc lập. Hai lớp dựa vào mực đã lọc (dừng ở 70 %, khóa ở 85 %) sẽ mất, nhưng còn: khoảng cách thô (3 lần đo liên tiếp dưới 4,5 cm), phao cơ, giới hạn 240 s / 300 s / 2,5 L, và ngắt rơ-le khi khởi động.
- Mất số đo quá 90 s (khi mô hình còn hợp lệ) thì báo `SENSOR_TIMEOUT`, khóa bơm; cắm lại thì tự xóa sau 2,5 s tín hiệu tốt.

**B4. Cảm biến báo sai nhưng vẫn "hợp lý", ví dụ báo 20 % trong khi bồn đầy, thì sao?**
- Đây là lý do có phao. Phao trên đóng mà siêu âm báo dưới 70 %, hoặc phao dưới đóng mà siêu âm báo trên 60 % thì báo `SENSOR_CONFLICT`. Hai nguồn độc lập mâu thuẫn thì không tin cái nào, dừng bơm.

**B5. Bơm chạy khô thì phát hiện bằng gì, mất bao lâu? (Đề hỏi "bơm chạy mà gần như không có dòng chảy")**
- Lớp 1, theo **dòng điện**: bơm ngập nước kéo khoảng 320–330 mA, nhấc khỏi nước còn 215–230 mA vì động cơ quay không tải. Dưới 270 mA liên tục 3 s (sau 2 s khởi động) thì báo `DRY_RUN`, khóa bơm, tổng khoảng 5 s.
- Lớp 2, theo **mực nước**: `NO_PROGRESS` sau 90 s mực không lên.
- Lớp 3: không có dòng chút nào trong 2 s là `NO_CURRENT` (lỗi điện).
- ⚠ Ngưỡng 270 mA đo trên con bơm này, nguồn này; đổi bơm hay đổi điện áp phải đo lại.

**B6. Đứt một dây bơm thì sao? Phân biệt với chạy khô thế nào?**
- Rơ-le đóng mà dòng gần 0 trong 2 s thì báo `NO_CURRENT`, nhãn "lỗi điện". Chạy khô thì **vẫn có dòng** nhưng thấp. Cùng triệu chứng "bồn không đầy" nhưng chỉ đúng chỗ sửa: điện hay nước.
- ⚠ `NO_CURRENT` chưa được thử bằng cách rút dây thật trước buổi bảo vệ; nhóm đã đo tín hiệu nó dựa vào và sẽ rút dây ngay trong demo.

**B7. Mất điện rồi có điện lại, hệ thống làm gì?**
- Rơ-le tắt ngay lệnh đầu tiên; ở BOOT chờ số đo thật; tổng thể tích đọc lại từ bộ nhớ flash (ghi mỗi 60 s, mất tối đa 1 phút dữ liệu thể tích); bản tin trạng thái gửi kèm lý do khởi động lại (mất điện, sụt áp, watchdog, nạp lại) để phân biệt.
- Bằng chứng: ngày 05/10 khởi động lại khi bồn 33 %, mức đầu tiên gửi lên là 32 %, bơm không bật.

**B8. Động cơ khởi động kéo dòng lớn làm sụt áp, ESP32 có bị reset không? Nhóm xử lý thế nào?**
- Có nguy cơ brownout. Tụ 470 µF trên đường 5 V cấp dòng đỉnh lúc khởi động; tụ 100 nF + 100 µF sát chân nguồn ESP32; diode 1N4007 và tụ 100 nF ở hai cực bơm dập xung cảm ứng khi ngắt.
- Firmware báo lý do khởi động lại, nên nếu có brownout sẽ thấy ngay trên bản tin trạng thái.

**B9. Diode 1N4007 mắc song song ngược ở bơm để làm gì? Không có thì sao?**
- Ngắt dòng qua cuộn cảm, điện áp V = L·di/dt vọt lên hàng trăm vôn, phóng hồ quang ở tiếp điểm rơ-le và gây nhiễu. Diode cho dòng cảm ứng chạy vòng qua cuộn dây và tắt dần.
- Tụ 100 nF ở cực bơm thì để dập nhiễu chổi than, không phải để dập xung.

---

## C. Cảm biến và đo đạc

**C1. Vì sao không tin một lần đo siêu âm? Bộ lọc gồm những gì?**
- Chùm sóng tỏa khoảng 15°, rộng hơn bồn 10 cm, nên một số tiếng dội về từ thành hoặc đáy. Năm bước: cửa sổ 15 lần đo, bỏ số xa hơn đáy; loại cửa sổ nếu khoảng tứ phân vị > 3,5 cm; lấy phân vị 25; kiểm tra vật lý (tốc độ thay đổi, không được tụt khi đang bơm); bộ lọc alpha-beta.
- Vì sao phân vị 25 chứ không phải trung vị: tiếng dội yếu bị bắt trễ một hoặc vài chu kỳ 40 kHz (mỗi chu kỳ khoảng 0,43 cm), sai số chỉ làm khoảng cách **dài ra**, nên số ngắn hơn đáng tin hơn.
- Vì sao khoảng tứ phân vị chứ không phải lớn nhất trừ nhỏ nhất: một tiếng dội lạc đủ làm hỏng max−min và vứt cả cửa sổ; đổi sang IQR nâng tỉ lệ nhận từ 60 lên 93 %.

**C2. Vùng mù là gì? Sao không sửa bằng phần mềm cho hết?**
- Khi mặt nước cách cảm biến khoảng 7,9–8,7 cm, cả 40 lần đo cho cùng một giá trị sai (tiếng dội lạc ổn định). Vì ổn định nên không lọc như nhiễu được.
- Phần mềm: bỏ số xa hơn đáy, cổng mô hình loại số lệch mô hình quá 2,5 cm, và mô hình bồn điều khiển tối đa 90 s để vượt qua vùng này.
- Nguyên nhân gốc là cơ khí (sóng dội vào thành/mép). Sửa tận gốc: dán mút quanh đầu dò, ống lặng sóng, hoặc nâng cảm biến.
- ⚠ Đã từng có luật "giá trị lặp lại nhiều lần thì tin" và nó tin nhầm tiếng dội lạc, bật bơm mỗi 40 s; nhóm đã bỏ luật đó.

**C3. Hiệu chuẩn mực nước bằng cách nào? Chuẩn đối chiếu có độc lập không?**
- Đóng van, bơm từng bậc 20 s, chờ lặng 12 s, đo 40 lần. Chuẩn = một lần đọc thước (4,9 cm) + thể tích đã bơm chia 100 cm² đáy. Kết quả: RMSE 0,68 cm ngoài vùng mù.
- ⚠ Nói thật: hệ số cảm biến lưu lượng được đặt từ chính lượt đầu, nên các điểm lượt đầu chỉ kiểm tra độ tuyến tính; lượt thứ hai dùng lại hệ số đó không chỉnh mới là kiểm tra độc lập hơn.

**C4. Đề bắt buộc cảm biến lưu lượng. Yêu cầu đó thật sự đạt chưa?**
- ⚠ Không nói đơn giản là "đạt". Phần cứng đã lắp đủ hai YF-S401, đọc bằng ngắt, gửi lên. Tới 22/09 bơm gây khoảng 1500 Hz nhiễu (gấp 40 lần tín hiệu thật). Sau khi làm lại tầng công suất ngày 05/10, cảm biến đầu vào đếm được nước thật (0 Hz khi tắt, 20–30 Hz khi chạy), đã hiệu chuẩn K = 85,5 và đang cấp cho mô hình bồn.
- Nhưng điều khiển và bộ đếm thể tích vẫn dùng lưu lượng suy từ mực nước, vì cảm biến mới kiểm chứng trong một lần hiệu chuẩn. Cảm biến đầu ra nằm dưới ngưỡng khởi động 0,3 L/phút ở lưu lượng xả của bàn thử.
- Kết luận nên nói: "đã thực hiện, kiểm chứng một phần, nhóm chuyển sang ước lượng từ mực nước và ghi rõ hạn chế".

**C5. Sai số thể tích −5,2 % đến từ đâu?**
- Thiết bị giả định bơm luôn cho 0,36 L/phút. Sai số lớn nhất trùng lúc xả nhanh nhất và lúc xô nguồn thấp (05/10 bơm chỉ cho 0,24 L/phút). Tức là sai số thuộc về giả định lưu lượng không đổi, không phải do cảm biến mực. Sửa bằng cảm biến đầu vào đã hiệu chuẩn.

**C6. Dòng điện bơm đo được 350 mA mà bơm ghi 100–200 mA. Ai sai?**
- ⚠ Chưa kiểm bằng đồng hồ vạn năng. Hệ số đã sửa theo sơ đồ (cầu chia 10k/20k, 8,11 mA/mV); trước đó firmware giả định chia 1:1 nên còn báo cao hơn. Cách dùng dòng điện của nhóm là **so tương đối** (có/không, cao/thấp), nên kết luận chạy khô hay mất dòng vẫn đúng dù thang tuyệt đối lệch.

**C7. Sao phải chia áp các tín hiệu cảm biến?**
- Các cảm biến chạy 5 V, ngõ ra tới 5 V; chân ESP32 chịu tối đa 3,3 V. Cầu 10k/20k cho 5 × 20/30 = 3,33 V.
- Với echo, độ rộng xung là phép đo; từng dùng bộ chuyển mức tự động TXS0108E và mất khoảng 30 % số đo vì nó làm méo xung.

---

## D. MQTT, mạng, mất kết nối

**D1. Vì sao QoS từng topic khác nhau? Sao không để hết QoS 2 cho chắc?**
- Hỏi "mất một tin thì tốn gì": telemetry 1 Hz mất một mẫu không đổi xu hướng nên QoS 0; trạng thái bơm, lỗi, lệnh, xác nhận mất là hỏng chức năng nên QoS 1.
- QoS 2 cần bắt tay 4 bước và theo dõi gói trên ESP32. Trùng lặp mà QoS 2 chống thì nhóm đã xử lý ở tầng ứng dụng: mỗi lệnh có mã riêng, nhận trùng chỉ gửi lại xác nhận, không làm hai lần.

**D2. Last will và retained dùng vào đâu?**
- Last will: đăng ký sẵn bản tin `online:false`; nếu ESP32 mất đột ngột, broker tự phát sau khoảng 1,5 lần keep alive 15 s (đo được 21,8 s). Dashboard còn tự chuyển đỏ sau 5 s không có dữ liệu.
- Retained: trạng thái bơm và trạng thái thiết bị; dashboard mở sau vẫn nhận ngay trạng thái hiện tại.

**D3. Mất mạng giữa lúc đang bơm thì sao? Có bơm tràn không? Dữ liệu có mất không?**
- Toàn bộ điều khiển và an toàn ở ESP32, nên vẫn bơm và tự dừng ở 70 %. Thử 120 s cắt mạng khi đang ở 9 %: bồn lên 51 % không đổi trạng thái, tự dừng ở 70,7 %.
- Dữ liệu vào bộ đệm vòng 240 mẫu (khoảng 4 phút), phát lại đúng thứ tự: 125 mẫu được phát lại, mất 4/129.
- ⚠ 4 mẫu mất là những mẫu gửi vào socket thiết bị **tưởng còn sống** trong khoảng 3 s trước khi phát hiện. Bộ đệm chỉ bảo vệ dữ liệu sinh ra sau khi biết mất mạng. Bản firmware đầu chờ keep alive 15 s nên mất 10.

**D4. Mất mạng lâu hơn 4 phút thì sao?**
- Bộ đệm vòng ghi đè mẫu cũ nhất, giữ 4 phút gần nhất (có đếm `ring_drop`). Điều khiển vẫn đúng. Tổng thể tích không mất vì cộng trên thiết bị và lưu flash. Nếu cần giữ lâu hơn: ghi bộ đệm ra flash hoặc giảm tần số mẫu khi offline.

**D5. Kết nối lại có làm vòng điều khiển bị treo không?**
- Có thể, nếu không cẩn thận: mở TCP tới broker không trả lời mặc định chặn 3 s; ghi vào socket chết từng chặn tới 10 s. Nhóm giới hạn mở kết nối 0,5 s, kiểm tra socket ghi được trước mỗi lần gửi, đóng socket sau 3 s bị từ chối. Đo được vòng điều khiển dài nhất lúc mất mạng là 0,88 s và thiết bị tự báo con số này.

**D6. Laptop đổi IP (đổi mạng) thì ESP32 tìm broker thế nào?**
- Tìm theo tên mDNS `Nolan.local`; không thấy thì dùng IP dự phòng. Đã gặp lỗi: tên không trả lời lần đầu thì kẹt ở IP cũ; đã sửa cho hỏi lại tên sau mỗi lần kết nối thất bại. Wi-Fi thử tối đa ba mạng, mỗi lần vào mạng cách nhau ít nhất 10 s.

**D7. Vì sao chọn Wi-Fi mà không dùng LoRa, Zigbee, BLE?**
- Có điện lưới nên không cần tiết kiệm năng lượng; trong nhà, tầm vài chục mét; Wi-Fi cho ESP32 địa chỉ IP nói thẳng với broker, không cần gateway. Nếu bồn ở xa, chạy pin thì LoRa hợp hơn.

---

## E. Lệnh, xác nhận, giao diện

**E1. Vì sao nút trên dashboard không đổi ngay khi bấm?**
- Nguyên tắc phản hồi (device shadow): màn hình chỉ hiện điều thiết bị đã xác nhận. Bấm → gửi lệnh có mã → "đang gửi" → ESP32 kiểm tra rào an toàn → trả lời `accepted`/`rejected` kèm lý do và trạng thái bơm thật. Nếu nút đổi ngay mà bơm bị từ chối thì người dùng tưởng bơm đang chạy.

**E2. Gửi cùng một lệnh hai lần (mạng chập chờn) thì bơm có bị bật hai lần không?**
- Không. Lệnh có mã riêng; thiết bị nhận trùng thì chỉ gửi lại xác nhận cũ. Tính lũy đẳng ở tầng ứng dụng.

**E3. Độ trễ lệnh đo thế nào cho đúng? Sao không đo một chiều?**
- Đo khứ hồi bằng một đồng hồ của máy chủ ở cả hai đầu nên lệch đồng hồ tự triệt tiêu: trung vị 29,1 ms, phân vị 95 là 48,5 ms, n = 40, không mất lệnh.
- Một chiều phải trừ hai đồng hồ, NTP trên vi điều khiển chỉ chính xác vài chục ms, bằng cỡ đại lượng cần đo; từng thấy một chiều 1100 ms trong khi khứ hồi 187 ms.
- Cải thiện lớn nhất: tắt chế độ ngủ modem Wi-Fi, phân vị 95 giảm từ 884 xuống 315 ms.

**E4. Vì sao dashboard hỏi định kỳ 1 Hz mà không dùng WebSocket như bài giảng gợi ý?**
- Thiết bị gửi đúng 1 Hz nên hỏi 1 Hz không phí; vài người xem, một thiết bị thì lưu lượng vài KB/s. WebSocket thêm kênh có trạng thái, heartbeat, kết nối lại. Trên khoảng 10 thiết bị hoặc cần 5 Hz thì nên chuyển.

---

## F. Backend, dữ liệu, bảo mật

**F1. Vì sao SQLite mà không dùng cơ sở dữ liệu chuỗi thời gian?**
- Giới hạn của CSDL quan hệ xuất hiện ở hàng nghìn lần ghi mỗi giây; nhóm ghi 1 dòng/giây từ 1 thiết bị. Vẫn theo nguyên tắc: chỉ đánh chỉ mục cột thời gian, không đánh chỉ mục số thứ tự để tránh bùng nổ chỉ mục.

**F2. Ai cũng mở được link công khai, vậy có ai điều khiển bơm được không?**
- Xem thì ai cũng được; điều khiển cần phiên quản trị (mật khẩu). Cookie HttpOnly, SameSite Strict, Secure qua HTTPS; sai 5 lần trong 5 phút khóa địa chỉ đó. Gọi thẳng API không có phiên bị trả 401.
- Từng thử phân quyền theo IP và bỏ: qua đường hầm Cloudflare mọi yêu cầu đều đến từ 127.0.0.1, nên luật đó vô tình cho cả Internet điều khiển.

**F3. Hệ thống có mã hóa đầu cuối không?**
- ⚠ **Không.** MQTT trong lab chạy cổng 1883 không mã hóa (chỉ trong mạng nội bộ); đường công khai thì dashboard đi qua HTTPS. Ai bắt được gói Wi-Fi trong lab có thể đọc telemetry và mật khẩu broker. Triển khai thật thì chuyển MQTTS cổng 8883, đổi lại ESP32 tốn thời gian và RAM cho bắt tay TLS.

**F4. Nếu kẻ xấu chiếm được tài khoản MQTT của ESP32 thì sao? Nếu giả dữ liệu mực nước thấp để bơm tràn?**
- ACL: tài khoản thiết bị không được ghi vào topic lệnh, nên không dùng nó để điều khiển bơm.
- Giả dữ liệu: điều khiển không dựa vào dữ liệu trên máy chủ; mực nước được đo trên chính thiết bị, và phao cơ, khoảng cách thô nằm trên thiết bị. Gửi lệnh liên tục cũng không làm bơm bật tắt nhanh hơn thời gian tối thiểu.

**F5. Phát hiện rò rỉ bằng đường nền làm thế nào? Đã phát hiện được chưa?**
- Chia ngày 48 khe 30 phút, mỗi khe học trung bình và phương sai theo trung bình trượt mũ (α = 0,2); vượt μ + 3σ hai khe liền thì báo. Khe có bộ đếm bị đặt lại hoặc thể tích vô lý thì bỏ qua.
- ⚠ Mỗi khe cần ba ngày dữ liệu mới được báo; sau bốn ngày thử ở các khung giờ khác nhau chưa khe nào đủ, nên mới trình bày được cơ chế, chưa có lần phát hiện thật.

---

## G. Kiến trúc và phạm vi

**G1. Vì sao đặt toàn bộ điều khiển trên ESP32 mà không để máy chủ quyết định?**
- Nguyên tắc bài giảng: cái gì cần dừng an toàn tức thì phải tính ở biên. Mạng có độ trễ và có thể mất; nếu máy chủ quyết định thì mất mạng là mất điều khiển. Máy chủ chỉ giữ việc cần nhớ nhiều ngày: lịch sử, báo cáo theo ngày, đường nền rò rỉ.

**G2. Mở rộng lên 100 bồn thì phải đổi gì?**
- Cây topic đã theo `wt/<site>/<device>` nên thêm thiết bị không đổi giao thức. Backend: chuyển sang CSDL chuỗi thời gian, WebSocket thay cho hỏi định kỳ, tài khoản riêng từng người, TLS, đường hầm có tên cố định, cập nhật firmware qua mạng.

**G3. Nếu bồn thật cao 2 m thì thiết kế còn đúng không?**
- Logic giữ nguyên, đổi tham số: chiều cao cảm biến, dải làm việc, lưu lượng bơm, các ngưỡng thời gian (vì đều tính từ lưu lượng đo thật). HC-SR04 đo tới khoảng 4 m nhưng nên dùng cảm biến chống nước; bồn rộng hơn thì hết chuyện tiếng dội từ thành.

**G4. Nhóm đã kiểm chứng hệ thống thế nào để tin các con số?**
- Mỗi thí nghiệm có chuẩn độc lập (thước, hình học bồn, đồng hồ máy chủ, số thứ tự bản tin). Mọi số lấy từ cơ sở dữ liệu bằng script chạy lại được. Có bài kiểm thử nghiệm thu tự động `systest.py` (5/5 đạt): kết nối, bảo mật, 40 lệnh, từ chối đúng lý do, dòng bơm.

---

## H. Câu "bẫy" có thể gặp

**H1. "Vậy cảm biến lưu lượng gắn vào cho có à?"** → Không. Nó đã đếm được nước thật sau khi sửa tầng công suất, đã hiệu chuẩn, đang dùng cho mô hình bồn. Nhóm chỉ chưa dùng nó để tính thể tích vì mới kiểm chứng một lần; đó là bước tiếp theo, đã ghi trong hạn chế.

**H2. "Sao bơm có lúc dừng ở 48 % chứ không phải 70 %?"** → Không phải lỗi điều khiển: van xả mở to tới mức nước ra bằng nước vào, mực đứng yên, luật `NO_PROGRESS` dừng bơm đúng như thiết kế để bơm không chạy vô ích. Vặn van nhỏ lại là bơm lên được 70 %.

**H3. "Vượt 70 % lên tới 76 % là sao?"** → Lần đó bồn vừa đi qua vùng mù bằng mô hình, bộ lọc chưa bắt kịp mực thật khi số đo quay lại, nên quyết định tắt hơi trễ. Vẫn còn cách ngưỡng tràn 85 % gần 9 điểm, và các lớp chống tràn khác vẫn hoạt động. Phiên điều khiển bình thường vượt chỉ 1,1–1,85 %.

**H4. "Phao cũng đo được mực, cần siêu âm làm gì?"** → Phao chỉ có hai trạng thái ở một độ cao cố định; siêu âm cho mực liên tục để điều khiển, tính lưu lượng, thể tích, phát hiện bơm không lên nước. Phao là lớp độc lập để phát hiện siêu âm báo sai và chặn tràn khi siêu âm hỏng.

**H5. "Nếu cùng lúc phao hỏng và siêu âm hỏng?"** → Còn khoảng cách thô (nếu chỉ bộ lọc sai), giới hạn 240 s, 300 s, 2,5 L theo thời gian và thể tích, độc lập với mọi cảm biến mực. Các lớp này không ngăn hoàn toàn mà giới hạn thiệt hại; nói thẳng đây là lớp yếu nhất.
