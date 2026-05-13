# TỪ ĐIỂN DỮ LIỆU (DATA DICTIONARY)

Tài liệu này định nghĩa chi tiết các trường dữ liệu được sinh ra trong quá trình mô phỏng, giúp các chuyên gia dữ liệu (Data Scientists) hiểu rõ ý nghĩa vật lý của các con số.

---

## 1. SQLite Database: `simulation_results.db`

Cơ sở dữ liệu tập trung lưu trữ mọi diễn biến chi tiết.

### 1.1. Bảng `rounds` (Tổng quan ván đấu)
Lưu lại "kết quả cuối cùng" của mỗi ván.

| Trường | Kiểu dữ liệu | Ý nghĩa |
| :--- | :--- | :--- |
| `id` | INTEGER (PK) | ID duy nhất của ván đấu. |
| `dealer` | TEXT | Tên của người chơi làm Nhà Cái trong ván đó. |
| `pot` | INTEGER | Tổng số tiền cược trong ván (Số lượng Nhà Con * Ante). |
| `winners` | INTEGER | Số lượng người chơi thắng Nhà Cái. |
| `scores` | TEXT | Chuỗi tóm tắt điểm số (Ví dụ: `D:8, P1:7, P2:9`). |

### 1.2. Bảng `swaps` (Chi tiết hành vi)
Lưu lại "quá trình ra quyết định" của AI. Đây là bảng quan trọng nhất để phân tích tâm lý.

| Trường | Kiểu dữ liệu | Ý nghĩa |
| :--- | :--- | :--- |
| `round_id` | INTEGER | Liên kết với bảng `rounds`. |
| `player` | TEXT | Tên người chơi thực hiện hành động. |
| `turn` | INTEGER | Thứ tự lượt đổi bài (1, 2, hoặc 3). |
| `satisfaction`| FLOAT (0-1) | Mức độ hài lòng của AI với bài hiện tại ($S$). |
| `desire` | FLOAT | Chỉ số thôi thúc muốn đổi bài ($D$). |
| `probability` | FLOAT (0-1) | Xác suất thực tế để AI quyết định đổi ($P_{final}$). |
| `swapped` | INTEGER | Trạng thái: `0`: SKIP (Bỏ lượt), `1`: TRADE (Đổi bài), `2`: STAY (Dằn). |
| `score_before`| INTEGER | Điểm số của người chơi ngay trước khi đổi. |
| `score_after` | INTEGER | Điểm số của người chơi ngay sau khi đổi (hoặc giữ nguyên). |
| `card_out` | TEXT | Tên lá bài đã vứt đi (ví dụ: "Hearts 7"). |
| `card_in` | TEXT | Tên lá bài đã nhận về (ví dụ: "Clubs King"). |

---

## 2. Các tệp CSV (Thư mục `data/`)

Dùng cho việc xử lý nhanh bằng thư viện Pandas hoặc Excel. Khi chạy ở **Mode 3 (Big Data Batch)**, các tệp CSV sẽ có thêm cột `Batch` ở đầu để phân biệt các đợt chạy khác nhau.

### 2.1. `*_bankroll_history.csv`
Theo dõi biến động tài sản theo thời gian.

- **Cấu trúc**: `Batch (Optional), Round, Player1_Balance, Player2_Balance, ...`
- **Mục đích**: Tính toán ROI, Drawdown và vẽ biểu đồ tăng trưởng.

### 2.2. `*_swap_decisions.csv`
Phiên bản text của bảng `swaps` trong SQLite.

- **Cấu trúc**: `Batch (Optional), RoundID, PlayerName, SwapTurn, Satisfaction, Desire, Probability, Decision, ScoreBefore, ScoreAfter, CardOut, CardIn`

### 2.3. `*_research_summary.txt`
Báo cáo tổng kết sau khi kết thúc đợt mô phỏng.

- **Thông số chính**:
    - **Total Rounds**: Tổng số ván đã chạy.
    - **Win Rate**: Tỉ lệ thắng của từng cá tính (Shark/Maniac/Nit).
    - **Average ROI**: Lợi nhuận trên vốn đầu tư trung bình.
    - **Simulation Seed**: Hạt giống ngẫu nhiên dùng để tái lập kết quả.

---

## 3. Các hằng số mặc định (Default Constants)

- **Ante**: 1 chip (Mức cược cơ sở).
- **Starting Balance**: 10,000 chips (Vốn khởi điểm).
- **House Edge**: Hòa điểm $\rightarrow$ Nhà cái thắng.
