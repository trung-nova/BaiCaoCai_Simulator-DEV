# Bài Cào Cái Simulator - Research & Analysis Guide

Chào ông bạn! Repo này chứa mã nguồn và dữ liệu mô phỏng cho trò chơi Bài Cào Cái (phiên bản có luật đổi bài). Hệ thống sử dụng mô hình Monte Carlo để giả lập hàng triệu ván đấu nhằm phân tích tâm lý hành vi của AI.

## 1. Cấu trúc Dữ liệu (Data Structure)

Dữ liệu được xuất ra dưới hai định dạng chính: **SQLite (.db)** để truy vấn sâu và **CSV** để nạp nhanh vào Excel/Pandas/PowerBI.

### A. SQLite Database (`*_simulation.db`)
Cấu trúc gồm 2 bảng chính:

#### Bảng `rounds` (Tổng quan ván đấu)
- `id`: Số thứ tự ván đấu.
- `dealer`: Tên người chơi đang làm Cái.
- `pot`: Tổng tiền cược trên bàn.
- `winners`: Số lượng người thắng trong ván đó.
- `scores`: Chuỗi tóm tắt điểm số của tất cả người chơi.

#### Bảng `swaps` (Chi tiết quyết định của AI)
Đây là bảng quan trọng nhất để phân tích hành vi:
- `round_id`: Link tới bảng `rounds`.
- `player`: Tên AI thực hiện quyết định.
- `turn`: Lượt đổi bài (1, 2 hoặc 3).
- `satisfaction`: Chỉ số hài lòng (0.0 - 1.0). Càng thấp AI càng muốn đổi bài.
- `desire`: Chỉ số thèm muốn đổi bài (tăng dần theo thời gian/cảm xúc).
- `probability`: Xác suất cuối cùng AI quyết định đổi bài (sau khi kết hợp Skill và Tâm lý).
- `swapped`: `1` nếu AI đã thực hiện đổi, `0` nếu giữ nguyên bài.

### B. Các file CSV
- `*_bankroll_history.csv`: Biến động số dư tài khoản của từng người chơi qua từng ván. Rất tốt để vẽ biểu đồ đường (Line Chart) thể hiện ROI.
- `*_swap_decisions.csv`: Dữ liệu thô tương tự bảng `swaps` nhưng ở dạng văn bản.
- `*_simulation_summary.txt`: Báo cáo tổng kết hiệu suất (Win Rate, Net Profit) của từng người chơi/Archetype.

---

## 2. Các nhóm AI (Archetypes)

Khi phân tích, ông cần chú ý đến 3 loại "cá tính" AI được định nghĩa trong `config.ini`:

1.  **SHARK (Cá mập):** Kỹ năng cao, lý trí, ít khi đổi bài bừa bãi.
2.  **MANIAC (Kẻ điên):** Kỹ năng thấp, rất hay đổi bài, dễ rơi vào trạng thái "TILT" (cay cú).
3.  **NIT (Kẻ nhát):** Rất thận trọng, chỉ giữ những bộ bài cực kỳ an toàn.

---

## 3. Gợi ý hướng phân tích (Research Ideas)

Nếu chưa biết bắt đầu từ đâu, ông có thể thử:
- **Biểu đồ ROI:** Vẽ `balance` theo thời gian của Shark vs Maniac để thấy sự khác biệt về đường cong lợi nhuận.
- **Heatmap:** Tương quan giữa `satisfaction` và xác suất thắng ván đó.
- **Phân tích TILT:** Xem xác suất đổi bài thay đổi như thế nào sau khi một AI thua 5 ván liên tiếp (dựa trên log trong `_tilt_events.txt`).

Chúc ông bạn phân tích vui vẻ! Có gì thắc mắc cứ hú tôi.
