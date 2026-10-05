# Kịch bản quay video minh chứng — Project 08

Mục đích: mỗi clip chứng minh **một yêu cầu cụ thể của đề**. Quay xong, các clip này dùng cho hai việc: nộp làm minh chứng và làm video dự phòng cho buổi demo.

Tổng thời lượng sau khi cắt ghép: khoảng **10–12 phút**.

---

## 0. Chuẩn bị (làm một lần, trước khi quay)

| Việc | Cách kiểm tra |
|---|---|
| Chạy hệ thống: `./start.sh` | Terminal in ra link công khai và mật khẩu quản trị |
| ESP32 trực tuyến | Dashboard có chấm xanh "trực tuyến", dòng thời gian cập nhật dưới 2 s |
| Laptop mở dashboard `http://localhost:8000`, **đã đăng nhập** quản trị | Thấy các nút Tự động / Thủ công / Bật bơm |
| Điện thoại thứ hai mở **link công khai bằng 4G** (không vào Wi-Fi) | Thấy cùng số liệu, **không có** nút điều khiển |
| Chế độ **Tự động**, không có lỗi | Ô trạng thái ghi IDLE hoặc FILLING |
| Van xả **mở hé**: nước rút chậm hơn bơm | Khi bơm chạy, mức nước trên dashboard phải tăng |
| Bồn nguồn đủ nước | Đầu hút ngập hẳn |
| Mức nước khoảng **35–45 %** | Để clip 1 không phải đợi lâu |

**Cách đặt máy quay:** một máy quay cố định thấy được **cả bồn lẫn màn hình laptop** trong cùng khung hình. Nếu không được thì quay hai luồng (bồn và màn hình) rồi ghép đôi khung hình khi dựng. Bật đồng hồ trên màn hình laptop để người xem đối chiếu thời gian.

**Lời dẫn:** mỗi clip mở đầu bằng một câu nói "clip này chứng minh gì". Gợi ý lời dẫn có sẵn ở mỗi clip, nói tiếng Việt hoặc tiếng Anh tùy nhóm.

---

## Clip 1 — Điều khiển tự động có trễ, bơm tự bật dưới 30 % và tự tắt ở 70 % (2–4 phút, tua nhanh phần giữa)

**Chứng minh:** đầu đo và cảm biến lưu lượng hoạt động; điều khiển tự động bằng dải trễ 30–70 %; dashboard cập nhật thời gian thực khi bơm và khi xả.

1. Lời dẫn: *"Hệ thống đang ở chế độ tự động. Ngưỡng bật bơm là 30 %, ngưỡng tắt là 70 %."*
2. Mở van xả to hơn một chút để mức nước tụt. Quay **thước trên dashboard và mặt nước trong bồn tụt cùng lúc**, cả trên laptop lẫn điện thoại 4G.
3. Khi mức xuống dưới 30 %: bơm tự bật, ô bơm chuyển xanh, dòng điện khoảng 470 mA, nhãn sức khỏe bơm "ok", lưu lượng vào khoảng 0,3 L/phút.
4. Vặn van xả về mức hé. Tua nhanh đoạn bơm.
5. Ở khoảng 70 %: bơm **tự tắt**. Quay rõ con số trên dashboard đúng lúc rơ-le kêu "tách".
6. Lời kết: *"Bơm tự dừng ở 70 %, không cần người can thiệp."*

## Clip 2 — Lệnh có xác nhận, và người xem không điều khiển được (1 phút)

**Chứng minh:** lệnh từ dashboard đi qua MQTT tới ESP32 và có xác nhận; giao diện chỉ hiện trạng thái đã được thiết bị xác nhận; người dùng không đăng nhập thì chỉ xem.

1. Trên laptop bấm **Thủ công**, rồi bấm **Bật bơm**. Quay dòng "Đang gửi lệnh…" chuyển thành kết quả, và bơm chạy thật trong bồn.
2. Bấm **Tắt bơm**, rồi bấm **Bật bơm** ngay lập tức. Dashboard phải báo **bị từ chối, còn chờ N giây** (bảo vệ bơm khỏi bật tắt liên tục).
3. Quay màn hình điện thoại 4G: không có nút điều khiển nào.
4. Lời dẫn: *"Mỗi lệnh có mã riêng và được ESP32 xác nhận. Người không có mật khẩu chỉ xem được."*
5. Chuyển lại **Tự động**.

## Clip 3 — Chế độ tay không vượt rào an toàn: nhấc phao trên (1 phút)

**Chứng minh:** chống tràn; chế độ tay không bỏ qua được an toàn; có ít nhất một lỗi được phát hiện.

1. Chuyển **Thủ công**, bấm **Bật bơm**.
2. Lúc bơm đang chạy, **nhấc phao trên lên**. Bơm phải **tắt ngay**, dashboard báo đỏ `OVERFLOW`.
3. Vẫn giữ phao, bấm **Xóa lỗi**: phải **bị từ chối** vì điều kiện tràn vẫn còn.
4. Bấm **Bật bơm**: phải **bị từ chối**.
5. Thả phao, bấm **Xóa lỗi**: lần này được chấp nhận. Chuyển lại **Tự động**.
6. Lời dẫn: *"Dù đang điều khiển tay, rào an toàn vẫn chạy mỗi 200 ms trên ESP32."*

