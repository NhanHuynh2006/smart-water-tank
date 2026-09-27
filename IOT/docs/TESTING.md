# Kiểm chứng theo đề bài — Project 08

Tài liệu này đi từng yêu cầu trong đề "Smart Water Tank, Consumption Monitoring and Automatic Pump Control", nói **làm gì để chứng minh**, **đọc số ở đâu**, và **hiện đã có bằng chứng chưa**. Cuối tài liệu là kịch bản demo trực tiếp 10–15 phút.

Ký hiệu trạng thái:

- ✅ đã có bằng chứng trong cơ sở dữ liệu hoặc đã kiểm trên bàn thí nghiệm
- 🔧 đã làm xong phần mềm, **còn phải đo lại trên phần cứng** trước khi nộp
- ⚠️ làm được một phần, phải ghi rõ trong mục Hạn chế của báo cáo

## 0. Chuẩn bị chung cho mọi phép thử

1. Laptop vào **điểm phát 2,4 GHz `NhanHuynh`**. ESP32 không thấy mạng 5 GHz, `ThanhNhan_5G` không dùng được.
2. Cắm ESP32, nạp bản mới nhất: `cd IOT && pio run -e main -t upload`, rồi mở `pio device monitor` để thấy `[WIFI] da noi` và `[MQTT] ... thanh cong`.
3. Chạy hệ thống: `./start.sh` ở thư mục gốc. Lệnh in ra mật khẩu quản trị và địa chỉ công khai.
4. Mọi công cụ đo nằm trong `IOT/tools/`, chạy từ `IOT/backend` để dùng chung môi trường Python:

   ```bash
   cd IOT/backend
   env -u PYTHONPATH .venv/bin/python ../tools/bench.py level     # E1
   env -u PYTHONPATH .venv/bin/python ../tools/bench.py fault     # E5
   env -u PYTHONPATH .venv/bin/python ../tools/bench.py outage    # E6
   env -u PYTHONPATH .venv/bin/python ../tools/analyze_experiments.py   # E2, E3, E4, E7 từ dữ liệu có sẵn
   ```

   `env -u PYTHONPATH` là bắt buộc, nếu không thư viện của ROS sẽ che mất thư viện trong venv.

   Mỗi lần chạy `bench.py` ghi kết quả vào `IOT/docs/bench_<loại>_<giờ>.json`. Đưa các file này vào báo cáo.

## 1. Bảng yêu cầu → cách chứng minh

