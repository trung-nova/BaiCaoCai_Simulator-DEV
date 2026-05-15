# HỒ SƠ TÂM LÝ VÀ PHÂN LOẠI AI (PLAYER ARCHETYPES DEEP DIVE)

Tài liệu này phân tích chi tiết các "cá tính" AI được mô phỏng trong hệ thống và cách các tham số toán học tạo ra sự khác biệt trong lối chơi.

---

## 1. Triết lý Thiết kế: Archetype là gì?

Trong dự án này, Archetype không phải là một tập hợp các quy tắc `If-Else` cứng nhắc, mà là một **Bộ tham số cấu hình (Configuration Set)** cho hàm quyết định xác suất. Điều này cho phép chúng ta tạo ra vô số biến thể người chơi chỉ bằng cách thay đổi các con số trong `config.ini`.

## 2. Phân tích 3 Archetype mẫu (Pre-defined Profiles)

### 2.1. SHARK (Cá mập) - Kẻ săn mồi lý trí
- **Kỹ năng (`skill`):** 0.75 - 0.95 (Vượt trội, nhưng vẫn có xác suất sai lầm nhỏ).
- **Độ tự tin (`mean_confidence`):** 0.2 (Điềm tĩnh, tự tin dựa trên năng lực).
- **Độ nhạy cảm (`k`):** 1.2.
- **Độ ngại rủi ro (`gamma`):** 2.5 (Tiêu chuẩn hài lòng cao).
- **Chiến thuật:** Shark chơi cực kỳ kỷ luật, tối ưu hóa lợi nhuận kỳ vọng (EV) và có khả năng "Săn Ba Tiên" cực tốt.

### 2.2. NIT (Kẻ nhát) - Người chơi bảo thủ
- **Kỹ năng (`skill`):** 0.45 - 0.75 (Trung bình khá).
- **Độ ngại rủi ro (`gamma`):** 3.0 (Cực cao).
- **Độ tự tin (`mean_confidence`):** -0.5 (Luôn lo sợ bài đối phương cao hơn).
- **Chiến thuật:** Nit ưu tiên bảo toàn vốn. Nó thường dừng lại (STAY) sớm khi cảm thấy an toàn, ít khi mạo hiểm trừ khi bài quá xấu.

### 2.3. MANIAC (Kẻ điên) - Con thiêu thân rủi ro
- **Kỹ năng (`skill`):** 0.20 - 0.50 (Thấp).
- **Độ tự tin (`mean_confidence`):** 0.8 (Ảo tưởng sức mạnh, cực kỳ hung hãn).
- **Độ nhạy cảm (`k`):** 0.8 (Lỳ lợm, bốc đồng).
- **Chiến thuật:** Maniac thích cảm giác mạnh và đổi bài liên tục. Với kỹ năng thấp, Maniac thường rơi vào trạng thái TILT và đốt sạch tiền nhanh nhất bàn.

### 2.4. FISH (Cá con) - Nguồn thanh khoản của bàn chơi
- **Kỹ năng (`skill`):** 0.05 - 0.30 (Rất thấp).
- **Độ tự tin (`mean_confidence`):** 0.4 (Chơi theo cảm tính, lạc quan tếu).
- **Độ tham lam (`greed`):** Rất cao (0.2).
- **Chiến thuật:** Fish là những người chơi không có chiến thuật rõ ràng, thường đổi bài dựa trên sự "linh cảm" thay vì xác suất. Đây là mục tiêu chính của các Shark.

---

## 3. Bản đồ Tham số (Parameter Mapping)

Hệ thống cho phép bạn tự định nghĩa Archetype mới bằng cách hiểu rõ ý nghĩa của từng tham số:

| Tham số | Ảnh hưởng đến hành vi |
| :--- | :--- |
| **`k`** | Kiểm soát "độ gắt" của cảm xúc. `k` cao làm AI trở nên quyết đoán, `k` thấp làm AI trở nên phân vân. |
| **`gamma`** | Dịch chuyển điểm bão hòa của độ thỏa mãn. `gamma` cao buộc AI phải có điểm cao mới thấy "đủ". |
| **`greed_threshold`** | Điều khiển tốc độ tích lũy của `Trade Desire`. Chỉ số này càng cao, AI càng "nóng lòng" muốn đổi bài qua mỗi lượt. |
| **`mean_confidence`** | Tâm thế khởi đầu của AI. Cao là hung hãn (Maniac), thấp là rụt rè (Nit). |
| **`min/max_skill`** | Xác định "trí thông minh" của AI. AI thông minh (>0.5) sẽ biết ưu tiên xác suất thắng thay vì cảm xúc. |

---

## 4. Tương tác Quần thể (Population Dynamics)

Khi bạn trộn lẫn nhiều Archetype trong một ván đấu, hệ thống sẽ nảy sinh các hiện tượng thú vị:
- **Sự dịch chuyển tài sản**: Tiền thường có xu hướng chảy từ Maniac sang Nit và Shark.
- **Áp lực lên Nhà cái**: Nếu nhà cái là một Nit đối đầu với một bàn toàn Shark, nhà cái sẽ nhanh chóng bị "bào" sạch tiền do không dám mạo hiểm trong những ván bài quyết định.

**Ghi chú:** Toàn bộ lịch sử hành vi của các Archetype này được ghi lại chi tiết trong file `_swap_decisions.csv` để phục vụ việc vẽ biểu đồ và phân tích tâm lý học hành vi.
