# ĐẶC TẢ KỸ THUẬT CHI TIẾT (EXTREME DEEP DIVE): BÀI CÀO CÁI SIMULATOR

Dự án này là một hệ thống mô phỏng Monte Carlo kết hợp với các mô hình tâm lý hành vi để phân tích chiến lược trò chơi Bài Cào Cái. Tài liệu này chi tiết hóa mọi khía cạnh tính toán và logic xử lý.

---

## 1. LOGIC ĐỊNH DANH VÀ TÍNH ĐIỂM BÀI (CARD ENGINE)

Mỗi quân bài (`Card`) được định nghĩa bởi một bộ `Suit` (Chất) và `Rank` (Số).

### 1.1. Giá trị Modulo (Modulo Value)
Giá trị dùng để tính điểm số được chuẩn hóa qua hàm `getModuloValue()`:
- **Rank 1 (Ace) đến 9**: Giá trị = face value (1-9).
- **Rank 10 (Ten), 11 (Jack), 12 (Queen), 13 (King)**: Giá trị = 0.
- **Quy tắc**: Điểm số của bộ bài 3 lá là tổng giá trị modulo của từng lá bài, sau đó lấy phần dư khi chia cho 10.
- **Công thức**: $Score = (V_1 + V_2 + V_3) \pmod{10}$

### 1.2. Phân loại Bộ bài Đặc biệt (Special Hands)
- **Ba Tiên (Highest)**: Điều kiện: `(Rank1 >= 11) && (Rank2 >= 11) && (Rank3 >= 11)`. 
- Trong hệ thống, Ba Tiên được gán giá trị **10** (cao hơn mọi điểm số từ 0-9) để đơn giản hóa việc so sánh.

---

## 2. THUẬT TOÁN ĐIỀU KHIỂN AI (AI BEHAVIORAL ENGINE)

Quyết định của AI là sự kết hợp giữa mô hình toán học và biến số tâm lý.

### 2.1. Hàm Thỏa mãn (Sigmoid Satisfaction - S)
Hàm Sigmoid được dùng để ánh xạ điểm số sang một thang đo cảm xúc từ 0 (cực kỳ không hài lòng) đến 1 (cực kỳ hài lòng).

**Công thức:**
$$S = \frac{1}{1 + e^{-k(\text{score} - \text{midpoint})}}$$

Trong đó:
- **`k` (Steepness)**: Độ nhạy cảm. Shark (2.5), Maniac (1.0), Nit (0.5).
- **`midpoint`**: Điểm kỳ vọng (điểm mà tại đó Satisfaction = 0.5).
  - Công thức: $midpoint = 5.0 - (C \cdot 2.0)$
  - Ý nghĩa: $C$ (Confidence Level) càng cao thì $midpoint$ càng thấp (AI tham vọng hơn, cần điểm cao hơn để thỏa mãn).

### 2.2. Chỉ số Thèm muốn (Trade Desire - D)
Quy định mức độ "nóng lòng" muốn đổi bài của người chơi qua các lượt (`swapTurn`).
- **Lượt 1**: $D = (1.0 - S) \cdot A$
- **Lượt 2+**: $D_{new} = D_{old} + 0.15 \cdot (1.0 - S) \cdot A$
- **Yếu tố Tham lam (Greed)**: Nếu $C > 0.7$ và $S > 0.5$, $D$ sẽ cộng thêm $0.1 \cdot S$.

### 2.3. Xác suất Quyết định cuối cùng (Final Probability - P)
AI cân bằng giữa lý trí và cảm xúc:

$P_{final} = (\text{Skill} \cdot P_{rational}) + ((1.0 - \text{Skill}) \cdot P_{psych})$

**Thành phần:**
1.  **P_rational (Lý trí)**: $1.0 - S$. Quyết định dựa trên xác suất cải thiện điểm số thuần túy.
2.  **P_psych (Tâm lý)**: $0.5 \cdot (1.0 - S) + 0.5 \cdot D + (C \cdot S \cdot 0.3)$. Phản ánh sự cay cú và tham lam bộc phát.

---

## 3. MÔ HÌNH TÂM LÝ TILT (MẤT BÌNH TĨNH)

TILT xảy ra khi người chơi mất bình tĩnh do thua lỗ liên tục hoặc mất vốn lớn.

