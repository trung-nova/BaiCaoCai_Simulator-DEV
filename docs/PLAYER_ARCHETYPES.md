# PHÂN LOẠI VÀ TÂM LÝ AI (PLAYER ARCHETYPES)

Tài liệu này dành cho việc phân tích dữ liệu, giải thích các tham số toán học điều khiển hành vi của AI.

## 1. Ba nhóm Cá tính chính (Archetypes)

Trong file `config.ini`, ông bạn sẽ thấy các thông số cho 3 nhóm AI:

| Nhóm | Đặc điểm hành vi | Thông số toán học |
| :--- | :--- | :--- |
| **SHARK (Cá mập)** | Chơi cực kỳ lý trí, kỹ năng cao, chỉ đổi bài khi xác suất thắng thực sự tăng lên. | `k` cao (nhạy cảm), `skill` (0.8-1.0) |
| **MANIAC (Kẻ điên)** | Thích rủi ro, rất hay đổi bài bất kể điểm số, dễ bị ảnh hưởng bởi tâm lý. | `k` thấp (lỳ lợm), `skill` (0.3-0.7) |
| **NIT (Kẻ nhát)** | Cực kỳ thận trọng, chỉ hài lòng với điểm số rất cao, sợ thua. | `gamma` cao, `confidence` thấp |

---

## 2. Mô hình Toán học điều khiển (The Math)

Hành vi AI được tính toán qua 3 bước:

### 2.1. Chỉ số Thỏa mãn (Sigmoid Satisfaction - S)
Hệ thống sử dụng hàm **Sigmoid** để mô phỏng cảm xúc của AI dựa trên điểm số (0-10).
> **S = 1 / (1 + exp(-k * (score - midpoint)))**

- **Midpoint**: Điểm mà tại đó AI bắt đầu thấy "tạm ổn" (thường là 5 điểm, nhưng bị điều chỉnh bởi `confidenceLevel`).
- **k**: Độ dốc của cảm xúc. `k` càng cao, AI càng "phân cực" (vui rất nhanh khi điểm cao và buồn rất nhanh khi điểm thấp).

### 2.2. Chỉ số Thèm muốn (Trade Desire - D)
Mô tả sự "nóng lòng" muốn đổi bài. Chỉ số này tăng dần qua các lượt đổi nếu AI vẫn chưa hài lòng.
- **Lượt 1**: `D = (1.0 - S) * aggression`
- **Lượt 2+**: `D` tăng thêm một lượng dựa trên sự không thỏa mãn và tính tham lam (`greed`).

### 2.3. Xác suất Quyết định (Final Probability - P)
Đây là con số cuối cùng dùng để Random:
> **P_final = (Skill * P_lý_trí) + ((1 - Skill) * P_tâm_lý)**

- **P_lý_trí**: Tỉ lệ nghịch với mức độ thỏa mãn (`1.0 - S`).
- **P_tâm_lý**: Tổng hợp từ sự thèm muốn (`D`) và các yếu tố bộc phát.
=> **Skill càng cao**, AI càng bỏ qua cảm xúc để ra quyết định dựa trên toán học thuần túy.

---

## 3. Cơ chế TILT (Mất bình tĩnh)

Đây là biến số "nhiễu" quan trọng nhất cho việc phân tích dữ liệu.
- **Điều kiện kích hoạt**:
  - Thua liên tiếp 5 ván.
  - HOẶC: Tài khoản giảm xuống dưới 70% so với ban đầu.
- **Hệ quả khi TILT**:
  - **Giảm Skill**: AI đưa ra quyết định sai lầm nhiều hơn.
  - **Tăng Confidence**: AI trở nên liều lĩnh vô căn cứ (Over-confident).
  - **Hành vi**: Đổi bài vô tội vạ, dẫn đến việc phá hỏng những bộ bài đang tốt.

*Dữ liệu về TILT có thể tìm thấy trong file `_tilt_events.txt` hoặc cột tâm lý trong database.*
