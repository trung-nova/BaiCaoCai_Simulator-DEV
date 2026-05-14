# MÔ HÌNH TOÁN HỌC VÀ HÀNH VI (MATHEMATICAL BEHAVIORAL MODEL)

Tài liệu này chi tiết hóa các công thức toán học được sử dụng để điều khiển trí tuệ nhân tạo (AI) trong hệ thống mô phỏng Bài Cào Cái. Mô hình được xây dựng dựa trên **Lý thuyết Hữu dụng (Utility Theory)** và **Tâm lý học hành vi**.

---

## 0. Triết lý Thiết kế và Cơ sở Hành vi (Design Philosophy & Behavioral Basis)

Dự án này không chỉ là một hệ thống mô phỏng trò chơi mà còn là một công cụ nghiên cứu về **Kinh tế học hành vi**. Việc đưa các tham số tâm lý vào AI nhằm mục đích mô phỏng hiện tượng **Hữu dụng hữu hạn (Bounded Rationality)** — nơi các cá thể không hành động hoàn toàn dựa trên logic toán học mà bị chi phối bởi các trạng thái nội tại.

### 0.1. Tại sao cần tham số tâm lý?
1.  **Tính thực chứng (Realism)**: Trong thực tế, con bạc không phải là một cỗ máy tính xác suất. Họ chịu ảnh hưởng của chuỗi thắng/thua, áp lực tài chính và cái tôi cá nhân.
2.  **Mô phỏng Đa cá thể (Population Diversity)**: Các tham số tâm lý tạo ra "DNA hành vi" riêng biệt cho từng AI, giúp quan sát được sự tương tác giữa các chiến thuật khác nhau (Shark vs. Maniac) trong một môi trường tài chính chung.
3.  **Nghiên cứu Định lượng (Quantitative Research)**: Bằng cách số hóa cảm xúc (Satisfaction, Desire), chúng ta có thể đo lường chính xác sự tác động của tâm lý lên hiệu quả kinh tế (ROI) và quản trị rủi ro.

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

> [!TIP]
> **Tùy chọn Bật/Tắt (TILT Toggle)**: Trong nghiên cứu đối chứng, người chơi có thể tắt hoàn toàn cơ chế này bằng cách đặt `enable_tilt=false` trong file `config.ini`. Khi tắt, AI sẽ luôn giữ trạng thái tâm lý ổn định bất kể kết quả ván đấu.

---

## 5. Lợi thế Nhà cái (House Edge)

Lợi thế toán học của sàn được thiết lập qua quy tắc so sánh không đối xứng:
$$Result = \begin{cases} \text{Player Wins} & \text{if } Score_P > Score_D \\ \text{Dealer Wins} & \text{if } Score_P \leq Score_D \end{cases}$$

Về mặt xác suất, trong một ván đấu ngẫu nhiên:
- $P(P > D) \approx 45\%$
- $P(D \geq P) \approx 55\%$
$\Rightarrow$ Nhà cái có lợi thế xấp xỉ **10%** trên mỗi lệnh cược (rất cao so với các trò chơi Casino tiêu chuẩn).

### 5.1. Phân tích Tỉ lệ thắng (Win Rate Analysis)

Trong báo cáo tổng kết (`FINAL SIMULATION REPORT`), tỉ lệ thắng (**Win Rate**) của các AI thường dao động quanh mức **41% - 44%**. Điều này có thể gây bất ngờ nếu so sánh với các trò chơi có 1 người thắng duy nhất (thường là $1/N$).

**Nguyên nhân toán học:**
1.  **Cơ chế đối đầu 1-1**: Mỗi ván đấu là tập hợp các cuộc đối đầu riêng biệt giữa từng Nhà con và Nhà cái. Nhiều người chơi có thể cùng "thắng" trong một ván nếu họ đều có điểm cao hơn Nhà cái.
2.  **Lợi thế Hòa (Tie-break House Edge)**: Như đã phân tích, xác suất thắng lý thuyết của Nhà con là ~45%. 
3.  **Hệ quả của việc luân phiên làm Cái (Dealer Rotation)**:
    - Khi làm **Nhà con** (chiếm $(N-1)/N$ thời gian): Tỉ lệ thắng kỳ vọng là $45\%$.
    - Khi làm **Nhà cái** (chiếm $1/N$ thời gian): Một "trận thắng" của Nhà cái chỉ được ghi nhận trong báo cáo nếu **tất cả** Nhà con khác đều thua (xác suất cực thấp, xấp xỉ $0.55^{N-1}$).
    - **Công thức hội tụ**: Với $N=17$, Win Rate tổng thể $\approx \frac{16}{17} \times 45\% + \frac{1}{17} \times 0\% \approx \mathbf{42.3\%}$.

Điều này giải thích tại sao các AI có kỹ năng cao (Shark) thường có Win Rate nhỉnh hơn (~44%) do tối ưu hóa được lượt đổi bài, trong khi các AI kém hơn sẽ thấp hơn mức trung bình này.

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

---

## 7. Cơ chế Săn Ba Tiên Chiến thuật (Tactical Ba Tiên Hunting)