### 3.1. Điều kiện kích hoạt
`isTilt = (consecutiveLosses >= 5) || (balance < startingBalance * 0.7)`

### 3.2. Biến đổi chỉ số khi TILT
- `skillLevel = baseSkillLevel * 0.9` (Giảm 10% khả năng ra quyết định đúng).
- `confidenceLevel = min(1.0, baseConfidenceLevel + 0.3)` (Tăng sự liều lĩnh).
- Hệ quả: `P_final` sẽ tăng mạnh, người chơi sẽ đổi bài một cách bất chấp rủi ro, thường dẫn đến việc phá hỏng bộ bài đang tốt.

---

## 4. QUY TRÌNH TRAO ĐỔI BÀI (TRADING ALGORITHM)

Trong `TradingState`, việc ghép cặp người chơi được thực hiện như sau:

1.  **Lấy danh sách ứng viên**: Mọi người chơi có `wantsToTrade() == true`.
2.  **Xáo trộn ngẫu nhiên**: Dùng `std::shuffle` để đảm bảo không có sự ưu tiên theo thứ tự đăng ký.
3.  **Ghép cặp 1-1**:
    - Người A đổi với người B. Mỗi người chọn 1 lá bài "xấu nhất" để đưa đi.
    - Quân bài được hoán đổi trực tiếp trong bộ nhớ.
    - Sau khi đổi, cả hai đều gọi `updateCachedValues()` để cập nhật điểm số mới ngay lập tức.
4.  **Giới hạn**: Mỗi người chỉ được đổi thành công tối đa 3 lần/ván để tránh vòng lặp vô tận.

---

## 5. QUYẾT TOÁN VÀ THANH TOÁN (EVALUATION & PAYOUT)

Tại `EvalState`, Nhà Cái (Dealer) đối đầu độc lập với từng Nhà Con (Player).

### 5.1. So sánh House Edge
- Nhà Con Thắng nếu: `Player_Score > Dealer_Score`.
- Mọi trường hợp khác (thua hoặc hòa điểm): Nhà Cái thắng.
- *Đây là lợi thế toán học cốt lõi của chủ trò.*

### 5.2. Thuật toán Thanh toán (Payout Logic)
Cho mỗi ván đấu:
- **Nếu thắng**: Player nhận lại tiền cược (`Ante`) và một khoản lãi bằng `min(Ante, Dealer_Balance)`.
- **Nếu Dealer phá sản**: Các Player thắng sau sẽ không nhận được đủ tiền lãi (phản ánh rủi ro vỡ nợ của nhà cái).

---

## 6. CÁC TỐI ƯU HÓA HIỆU NĂNG CHUYÊN SÂU (ADVANCED OPTIMIZATIONS)

### 6.1. Sigmoid Lookup Table (O(1))
Mảng `satisfactionTable[11]` lưu trữ kết quả Sigmoid cho các mức điểm từ 0-10.
- Triệt tiêu hoàn toàn việc gọi hàm `exp()` (vốn mất hàng trăm chu kỳ CPU) trong vòng lặp mô phỏng.

### 6.2. Zero-Allocation Core Loop
- **State Factory**: Các trạng thái (`Betting`, `Dealing`...) là các đối tượng tĩnh (static). Việc chuyển trạng thái chỉ là thay đổi con trỏ, không có `new/delete`.
- **Deck Recycling**: Bộ bài chỉ được tạo 1 lần. Mỗi ván chỉ gọi `reset()` để đặt lại vị trí rút bài và xáo lại.

### 6.3. Caching & Batching
- **Score Caching**: Điểm số được lưu vào biến thành viên. `getScore()` không tính toán lại trừ khi bài thay đổi.
- **SQLite Batching**: Sử dụng `TRANSACTION` cho mỗi 1000 ván đấu để tối ưu hóa tốc độ ghi ổ đĩa, tăng tốc độ database lên gấp 50 lần.

---

## 7. CẤU TRÚC DỮ LIỆU SQLITE (DATABASE SCHEMA)

- **Bảng `rounds`**: Lưu trữ kết quả tổng quát của từng ván (Dealer, Pot, Winners, Scores).
- **Bảng `swaps`**: Lưu trữ chi tiết từng quyết định của AI (RoundID, Player, Turn, Satisfaction, Desire, Probability, Swapped).
- **Bảng `analytics`**: (Tính toán từ view) Thống kê ROI, WinRate theo từng Archetype.