## Clip 4 — Bơm chạy mà không có nước lên: chẩn đoán sức khỏe bơm (2 phút)

**Chứng minh:** luật lỗi "bơm chạy mà gần như không có dòng chảy" mà đề nêu; phần nâng cao "dùng dòng điện và lưu lượng để phân loại lỗi bơm".

1. Đợi một lần bơm tự động, hoặc bấm Bật bơm ở chế độ Thủ công.
2. **Nhấc đầu hút của bơm ra khỏi nước** và giữ nguyên.
3. Quay ô dòng điện: dòng **vẫn còn**, nhưng sau khoảng 25 s nhãn chuyển thành **"bơm chạy nhưng mực nước không lên"**.
4. Giữ tới khi bơm **tự tắt** và báo `NO_PROGRESS`, khoảng 90 s; đoạn chờ có thể tua nhanh.
5. Thả đầu hút về lại, bấm **Xóa lỗi**, chuyển **Tự động**.
6. Lời dẫn: *"Có dòng điện mà mực nước không lên nghĩa là lỗi phía thủy lực: đầu hút khô, ống tắc hoặc tuột."*

## Clip 5 — Mất điện bơm: phân biệt lỗi điện với lỗi nước (1 phút)

**Chứng minh:** phân loại lỗi bơm theo dòng điện (đối chiếu với clip 4).

1. Trong một lần bơm, **rút một dây nguồn của bơm**.
2. Sau khoảng 2 s: bơm bị khóa, dashboard báo `NO_CURRENT`, nhãn **"không có dòng · lỗi điện"**.
3. Cắm dây lại, bấm **Xóa lỗi**, chuyển **Tự động**.
4. Lời dẫn: *"Rơ-le đóng mà không có dòng điện nghĩa là lỗi phía điện: đứt dây, hỏng rơ-le hoặc hỏng động cơ."*

## Clip 6 — Cảm biến mức hỏng thì hệ thống an toàn (2–3 phút)

**Chứng minh:** xử lý an toàn khi cảm biến hỏng hoặc đọc vô lý.

1. **Rút dây ECHO** của cảm biến siêu âm.
2. Dashboard đánh dấu mức nước "không tin được"; sau khoảng 90 s báo `SENSOR_TIMEOUT` và bơm bị khóa. Đoạn chờ có thể tua nhanh.
3. **Cắm dây lại**: vài giây sau lỗi **tự xóa**, hệ thống trở về bình thường.
4. Lời dẫn: *"Mất cảm biến thì không bơm mù. Có lại tín hiệu thì tự hồi phục."*

## Clip 7 — Mất mạng mà bồn vẫn tự điều khiển, dữ liệu không mất (3 phút)

**Chứng minh:** điều khiển tại chỗ khi mất mạng, gửi thông báo khả dụng (last will), và đồng bộ lại khi có mạng.

1. Đợi bơm **đang chạy** ở chế độ tự động, mức khoảng 40–55 %.
2. Lời dẫn: *"Giờ tôi tắt Wi-Fi của thiết bị trong lúc bơm đang chạy."* Rồi **tắt điểm phát Wi-Fi** trên điện thoại.
3. Quay dashboard trên laptop: sau khoảng 5 s báo **mất kết nối**, chấm chuyển đỏ.
4. Quay **bồn**: bơm vẫn chạy và **tự tắt ở khoảng 70 %** dù không có mạng. Đây là cảnh quan trọng nhất.
5. **Bật lại điểm phát.** Trong khoảng 20 s thiết bị trở lại trực tuyến, và biểu đồ được **lấp kín khoảng trống** bằng dữ liệu đã đệm.
6. Lời dẫn: *"Toàn bộ luật điều khiển và an toàn chạy trên ESP32. Mất mạng chỉ mất phần giám sát từ xa."*

Laptop cũng đang dùng điểm phát này, nên khi tắt thì laptop mất mạng theo. Không sao: broker và backend chạy ngay trên laptop nên vẫn hoạt động. Đường link công khai trên điện thoại sẽ tạm mất trong lúc đó.

---

## Sau khi quay
Ghép thành **video dự phòng 10–12 phút** theo thứ tự clip 1 → 7, có chữ chú thích tên yêu cầu ở đầu mỗi clip. Đề chỉ bắt buộc ít nhất một lỗi được phát hiện, nên nếu thiếu thời gian thì giữ clip 3 và bỏ bớt clip 4–6.

| Clip | Yêu cầu của đề được chứng minh |
|---|---|
| 1 | Đo mức và lưu lượng; điều khiển tự động có dải trễ; dashboard cập nhật khi bơm và khi xả |
| 2 | Lệnh có xác nhận; giao diện chỉ hiện trạng thái đã xác nhận; phân quyền xem và điều khiển |
| 3 | Chống tràn; chế độ tay không vượt được an toàn; phát hiện lỗi |
| 4 | Luật "bơm chạy mà gần như không có dòng chảy"; phần nâng cao phân loại lỗi bơm |
| 5 | Phân loại lỗi bơm theo dòng điện |
| 6 | An toàn khi cảm biến hỏng |
| 7 | Điều khiển tại chỗ khi mất mạng; thông báo khả dụng; đồng bộ lại khi có mạng |
