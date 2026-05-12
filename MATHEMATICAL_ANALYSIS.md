# MÔ HÌNH TOÁN HỌC VÀ HÀNH VI (MATHEMATICAL BEHAVIORAL MODEL)

Tài liệu này chi tiết hóa các công thức toán học được sử dụng để điều khiển trí tuệ nhân tạo (AI) trong hệ thống mô phỏng Bài Cào Cái. Mô hình được xây dựng dựa trên **Lý thuyết Hữu dụng (Utility Theory)** và **Tâm lý học hành vi**.

---

## 1. Hàm Thỏa mãn (Logistic Sigmoid Satisfaction)

Cốt lõi của tâm lý AI là sự ánh xạ từ điểm số thực tế ($x \in [0, 10]$) sang mức độ hài lòng cảm tính ($S \in [0, 1]$). Chúng ta sử dụng hàm **Logistic Sigmoid** để mô phỏng sự bão hòa của cảm xúc:

$$S(x) = \frac{1}{1 + e^{-k(x - x_0)}}$$

### 1.1. Tham số $k$ (Steepness - Độ nhạy cảm)
Tham số $k$ điều khiển độ dốc của hàm, đại diện cho tính cách của người chơi:
- **$k \in [2.0, 3.0]$ (SHARK)**: AI có ranh giới hài lòng rất rõ ràng. Chỉ cần đạt ngưỡng là mức thỏa mãn tăng vọt.
- **$k \in [0.5, 1.0]$ (NIT/MANIAC)**: AI có cảm xúc biến thiên chậm, khó phân biệt rõ ràng giữa các mức điểm trung bình.

### 1.2. Điểm kỳ vọng $x_0$ (Midpoint)
$x_0$ là "điểm hòa vốn" về mặt cảm xúc ($S = 0.5$). Nó không cố định mà phụ thuộc vào **Chỉ số tự tin ($C$)** và **Hệ số rủi ro ($\gamma$)**:

$$x_0 = (5.0 - 2 \cdot C) + (\gamma - 2.0)$$

> **Ý nghĩa**: Khi tự tin ($C$) tăng cao (do đang thắng), $x_0$ dịch sang trái $\rightarrow$ AI dễ thấy "bài này cũng tạm" hơn. Ngược lại, người chơi cẩn trọng (Gamma cao) sẽ đẩy $x_0$ sang phải, yêu cầu bài cực cao mới thấy an tâm.

---

## 2. Chỉ số Thèm muốn (Trade Desire - $D$)

Chỉ số $D$ mô tả động lực thôi thúc AI thực hiện hành động đổi bài. Đây là một biến trạng thái tích lũy:

- **Khởi tạo (Lượt 1):**
  $$D_1 = (1.0 - S) \cdot A$$
- **Tích lũy (Lượt $n$):**
  $$D_n = D_{n-1} + 0.15 \cdot (1.0 - S) \cdot A + Greed$$

**Trong đó:**
- **$A$ (Aggression)**: $0.5 + (0.5 \cdot C)$.
- **$Greed$ (Sự tham lam)**: Chỉ kích hoạt khi bài đã tốt nhưng người chơi vẫn muốn tối ưu thêm.
  $$Greed = \begin{cases} 0.1 \cdot S & \text{nếu } C > 0.7 \text{ và } S > 0.5 \\ 0 & \text{trường hợp khác} \end{cases}$$

---

## 3. Xác suất Quyết định (Decision Probability - $P_{final}$)

Quyết định cuối cùng được tính theo mô hình **Dual-Process Theory**, kết hợp giữa Hệ thống 1 (Trực giác/Tâm lý) và Hệ thống 2 (Lý trí/Kỹ năng):

$$P_{final} = (Skill \cdot P_{rational}) + ((1 - Skill) \cdot P_{psych})$$

### 3.1. $P_{rational}$ (Lý trí)
Dựa trên xác suất cải thiện kỳ vọng ($E[I]$). Trong mô hình hiện tại:
- Nếu $Skill > 0.5$: $P_{rational} = 1.0 - S$ (Bài càng thấp, xác suất cải thiện càng cao).
- Nếu $Skill \leq 0.5$: $P_{rational} = 0.5$ (Quyết định ngẫu nhiên).

### 3.2. $P_{psych}$ (Tâm lý)
Mô phỏng các xung động cảm tính, đặc biệt là khi bị TILT:
$$P_{psych} = 0.5(1-S) + 0.5D + (C \cdot S \cdot 0.3)$$

---

## 4. Mô hình Biến động TILT

TILT được định nghĩa là trạng thái mất kiểm soát sau một chuỗi biến cố tiêu cực. Hệ thống sử dụng một bộ lọc ngưỡng (Threshold Filter):

Khi $Losses \geq 5$ hoặc $Balance < 0.7 \cdot Balance_{initial}$:

1.  **Suy giảm Kỹ năng**: $Skill_{active} = Skill_{base} \cdot 0.9$
2.  **Tăng cường Tự tin ảo**: $Confidence_{active} = \min(1.0, Confidence_{base} + 0.3)$

