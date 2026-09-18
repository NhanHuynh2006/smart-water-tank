# Đặc tả giao thức

Tài liệu này được chốt trước khi viết mã, đóng vai trò hợp đồng nội bộ giữa ba nhánh phần sụn, dịch vụ nền và giao diện.

## Cây chủ đề

Gốc: `wt/{site}/{device_id}`. Triển khai hiện tại là `wt/lab1/esp32-01`.

| Chủ đề | Hướng | QoS | Lưu giữ | Luận cứ |
|---|---|---|---|---|
| `.../telemetry` | bắc | 0 | không | chuỗi liên tục 1 Hz, mất một mẫu không đổi kết luận về xu hướng |
| `.../state/pump` | bắc | 1 | có | chỉ phát khi đổi trạng thái, mất là giao diện sai tới lần đổi kế tiếp |
| `.../event/fault` | bắc | 1 | không | mất một cảnh báo là hành vi sai của hệ thống an toàn |
| `.../status` | bắc | 1 | có | bản tin di chúc chỉ phát một lần, không có cơ hội phát lại |
| `.../cmd` | nam | 1 | không | lệnh không tới nơi là lỗi chức năng trực tiếp |
| `.../cmd/ack` | bắc | 1 | không | xác nhận không về là lỗi chức năng trực tiếp |

Không chủ đề nào dùng QoS 2. Vấn đề nhận trùng bản tin đã được xử lý ở mức ứng dụng bằng mã định danh lệnh duy nhất, nên thiết bị chỉ gửi lại xác nhận cho cùng mã mà không thực hiện hành động hai lần.

## Bản tin dữ liệu đo

```json
{
  "dev": "esp32-01", "ts": 1735689600, "ts_ms": 1735689600123, "seq": 10421,
  "level_pct": 62.4, "level_cm": 15.6, "level_ok": true,
  "flow_lpm": 1.28, "volume_l": 143.62, "volume_today_l": 12.40,
  "flow_out_lpm": 0.42, "volume_out_l": 98.10, "volume_out_today_l": 8.75,
  "pump": true, "state": "FILLING", "mode": "AUTO",
  "current_mv": 38.2, "float_max": false, "float_src": true,
  "fault": "", "rssi": -55
}
```

- `seq` tăng đơn điệu, cho phép đếm chính xác số bản tin mất trong thí nghiệm E7.
- `ts` do thiết bị sinh sau khi đồng bộ thời gian mạng, **độ phân giải một giây**. Bằng 0 nghĩa là chưa đồng bộ, backend loại khỏi thống kê độ trễ.
- `ts_ms` cùng nguồn đồng hồ với `ts` nhưng **độ phân giải mili giây**. Bổ sung vì `ts` theo giây khiến hiệu `recv_ts - ts` mang sai số lượng tử hoá tới ±500 ms, lớn hơn cả độ trễ mạng thật nên nhấn chìm phép đo. Trường `ts` được giữ nguyên để không phá vỡ bên đọc cũ; backend ưu tiên `ts_ms` và chỉ lùi về `ts` khi thiết bị không gửi.
- Cả hai trường trên chỉ cho **độ trễ một chiều giữa hai đồng hồ khác nhau**, nên sai số bị chặn dưới bởi độ chính xác đồng bộ NTP của thiết bị, thường ±10 tới ±50 ms qua Wi-Fi. Số liệu chính của thí nghiệm E7 là **độ trễ khứ hồi của lệnh**, đo bằng một đồng hồ máy chủ duy nhất ở cả hai đầu nên không dính sai lệch đồng bộ.
- `flow_lpm` và `volume_l` là **cảm biến đầu vào**, đo nước bơm từ bồn nguồn lên bồn chính. `flow_out_lpm` và `volume_out_l` là **cảm biến đầu ra**, đo nước tiêu thụ. Hai cảm biến có hệ số K riêng vì có thể khác loại.
- `level_ok` cho biết phép đo có đáng tin không, để giao diện phân biệt bồn cạn thật với cảm biến hỏng.
- Bản tin phát lại từ bộ đệm sau khi khôi phục kết nối mang thêm `"replay": true` và bị loại khỏi mọi thống kê độ trễ.

## Bản tin lệnh và xác nhận

```json
// .../cmd
{"cmd_id":"a7f3c2e1", "action":"pump", "value":true, "ts":1735689600}

// .../cmd/ack
{"dev":"esp32-01", "cmd_id":"a7f3c2e1", "status":"rejected",
 "reason":"overflow_guard", "state":"OVERFLOW_LOCK",
 "pump":false, "mode":"MANUAL", "ts":1735689601}
```

Bốn hành động: `mode`, `pump`, `clear_fault`, `reset_volume`.

Mười mã lý do từ chối: `auto_mode_active`, `fault_active`, `overflow_guard`, `float_max_triggered`, `level_sensor_invalid`, `source_tank_empty`, `min_off_time`, `no_active_fault`, `condition_still_present`, `unknown_action`.

Xác nhận luôn mang trạng thái thực tế của thiết bị sau khi xử lý lệnh, nên giao diện hiển thị trạng thái đã được thiết bị xác nhận chứ không phải trạng thái người dùng vừa yêu cầu.

## Bản tin sự cố và khả dụng

```json
// .../event/fault
{"dev":"esp32-01", "code":"DRY_RUN", "ts":1735689600,
 "level_pct":42.1, "flow_lpm":0.02}

// .../status  (lưu giữ, và là nội dung di chúc)
{"online": true}
```

Thiết bị đăng ký trước bản tin di chúc `{"online": false}` khi kết nối, rồi ghi đè bằng `{"online": true}`. Nếu nút mất nguồn đột ngột, broker tự công bố di chúc.

Backend bổ sung lớp kiểm tra thứ hai dựa trên độ tươi dữ liệu: bản tin gần nhất cũ hơn 5 giây thì coi như mất kết nối. Cơ chế này bắt được cả trường hợp phiên còn sống nhưng luồng dữ liệu đã dừng.