| # | Yêu cầu của đề | Cách chứng minh | Trạng thái |
|---|---|---|---|
| R1 | ESP32, cảm biến mức và cảm biến lưu lượng (bắt buộc) | Ảnh mạch, sơ đồ đấu dây `docs/wiring.md`, telemetry có `level_cm`, `flow_lpm`, `flow_out_lpm` | ⚠️ Cảm biến mức chạy tốt. Hai YF-S401 **đã lắp** nhưng bị nhiễu dẫn ~1500 Hz khi bơm chạy và dòng ra 0,09–0,15 L/phút thấp hơn ngưỡng đo 0,3 L/phút của cảm biến. Firmware đang tính lưu lượng từ mức nước (`FLOW_FROM_LEVEL=1`). Báo cáo phải nói thẳng điều này và trình bày số đo nhiễu |
| R2 | Bơm điện áp thấp qua rơ-le/MOSFET có bảo vệ cảm ứng | Ảnh module rơ-le, diode chống ngược song song bơm | ✅ |
| R3 | Chống tràn, an toàn khi cảm biến hỏng hoặc số đo vô lý | E5: nhấc phao trên → `OVERFLOW`; rút dây Echo → `SENSOR_TIMEOUT`; ba đường chống tràn độc lập (phao, % mức, khoảng cách thô) | 🔧 luật đã chạy trong DB, đo lại độ trễ trên bản mới |
| R4 | Lưu lượng tức thời và thể tích cộng dồn | Dashboard ô "Dòng vào/Dòng ra", `volume_l`, biểu đồ **Thể tích theo ngày** | ✅ E2 bên dưới: sai số thể tích −5 % trung bình |
| R5 | MQTT: telemetry, trạng thái cơ cấu chấp hành, sự cố, khả dụng | `docs/protocol.md`; `mosquitto_sub -u backend -P backend_pass -t 'wt/#' -v` cho thấy đủ 6 chủ đề, `status` giữ lại + di chúc (LWT) | ✅ |
| R6 | Lưu chuỗi thời gian, REST API, dashboard (mức, lưu lượng, thể tích ngày, bơm, cảnh báo, độ tươi) | SQLite `backend/watertank.db`, `/docs` của FastAPI, dashboard có dòng thời gian "N s trước" cạnh đèn kết nối | ✅ |
| R7 | Điều khiển tự động trễ/máy trạng thái + ít nhất một luật lỗi | Ngưỡng 30 %/70 %, máy 7 trạng thái; 11 luật lỗi | ✅ E3, E4 |
| R8 | Chế độ tay không được vượt rào an toàn | Chế độ tay, bật bơm, rồi nhấc phao trên → bơm ngắt ngay trong 200 ms, lỗi `OVERFLOW`; bấm Bật bơm lại → từ chối `fault_active`; bấm Xóa lỗi khi phao còn nhấc → từ chối `condition_still_present` | 🔧 làm lại trước demo, chụp màn hình ô báo lỗi |
| R9 | Lệnh có xác nhận, giao diện chỉ hiện trạng thái đã xác nhận | Dòng báo "Đang gửi lệnh…", trạng thái chỉ đổi theo `cmd/ack`; bảng `commands` có `rtt_ms` | ✅ 75 lệnh, trung vị 42 ms |
| R10 | Mất mạng vẫn điều khiển mức tại chỗ | E6: cắt mạng 90 s trong lúc bơm, xem mẫu phát lại có chuyển FILLING→IDLE | 🔧 đo lại trên bản mới (có thêm `ctl_gap_ms`) |
| R11 | Nâng cao: rò rỉ/tiêu thụ bất thường theo đường nền trượt **hoặc** nhất quán dòng điện–lưu lượng để phân loại lỗi bơm | Cả hai đã làm. Đường nền 48 khe × 30 phút (`/api/leak/baseline`); phân loại `pump_health` = `ok / no_current / no_flow` hiện trên dashboard và trong mỗi bản tin sự cố | ⚠️ đường nền cần ≥ 3 ngày dữ liệu mỗi khe mới cảnh báo; phần demo được là **phân loại lỗi bơm** |
| R12 | Hiệu chuẩn mức và lưu lượng với chuẩn vật lý | E1 thước kẻ, E2 cân bằng thể tích | 🔧 E1 phải đo |
| R13 | Đáp ứng điều khiển, tần suất đóng cắt, độ trễ phát hiện lỗi, sai số thể tích | E3, E4, E5, E2 | ✅ E2–E4, 🔧 E5 |
| R14 | Thử lỗi cảm biến / không có dòng, thử mất và có lại mạng, đo độ trễ telemetry/điều khiển | E5, E6, E7 | ✅ E7, 🔧 E5, E6 |

## 2. Các thí nghiệm

### E1 — Hiệu chuẩn mức nước (🔧 cần đo)

Chuẩn vật lý: thước kẻ dựng đứng trong bồn, số 0 chạm đáy.

1. Chuyển **Thủ công** trên dashboard để bơm không tự chạy.
2. `bench.py level`. Đổ nước tới khoảng 2, 4, 6, 8, 10, 12 cm (ít nhất 5 mức). Mỗi mức: chờ mặt nước lặng, đọc thước, gõ số vào, công cụ lấy trung bình 20 s.
3. Kết quả: sai số từng điểm, RMSE, sai lớn nhất, và hệ số `LEVEL_CAL_A/B`. Nếu RMSE > 0,3 cm thì chép A/B vào `include/config.h`, nạp lại, đo lại một lượt để chứng minh đã giảm.

Bảng đưa vào báo cáo: `mức thước | thiết bị | độ lệch chuẩn | sai số`.

### E2 — Lưu lượng và sai số thể tích (✅ có số)

Chuẩn vật lý: bồn có đáy 10 × 10 cm, nên 1 cm mức = 0,1 L. Trong mỗi lần bơm tự động:

    V_chuẩn = Δh × 100 cm² + tốc độ xả × thời gian bơm

Tốc độ xả đo **độc lập** bằng độ dốc mức nước lúc bơm tắt, 90 s trước và sau lần bơm. `analyze_experiments.py` đã làm việc này trên 6 chu kỳ trọn vẹn ngày 23/09:

| | trung bình | nhỏ nhất | lớn nhất |
|---|---|---|---|
| Sai số thể tích bơm | −5,2 % | −15,2 % | +8,2 % |
| Tốc độ xả đo được | 0,16 L/phút | 0,11 | 0,20 |

Phải nói trong báo cáo: sai số này là của **hằng số bơm 0,36 L/phút**, không phải của YF-S401. Để đo thêm bằng bình đong: tháo ống ra khỏi bồn, cho vào ca có vạch, bơm thủ công đúng 60 s, so thể tích trong ca với 0,36 L.

