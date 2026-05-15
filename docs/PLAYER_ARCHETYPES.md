# HỒ SƠ TÂM LÝ VÀ PHÂN LOẠI AI (PLAYER ARCHETYPES DEEP DIVE)

Tài liệu này phân tích chi tiết các "cá tính" AI được mô phỏng trong hệ thống và cách các tham số toán học tạo ra sự khác biệt trong lối chơi.

---

## 1. Triết lý Thiết kế: Archetype là gì?

Trong dự án này, Archetype không phải là một tập hợp các quy tắc `If-Else` cứng nhắc, mà là một **Bộ tham số cấu hình (Configuration Set)** cho hàm quyết định xác suất. Điều này cho phép chúng ta tạo ra vô số biến thể người chơi chỉ bằng cách thay đổi các con số trong `config.ini`.

## 2. Phân tích 3 Archetype mẫu (Pre-defined Profiles)

### 2.1. SHARK (Cá mập) - Kẻ săn mồi lý trí
- **Kỹ năng (`skill`):** 0.8 - 1.0 (Rất cao).
- **Độ tự tin (`confidence`):** Khởi tạo quanh mức **0.2** (Lý trí, không thái quá).
- **Độ nhạy cảm (`k`):** 2.5 (Phản ứng rất gắt với sự thay đổi điểm số).
- **Độ ngại rủi ro (`gamma`):** 2.2 (Tiêu chuẩn hài lòng cao).
- **Trạng thái TILT:** Khi bị TILT, tự tin vọt lên **+0.3** (đạt khoảng 0.5 - 0.7), kỹ năng giảm 10%.
- **Chiến thuật:** Shark chơi cực kỳ kỷ luật. Nó biết rằng bài 8-9 điểm là rất tốt, nhưng có khả năng đặc biệt là **"Săn Ba Tiên"** (ném bài 9 điểm để cầu bài hình nếu đã có 2 lá Tây). Shark đại diện cho nhóm người chơi chuyên nghiệp, tối ưu hóa lợi nhuận kỳ vọng (EV) nhưng đôi khi mắc bẫy "quá thông minh" dẫn đến rủi ro lớn.

### 2.2. MANIAC (Kẻ điên) - Con thiêu thân rủi ro
- **Kỹ năng (`skill`):** 0.3 - 0.6 (Trung bình kém).
- **Độ nhạy cảm (`k`):** 1.0 (Lỳ lợm, không quá quan tâm đến điểm số).
- **Độ tham lam (`greed`):** Cao.
- **Chiến thuật:** Maniac thích cảm giác mạnh. Nó thường đổi bài liên tục ngay cả khi đang cầm điểm trung bình (5-6 điểm). Hành động của Maniac mang tính chất bộc phát và khó đoán, thường gây nhiễu cho các phân tích xác suất thông thường.

### 2.3. NIT (Kẻ nhát) - Người chơi bảo thủ
- **Kỹ năng (`skill`):** 0.4 - 0.7 (Trung bình).
- **Độ ngại rủi ro (`gamma`):** 3.0 (Cực cao).
- **Tự tin (`confidence`):** Thường thấp hoặc âm.
- **Chiến thuật:** Nit cực kỳ sợ thua. Nó thường "Dằn" bài ngay khi có điểm số an toàn và hiếm khi thực hiện các lượt đổi bài mạo hiểm. Nit đại diện cho nhóm người chơi ưu tiên bảo toàn vốn hơn là tìm kiếm lợi nhuận lớn.

---

## 3. Bản đồ Tham số (Parameter Mapping)

Hệ thống cho phép bạn tự định nghĩa Archetype mới bằng cách hiểu rõ ý nghĩa của từng tham số:

| Tham số | Ảnh hưởng đến hành vi |
| :--- | :--- |
| **`k`** | Kiểm soát "độ gắt" của cảm xúc. `k` cao làm AI trở nên quyết đoán, `k` thấp làm AI trở nên phân vân. |
| **`gamma`** | Dịch chuyển điểm bão hòa của độ thỏa mãn. `gamma` cao buộc AI phải có điểm cao mới thấy "đủ". |
| **`greed_threshold`** | Điều khiển tốc độ tích lũy của `Trade Desire`. Chỉ số này càng cao, AI càng "nóng lòng" muốn đổi bài qua mỗi lượt. |
| **`min/max_skill`** | Xác định "trí thông minh" của AI. AI thông minh sẽ biết ưu tiên xác suất thắng thay vì cảm xúc từ hàm Sigmoid. |

---

## 4. Tương tác Quần thể (Population Dynamics)

Khi bạn trộn lẫn nhiều Archetype trong một ván đấu, hệ thống sẽ nảy sinh các hiện tượng thú vị:
- **Sự dịch chuyển tài sản**: Tiền thường có xu hướng chảy từ Maniac sang Nit và Shark.
- **Áp lực lên Nhà cái**: Nếu nhà cái là một Nit đối đầu với một bàn toàn Shark, nhà cái sẽ nhanh chóng bị "bào" sạch tiền do không dám mạo hiểm trong những ván bài quyết định.

**Ghi chú:** Toàn bộ lịch sử hành vi của các Archetype này được ghi lại chi tiết trong file `_swap_decisions.csv` để phục vụ việc vẽ biểu đồ và phân tích tâm lý học hành vi.
