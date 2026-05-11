# MÔ HÌNH TOÁN HỌC VÀ HÀNH VI (MATHEMATICAL BEHAVIORAL MODEL)

Tài liệu này chi tiết hóa các công thức toán học được sử dụng để điều khiển trí tuệ nhân tạo (AI) trong hệ thống mô phỏng Bài Cào Cái.

---

## 1. Hàm Thỏa mãn (Logistic Sigmoid Satisfaction)

Cốt lõi của tâm lý AI là sự ánh xạ từ điểm số thực tế ($x \in [0, 10]$) sang mức độ hài lòng cảm tính ($S \in [0, 1]$). Chúng ta sử dụng hàm **Logistic Sigmoid**:

$$S(x) = \frac{1}{1 + e^{-k(x - x_0)}}$$

### 1.1. Tham số $k$ (Steepness/Sensitivity)
Tham số $k$ điều khiển độ nhạy cảm của AI đối với sự thay đổi điểm số:
- **$k$ cao (2.0 - 3.0):** AI rất nhạy cảm. Chỉ cần thay đổi 1-2 điểm là mức độ thỏa mãn nhảy vọt từ 0 lên 1 (Tính cách **SHARK**).
- **$k$ thấp (0.5 - 1.0):** AI có cảm xúc "lỳ" hơn, sự hài lòng thay đổi từ từ qua các mức điểm (Tính cách **NIT** hoặc **MANIAC**).

### 1.2. Điểm kỳ vọng $x_0$ (Midpoint) và Tham số $\gamma$ (Gamma)
$x_0$ là điểm mà tại đó AI đạt 50% mức độ thỏa mãn. Nó không cố định mà biến thiên theo tính cách và sự tự tin:

$$x_0 = (5.0 - 2 \cdot Confidence) + (\gamma - 2.0)$$

Trong đó:
- **$Confidence$**: Chỉ số tự tin (biến thiên khi TILT).
- **$\gamma$ (Gamma)**: Hệ số rủi ro/kỳ vọng cơ sở. 
    - **$\gamma > 2.0$**: Đẩy $x_0$ sang phải $\rightarrow$ AI khó thỏa mãn hơn, cần điểm cao hơn để thấy an tâm (Đặc thù của nhóm **NIT**).
    - **$\gamma < 2.0$**: Đẩy $x_0$ sang trái $\rightarrow$ AI dễ thỏa mãn hơn với điểm số thấp.

---

## 2. Chỉ số Thèm muốn (Trade Desire - $D$)

Chỉ số $D$ mô tả động lực thôi thúc AI thực hiện hành động đổi bài. Nó tích lũy qua thời gian nếu mục tiêu hài lòng chưa đạt được.

- **Khởi tạo (Lượt 1):**
  $$D_1 = (1.0 - S) \cdot Aggression$$
- **Tích lũy (Lượt $n$):**
  $$D_n = D_{n-1} + 0.15 \cdot (1.0 - S) \cdot Aggression + Greed$$

**Trong đó:**
- $Aggression = 0.5 + (0.5 \cdot Confidence)$
- $Greed = 0.1 \cdot S$ (chỉ kích hoạt khi $Confidence > 0.7$ và $S > 0.5$ - phản ánh tâm lý muốn "tới bến" khi bài đang khá).

---

## 3. Xác suất Quyết định (Decision Probability - $P_{final}$)

AI không đưa ra quyết định nhị phân cứng nhắc mà tính toán một xác suất để thực hiện hành động. Đây là sự kết hợp giữa **Lý trí (Rational)** và **Tâm lý (Psychological)**.

$$P_{final} = (Skill \cdot P_{rational}) + ((1 - Skill) \cdot P_{psych})$$

### 3.1. $P_{rational}$ (Lý trí)
Dựa trên xác suất cải thiện. Trong mô hình đơn giản hóa:
- Nếu $Skill > 0.5$: $P_{rational} = 1.0 - S$ (Bài càng tệ càng muốn đổi).
- Nếu $Skill \leq 0.5$: $P_{rational} = 0.5$ (Quyết định ngẫu nhiên do thiếu kỹ năng).

### 3.2. $P_{psych}$ (Tâm lý)
Phản ánh các xung động bộc phát:
$$P_{psych} = 0.5(1-S) + 0.5D + (Confidence \cdot S \cdot 0.3)$$

---

## 4. Mô hình Biến động TILT

TILT là một hàm chuyển đổi trạng thái (State Transition) làm thay đổi các tham số đầu vào của mô hình:

Khi ván đấu thỏa mãn điều kiện $Losses \geq 5$ hoặc $Balance < 0.7 \cdot Balance_{start}$:

1.  **Suy giảm Kỹ năng:** $Skill_{active} = Skill_{base} \cdot 0.9$
2.  **Tăng cường Tự tin thái quá:** $Confidence_{active} = \min(1.0, Confidence_{base} + 0.3)$

**Hệ quả toán học:**
Khi $Confidence$ tăng mạnh, $x_0$ giảm xuống $\rightarrow$ $S$ tăng ảo $\rightarrow$ AI trở nên hung hãn một cách phi lý, dẫn đến các quyết định đổi bài rủi ro cực cao (High Variance).

---

## 5. Lợi thế Nhà cái (House Edge)

Lợi thế toán học của sàn được thiết lập qua quy tắc so sánh:
$$Result = \begin{cases} Player\ Wins & \text{if } Score_P > Score_D \\ Dealer\ Wins & \text{if } Score_P \leq Score_D \end{cases}$$

Điểm hòa ($Score_P = Score_D$) luôn thuộc về Nhà Cái. Đây là hằng số đảm bảo kỳ vọng lợi nhuận ($E[X]$) của Dealer luôn dương trong dài hạn (Luật chơi đặc trưng của Bài Cào Cái).