### E3 — Đáp ứng điều khiển (✅ có số)

Nguồn: `analyze_experiments.py`, 6 chu kỳ tự động trọn vẹn:

- Bơm bật khi mức < 30 %, ngắt ở **70,1–70,8 %** (đặt 70 %).
- Vọt lố sau khi ngắt **+1,1 … +1,85 %** (nước còn trong ống).
- Nạp 30 → 70 % mất **144–160 s**, tốc độ dâng ≈ 0,036 cm/s.

### E4 — Tần suất đóng cắt (✅ có số)

84 phút: 18 lần bật (12,9 lần/giờ), khoảng nghỉ ngắn nhất 32 s, lớn hơn `MIN_OFF` 20 s. Tỉ lệ mẫu mức hợp lệ 99,5 %, không có sự cố giả.

### E5 — Phát hiện lỗi (🔧 cần đo trên bản mới)

Mỗi phép: chạy `bench.py fault`, nhấn Enter **đúng lúc** tác động, công cụ in mã lỗi, độ trễ, `pump_health`, và thời điểm bơm thực sự ngắt. Làm mỗi phép 3 lần, lấy trung bình.

| Tác động | Mã mong đợi | Độ trễ thiết kế | Ghi chú |
|---|---|---|---|
| Nhấc phao trên lên (bồn đang bơm) | `OVERFLOW` | < 1 s (1 chu kỳ 200 ms) | Sau đó bấm "Xóa lỗi" phải bị từ chối khi phao còn nhấc |
| Rút dây Echo của HC-SR04 | `SENSOR_TIMEOUT` | ≈ 45 s | Tự hồi phục sau 2,5 s khi cắm lại |
| Nhấc đầu hút bơm ra khỏi nước (chế độ tay) | `pump_health = no_flow` sau 25 s, rồi `NO_PROGRESS` | 25 s / 90 s | **Đây là luật "bơm chạy mà không có dòng" của đề**. Dòng điện vẫn có, mức không lên |
| Rút một dây nguồn của bơm (rơ-le vẫn đóng) | `NO_CURRENT`, `pump_health = no_current` | ≈ 2 s | Phân biệt được với trường hợp trên nhờ dòng điện |
| Để bơm chạy tay quá lâu | `FILL_TIMEOUT` | 240 s | Tùy chọn |

Dữ liệu cũ trong DB (cấu hình trước) cho `SENSOR_TIMEOUT`: 1–5 s với ngưỡng cũ và ≈ 40 s với ngưỡng 45 s hiện tại.

### E6 — Mất mạng và có lại mạng (🔧 cần đo trên bản mới)

Mục tiêu đề bài: **bồn vẫn tự điều khiển khi mất mạng**, và dữ liệu không mất.

Cách A, giống thực tế nhất: tắt điểm phát `NhanHuynh` trên điện thoại.
Cách B, lặp lại được chính xác: laptop chặn IP của ESP32 (xem IP trên màn hình serial).

1. Xả nước cho mức xuống dưới 30 % để bơm **đang chạy** trước khi cắt.
2. Chạy:

   ```bash
   # cách B: công cụ tự chặn 90 s rồi tự mở (cần sudo)
   env -u PYTHONPATH .venv/bin/python ../tools/bench.py outage --ip <IP_ESP32> --seconds 90
   # cách A: công cụ nhắc bạn tắt/bật điểm phát và nhấn Enter
   env -u PYTHONPATH .venv/bin/python ../tools/bench.py outage
   ```

3. Công cụ in ra:
   - `lwt_after_cut_s`: broker phát hiện mất thiết bị sau bao lâu (thiết kế ≈ 1,5 × keepalive 15 s ≈ 22 s). Dashboard đổi sang "mất kết nối" sau 5 s không có dữ liệu.
   - `pump_transitions_while_offline`: **bằng chứng chính** — ví dụ `FILLING->IDLE pump=False @ 70.3%`, nghĩa là thiết bị tự ngắt bơm ở 70 % khi không có mạng.
   - `replayed`, `missing_seq`: số mẫu phát lại từ bộ đệm vòng 240 mẫu (4 phút) và số mẫu mất.
   - `ctl_gap_ms_during_outage`: chu kỳ điều khiển dài nhất trong lúc mất mạng. Bình thường ~200 ms; bản mới giới hạn mỗi lần thử kết nối broker ở 0,5 s thay vì 3 s mặc định.
   - `online_after_restore_s`: thời gian nối lại.

Bảng đưa vào báo cáo: `lần | thời gian mất | LWT | nối lại | phát lại | mất | chuyển trạng thái khi offline | ctl_gap`.