**Hệ quả**: AI sẽ rơi vào vòng xoáy "đánh liều để gỡ vốn", một hiện tượng phổ biến trong tâm lý học cờ bạc (Gambler's Fallacy).

---

## 5. Lợi thế Nhà cái (House Edge)

Lợi thế toán học của sàn được thiết lập qua quy tắc so sánh không đối xứng:
$$Result = \begin{cases} Player\ Wins & \text{if } Score_P > Score_D \\ Dealer\ Wins & \text{if } Score_P \leq Score_D \end{cases}$$

Về mặt xác suất, trong một ván đấu ngẫu nhiên:
- $P(P > D) \approx 45\%$
- $P(D \geq P) \approx 55\%$
$\Rightarrow$ Nhà cái có lợi thế xấp xỉ **10%** trên mỗi lệnh cược (rất cao so với các trò chơi Casino tiêu chuẩn).

---

## 6. Cơ chế Hạt giống Ngẫu nhiên và Tính Tái lập (Deterministic Seed & Reproducibility)

Hệ thống mô phỏng sử dụng cơ chế **Hạt giống ngẫu nhiên (Seed)** để đảm bảo tính khoa học và khả năng kiểm chứng (verifiability) của các kết quả nghiên cứu. Đây là cơ sở để biến một hệ thống có tính ngẫu nhiên cao thành một mô hình toán học tất định (deterministic).

### 6.1. Bản chất của Bộ sinh số Mersenne Twister (`std::mt19937`)

Dự án sử dụng thuật toán Mersenne Twister, một bộ máy sinh số giả ngẫu nhiên (Pseudo-Random Number Generator - PRNG) dựa trên các phép toán logic (dịch bit, XOR). 

- **Tính tất định (Determinism)**: Với một giá trị Seed $S$ ban đầu, bộ máy sẽ luôn tạo ra một chuỗi các số nguyên khổng lồ ($0$ đến $2^{32}-1$) giống hệt nhau theo một thứ tự cố định. Có thể ví Seed như "Số thứ tự trang" trong một cuốn sách chứa các con số được in sẵn. Khi ta mở đúng trang đó, các con số đọc được sẽ luôn theo một trình tự không đổi.

### 6.2. Phân cấp Hạt giống (Hierarchical RNG Structure)

Để đảm bảo các biến số trong mô phỏng không bị chồng lấn và gây nhiễu, hệ thống sử dụng cấu trúc phân cấp:

1.  **Hạt giống Tổng (Global Seed - $S_{global}$)**: 
    - Do người dùng nhập hoặc sinh tự động từ thời gian hệ thống.
    - Điều khiển việc khởi tạo toàn bộ "vũ trụ" mô phỏng.
2.  **Hạt giống Thành phần (Component Seeding)**:
    - Sử dụng bộ máy tổng để sinh ra các thông số cơ bản cho từng AI (Skill, Confidence).
    - Sinh ra các **Hạt giống con (Local Seeds)** cho riêng từng đối tượng AI để chúng tự vận hành logic bên trong ván đấu.

### 6.3. Quy trình chuyển đổi từ Seed sang Tham số

Hệ thống thực hiện phép ánh xạ từ các số nguyên khổng lồ sang các tham số thực tế thông qua các bước:

#### Bước A: Ép khuôn Kỹ năng (Distribution Mapping)
Sử dụng **Phân phối chuẩn (Normal Distribution)** $\mathcal{N}(\mu, \sigma^2)$ để tạo ra sự đa dạng tự nhiên cho AI:
- **Lấy số thô**: Rút một số nguyên $U$ từ bộ máy tổng.
- **Áp dụng thuật toán Box-Muller**: Biến đổi $U$ thành một giá trị $x$ nằm trong đường cong hình chuông (Bell Curve).
- **Công thức tham số**: 
  $$Skill = \text{Clamp}(\mu_{arch} + \sigma \cdot x, 0.0, 1.0)$$
  *Trong đó $\mu_{arch}$ là giá trị trung bình của từng nhóm AI (Shark, Maniac, Nit).*

#### Bước B: Chuỗi Nhân quả (Chain of Causality)
Tính tái lập được đảm bảo bởi thứ tự rút số không đổi. Ví dụ với **Seed: 7**:

| Lượt rút | Mục đích | Số thô rút ra | Tham số được gán |
| :--- | :--- | :--- | :--- |
| 1 | Skill cho AI_1 | 1,234,567,890 | **0.88** (Shark-like) |
| 2 | Confidence cho AI_1 | 987,654,321 | **0.15** (Dận dặt) |
| 3 | Hạt giống con cho AI_1 | 2,121,212,121 | Dùng làm Seed nội bộ cho AI_1 |
| 4 | Skill cho AI_2 | 3,333,333,333 | **0.45** (Normal-like) |
| 5 | ... | ... | ... |

### 6.4. Ý nghĩa trong Nghiên cứu Học thuật

1.  **Tính Tái lập (Reproducibility)**: Một kịch bản mô phỏng ván thứ 10,000 xảy ra hiện tượng TILT có thể được tái hiện lại chính xác 100% trên bất kỳ máy tính nào chỉ bằng cách nhập lại đúng giá trị Seed.
2.  **Kiểm soát Biến số (Sensitivity Analysis)**: Nhà nghiên cứu có thể giữ nguyên Seed (để giữ nguyên bộ bài và tính cách nhân vật) nhưng thay đổi các tham số trong `config.ini` để quan sát sự thay đổi của kết quả, giúp cô lập biến số hiệu quả.
3.  **Hành vi Định mệnh**: Quyết định của AI không còn là "may rủi" mà là kết quả tất yếu của một chuỗi logic toán học bắt nguồn từ Seed. Điều này cho phép phân tích hành vi AI như một hệ thống tất định phức hợp.