Hệ thống mô phỏng một cấp độ tư duy cao hơn cho AI thông qua việc nhận diện bộ bài có tiềm năng thắng tuyệt đối (Ba Tiên).

### 7.1. Logic Phân loại Kỹ năng (Heuristic Selection)
Cơ chế này chỉ kích hoạt khi AI đạt ngưỡng Kỹ năng thực tế lớn hơn một giá trị giới hạn ($S_{threshold} = 0.7$).
- **AI Phổ thông ($S \le 0.7$)**: Tiếp cận theo hướng "Tối đa hóa điểm số". Coi lá bài Tây (0 điểm) là rác và ưu tiên vứt đi để tìm lá bài số (1-9).
- **AI Cao cấp ($S > 0.7$)**: Tiếp cận theo hướng "Tối ưu hóa bộ bài đặc biệt".

### 7.2. Điều kiện kích hoạt Hunting Mode
Hunting Mode được kích hoạt khi và chỉ khi:
1.  **Số lượng quân hình (J, Q, K)** trong tay hiện tại = **2**.
2.  **Lá bài còn lại không phải là lá Ace (Xì)**.
    - *Lưu ý*: Việc loại trừ lá Ace là một biến số chiến thuật, giả định rằng sự kết hợp giữa 2 lá Tây và 1 lá Ace mang lại một giá trị kỳ vọng khác hoặc nằm ngoài chiến lược săn bài của AI cao cấp trong mô phỏng này.

### 7.3. Hành động (Discard Decision)
Thay vì vứt lá bài 0 điểm (Tây) như logic thông thường, AI cao cấp sẽ:
- **Giữ lại 2 lá Tây**.
- **Vứt bỏ lá bài số/thường (lá thứ 3)**.
- **Mục tiêu**: Tận dụng cơ hội bốc được lá Tây thứ 3 để đạt bộ Ba Tiên (Thắng tuyệt đối).

Điều này giúp mô phỏng sự khác biệt rõ rệt giữa một người chơi chỉ biết tính điểm (Normal/Maniac) và một người chơi biết "nuôi bài" (Shark).

## 8. Phân tích Nghịch lý Kỹ năng (The High-Skill Paradox)

Trong quá trình mô phỏng, một hiện tượng thường xuyên xảy ra là nhóm **Shark (Kỹ năng > 0.8)** đôi khi có tỉ lệ thua lỗ nặng nề hơn các nhóm khác. Đây không phải lỗi logic mà là kết quả của sự tương tác giữa các quy tắc toán học:

### 8.1. Rủi ro từ Chiến thuật Săn Ba Tiên
Đây là nguyên nhân chính dẫn đến các chuỗi thua lỗ đột ngột của AI cao cấp:
- **Hành vi**: Shark sẵn sàng vứt bỏ quân bài có giá trị cao (như quân 9) để giữ lại 2 quân Tây với hy vọng bốc được quân Tây thứ 3.
- **Toán học**: Xác suất bốc trúng quân Tây thứ 3 để hoàn thành bộ Ba Tiên là rất thấp (~1/13). Việc đánh đổi một tay bài **9 điểm** (xác suất thắng thực tế ~90%) để lấy một cơ hội thắng tuyệt đối nhưng xác suất thấp là một chiến lược có **giá trị kỳ vọng (EV) âm** trong ngắn hạn.
- **Kết luận**: Shark "quá thông minh" dẫn đến việc chơi mạo hiểm hơn mức cần thiết, trong khi nhóm Nit (thận trọng) chỉ giữ bài 7-8 điểm lại có ROI ổn định hơn.

### 8.2. Sự bào mòn của Lợi thế Nhà cái (House Edge)
- Quy tắc **"Hòa điểm thì Nhà cái thắng"** tạo ra một rào cản vô hình mà kỹ năng không thể vượt qua.
- Dù Shark có kỹ năng đổi bài tốt để đạt điểm cao (8-9 điểm), họ vẫn thua nếu Nhà cái cũng đạt điểm đó. Với House Edge lên tới **10%**, mọi kỹ năng tối ưu hóa bài đều bị bào mòn theo thời gian nếu AI không có đủ vốn để vượt qua các chuỗi biến động (Variance).

### 8.3. Bẫy tâm lý TILT của người chơi lớn
- Nhóm Shark thường bắt đầu với sự tự tin cao và kỳ vọng lớn. Khi rơi vào chuỗi thua (Bad Run), cơ chế **TILT** kích hoạt làm giảm 10% kỹ năng và tăng 30% sự tự tin ảo.
- Một "Shark" bị TILT sẽ trở thành một "Maniac" có tính toán sai lầm, dẫn đến việc cháy túi nhanh hơn nhóm người chơi bình thường vốn đã quen với việc thua lỗ nhỏ lẻ.

> [!NOTE]
> Hiện tượng này minh chứng cho một bài học lớn trong Lý thuyết Trò chơi: **Trong một trò chơi có tổng âm (Negative-Sum Game) và House Edge cao, kỹ năng tối ưu hóa bài đôi khi không quan trọng bằng chiến thuật quản trị rủi ro và biết điểm dừng.**