### E7 — Độ trễ (✅ có số)

- **Khứ hồi lệnh** (dashboard → backend → broker → ESP32 → ack → backend), đo bằng một đồng hồ: n = 75, trung vị **42 ms**, p95 138 ms, lớn nhất 182 ms. Đây là số chính.
- Telemetry một chiều: trung vị 167 ms, p95 238 ms. Hai đồng hồ khác nhau (NTP), chỉ để tham khảo.
- Xem trực tiếp: `/api/stats/latency?minutes=10`.

### E8 — Thể tích theo ngày và rò rỉ (✅ / ⚠️)

- Dashboard thẻ **Thể tích theo ngày**, API `/api/volume/daily`.
- `/api/leak/baseline` cho biết bao nhiêu khe 30 phút đã đủ 3 ngày dữ liệu. Hiện chưa đủ để cảnh báo — ghi vào Hạn chế, hoặc để hệ thống chạy liên tục vài ngày trước buổi bảo vệ.

## 3. Kịch bản demo trực tiếp (10–15 phút)

Trước giờ demo: nạp firmware, chạy `./start.sh`, mở dashboard trên máy chiếu **và** trên điện thoại bằng 4G (chứng minh xem từ xa), đăng nhập quản trị trên máy chiếu, đặt về **Tự động**, mức nước khoảng 40 %.

1. **Tổng quan (1 phút).** Chỉ sơ đồ bồn, các ô số, dòng "1,0 s trước", biểu đồ theo ngày.
2. **Xả nước → tự bơm (3 phút).** Mở van xả. Mức giảm trên cả máy chiếu và điện thoại. Dưới 30 % bơm tự bật, ô bơm xanh, dòng vào 0,36 L/phút, `pump_health` = ok. Tới 70 % bơm tự ngắt.
3. **Lệnh có xác nhận (1 phút).** Chuyển Thủ công, bấm Bật bơm: dòng báo "Đang gửi lệnh…" rồi trạng thái bơm chỉ đổi khi thiết bị xác nhận. Bấm lại ngay sau khi tắt → bị từ chối "chờ N s" (bảo vệ đóng cắt). Mở điện thoại chưa đăng nhập: chỉ xem, không có nút.
4. **Chế độ tay không vượt rào (1 phút).** Vẫn ở Thủ công, bật bơm, rồi nhấc phao trên → bơm ngắt ngay, cảnh báo đỏ `OVERFLOW`. Bấm Bật bơm → bị từ chối. Bấm Xóa lỗi khi phao vẫn nhấc → bị từ chối vì điều kiện còn đó. Thả phao, Xóa lỗi → được.
5. **Một lỗi được phát hiện (2 phút).** Chọn một:
   - nhấc đầu hút bơm khỏi nước → sau 25 s ô dòng điện báo **"có dòng, không có nước · chạy khô"** (`no_flow`), sau 90 s `NO_PROGRESS` ngắt bơm; hoặc
   - rút dây Echo → `SENSOR_TIMEOUT`, cắm lại tự hồi phục.
   Bấm **Xóa lỗi**, chuyển lại Tự động.
6. **Mất mạng (3 phút).** Khi bơm đang chạy, tắt điểm phát. Dashboard báo mất kết nối, **nhưng bơm vẫn tự ngắt ở 70 %** (thấy bằng mắt trên bồn và đèn rơ-le). Bật lại điểm phát: dữ liệu phát lại điền kín khoảng trống trên biểu đồ.
7. **Câu hỏi.**

Kịch bản dự phòng: quay video toàn bộ các bước trên một lần trước buổi bảo vệ (đề yêu cầu có video demo dự phòng).

## 4. Danh mục bằng chứng phải nộp

- [ ] Nguyên mẫu + mã nguồn (repo GitHub)
- [ ] Báo cáo (`iot-report-en/main.pdf`) — còn điền tên, MSSV, bảng đóng góp
- [x] Slide tiếng Anh 30 trang (`slides-en/`, bản trình chiếu trên claude.ai) — còn điền tên nhóm
- [ ] Sơ đồ kiến trúc (`docs/architecture.html`) và sơ đồ đấu dây (`docs/wiring.md`)
- [ ] Đặc tả MQTT/API (`docs/protocol.md`, FastAPI `/docs`)
- [ ] Lược đồ CSDL (báo cáo chương 5)
- [ ] Ảnh chụp dashboard: bình thường, đang bơm, có lỗi, mất kết nối
- [ ] Bảng/biểu đồ E1–E8 (`docs/results.json`, `docs/bench_*.json`)
- [ ] Video demo dự phòng