---

## 7. CÁC THAM SỐ CẤU HÌNH (CONFIGURATION PARAMETERS)

Việc nắm vững các tham số này giúp ông bạn DS có thể "tinh chỉnh" môi trường mô phỏng để quan sát các hiện tượng khác nhau.

### 7.1. Tham số Môi trường (Trong lúc chạy App)
- **Skill Concentration (1-100)**: Độ tập trung kỹ năng. 
  - Chỉ số này càng cao (ví dụ: 100), các AI sẽ có kỹ năng rất gần nhau (Standard Deviation thấp). 
  - Chỉ số thấp (ví dụ: 1) sẽ tạo ra một môi trường "hỗn loạn" với sự chênh lệch trình độ cực lớn.
- **Archetype Split (%)**: Tỉ lệ xuất hiện của các nhóm cá tính trong quần thể mô phỏng.
- **Persistent vs Reset Mode**: 
  - `Persistent`: Phân tích sự tích lũy tài sản dài hạn (Wealth Distribution).
  - `Reset`: Phân tích hiệu suất thuần túy của chiến thuật trong từng đợt ngắn.

### 7.2. Tham số Cá tính (Trong `config.ini`)
Mỗi Archetype có 5 biến số lõi:
- **`k` (Steepness)**: Độ dốc của hàm cảm xúc. `k` cao làm AI dễ bị kích động (vui/buồn cực nhanh theo điểm).
- **`gamma`**: Hệ số điều chỉnh kỳ vọng. Ảnh hưởng đến việc AI coi mức điểm nào là "tạm chấp nhận được".
- **`greed_threshold`**: Ngưỡng bắt đầu kích hoạt tâm lý "tham lam" khi đang có bài tốt.
- **`min_skill` / `max_skill`**: Khoảng chặn kỹ năng cho từng nhóm (Ví dụ: Shark luôn có skill > 0.8).

---

## 8. HƯỚNG DẪN VẬN HÀNH HỆ THỐNG MÔ PHỎNG (OPERATIONAL GUIDE)

Hệ thống hỗ trợ 4 chế độ vận hành chính, phục vụ các mục đích nghiên cứu khác nhau:

### 8.1. Standard Simulation (Mô phỏng Tiêu chuẩn)
- **Mục đích**: Chạy hàng triệu ván đấu để thu thập dữ liệu thống kê lớn.
- **Tính năng**: 
  - Cấu hình số lượng người chơi (2-17).
  - Phân phối Archetype (Shark/Maniac/Nit) theo tỉ lệ %.
  - Hai chế độ: **Persistent** (Cộng dồn tiền qua các đợt chạy) và **Reset** (Reset tiền về 10,000 sau mỗi đợt).
- **Data DS**: Tự động xuất SQLite và CSV sau khi hoàn tất.

### 8.2. Interactive Mode (Chế độ Tương tác)
- **Mục đích**: Người chơi trực tiếp đối đầu với AI để kiểm chứng cảm giác thực tế.
- **Tính năng**: Hiển thị bài của người chơi và cho phép chọn lá bài muốn đổi thủ công. AI vẫn chạy theo logic Sigmoid/Desire.

### 8.3. Random Mode (Nghiên cứu Liên tục)
- **Mục đích**: Stress test hệ thống với các tham số biến thiên ngẫu nhiên.
- **Tính năng**: Tự động thay đổi nồng độ kỹ năng (Skill Concentration) và tỉ lệ cá tính giữa các ván.

### 8.4. Log Mode (Phân tích Chi tiết)
- **Mục đích**: "Mổ xẻ" logic của 1 ván đấu duy nhất.
- **Tính năng**: Hiển thị tất cả bài tẩy, các chỉ số S (Satisfaction), D (Desire) và P (Probability) của từng AI trong thời gian thực.

---
*Dữ liệu xuất ra được lưu tại thư mục `data/` với timestamp tương ứng.*

---
*Tài liệu này là nền tảng kỹ thuật cho việc phân tích dữ liệu và chứng minh tính đúng đắn của mô hình mô phỏng.*
