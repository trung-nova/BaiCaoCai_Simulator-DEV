# TỪ ĐIỂN DỮ LIỆU (DATA DICTIONARY)

Tài liệu này định nghĩa chi tiết các trường dữ liệu được sinh ra trong quá trình mô phỏng, giúp việc phân tích dữ liệu (Data Analysis) trở nên dễ dàng và chính xác.

---

## 1. Các tệp báo cáo chính (CSV - Thư mục `data/`)

Hệ thống xuất ra 4 file CSV chính cho mỗi phiên mô phỏng. Nếu chạy ở chế độ **Random/Batch Mode**, các file sẽ có thêm cột **`Batch`** ở đầu.

### 1.1. `*_rounds_summary.csv` (Tổng quan ván đấu)
Lưu kết quả cuối cùng của từng ván.
- **RoundNum**: Số thứ tự ván.
- **Dealer**: Tên người làm Nhà Cái.
- **Pot**: Tổng tiền cược.
- **WinnersCount**: Số người thắng cái.
- **ScoresSummary**: Chuỗi điểm số toàn bộ bàn chơi (Ví dụ: `"Dealer:5 AI_1:9..."`).

### 1.2. `*_swap_decisions.csv` (Hành vi AI)
Ghi lại mọi lượt quyết định đổi bài. Đây là dữ liệu quan trọng nhất để phân tích tâm lý.
- **RoundID**: ID ván đấu.
- **PlayerName**: Tên AI thực hiện hành động.
- **SwapTurn**: Lượt đổi (1, 2, hoặc 3).
- **Satisfaction**: Độ thỏa mãn ($S$) tại thời điểm đó (0-1).
- **Desire**: Chỉ số thôi thúc muốn đổi bài ($D$).
- **Probability**: Xác suất quyết định cuối cùng ($P_{final}$).
- **Decision**: `0`: SKIP, `1`: TRADE, `2`: STAY.
- **ScoreBefore/After**: Điểm số trước và sau khi đổi.
- **CardOut/In**: Lá bài vứt đi và nhận về.

### 1.3. `*_ai_configs.csv` (Hồ sơ AI)
Lưu thông số khởi tạo của từng AI trong phiên chạy.
- **PlayerName**: Tên định danh.
- **Archetype**: Loại cá tính (ví dụ: SHARK, MANIAC, ...).
- **Skill**: Chỉ số kỹ năng (0-1).
- **Confidence**: Chỉ số tự tin ban đầu.
- **InitialBalance**: Số tiền vốn khởi điểm.

### 1.4. `*_bankroll_history.csv` (Biến động tài sản)
Theo dõi số dư tài khoản qua từng ván.
- **Round**: Số thứ tự ván.
- **[PlayerNames...]**: Cột riêng cho từng người chơi chứa số dư hiện tại.

---

## 2. Báo cáo tổng kết (`*_summary.txt`)

Một tệp văn bản tóm tắt các thông số quan trọng:
- **Timestamp**: Thời gian chạy.
- **Parameters**: Các tham số đầu vào (Seed, Archetype Distribution).
- **Final Results**: Bảng thống kê số dư cuối cùng của tất cả người chơi.

---

## 3. Quy tắc trò chơi (Game Logic Rules)

Dữ liệu được sinh ra dựa trên các quy tắc ngầm định sau:
- **Ante**: 1 đơn vị (Cược cơ sở).
- **Starting Balance**: 10,000 đơn vị (Mặc định).
- **House Edge**: Nhà cái thắng nếu hòa điểm với người chơi.
- **Ba Tiên**: 3 lá hình (J, Q, K) là bộ bài mạnh tuyệt đối (10 điểm).
