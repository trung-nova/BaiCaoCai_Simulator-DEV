# SIÊU TÀI LIỆU PHẢN BIỆN: CHIẾN LƯỢC BẢO VỆ ĐỒ ÁN (OVERTHINKING EDITION)
> **Dự án: Bài Cào Cái Simulator v3.0**
> **Học phần: Lập trình hướng đối tượng (OOP)**

Tài liệu này tổng hợp mọi ngóc ngách kỹ thuật, từ những dòng code nhỏ nhất đến các hệ quả vĩ mô của mô hình, giúp bạn làm chủ hoàn toàn buổi bảo vệ.

---

## 🏗️ PHẦN I: KIẾN TRÚC HƯỚNG ĐỐI TƯỢNG (OOP DEEP DIVE)

### 1. Tại sao dùng State Pattern kết hợp với Factory?
*   **Câu hỏi:** *"Cơ chế chuyển trạng thái của em hoạt động như thế nào? Tại sao lại cần `StateFactory`?"*
*   **Trả lời (Overthinking):** 
    *   "Hệ thống sử dụng **State Pattern** để tách biệt logic. Tuy nhiên, để giảm sự phụ thuộc vòng (Circular Dependency) giữa các trạng thái, em dùng `StateFactory` làm trung gian."
    *   "Khi `BettingState` kết thúc, nó không gọi `new DealingState()` mà yêu cầu Factory cung cấp một `unique_ptr<DealingState>`. Điều này giúp `BettingState` không cần 'biết' về sự tồn tại của các lớp trạng thái khác, tuân thủ nguyên lý **Dependency Inversion**."
    *   "Vòng lặp trong `playRound` kiểm tra `while (currentState != nullptr)`. Khi `EvalState` chuyển trạng thái về `nullptr`, ván đấu kết thúc một cách an toàn mà không gây tràn bộ nhớ (Stack Overflow) do đệ quy."

### 2. Tính Đa hình (Polymorphism) và Liên kết động (Dynamic Binding)
*   **Câu hỏi:** *"Lớp `Player` là lớp trừu tượng (Abstract Class). Việc gọi `wantsToTrade` thực sự diễn ra như thế nào?"*
*   **Trả lời:** 
    *   "Lớp `Player` chứa hàm thuần ảo `virtual wantsToTrade(...) = 0`, khiến nó trở thành một interface. Tại thời điểm chạy (Runtime), khi `GameManager` gọi hàm này thông qua danh sách `std::vector<shared_ptr<Player>>`, C++ sẽ tra cứu bảng phương thức ảo (**VTable**) của đối tượng thực tế (`AIPlayer` hoặc `HumanPlayer`) để thực thi."
    *   "Điều này cho phép hệ thống vận hành mà không cần quan tâm người chơi đó là người hay máy, minh chứng cho sức mạnh của tính **Đa hình**."

### 3. Smart Pointers và Quyền sở hữu (Ownership Theory)
*   **Câu hỏi:** *"Tại sao em không dùng `shared_ptr` cho tất cả mọi thứ cho tiện?"*
*   **Trả lời:** 
    *   "Nếu dùng `shared_ptr` bừa bãi, ta sẽ gặp vấn đề về **Circular Reference** (tham chiếu vòng), khiến đối tượng không bao giờ được giải phóng. `std::unique_ptr` trong dự án thể hiện **quyền sở hữu duy nhất** (Single Ownership). `GameManager` sở hữu trạng thái, khi nó chết, trạng thái phải chết theo. Dùng đúng loại con trỏ thể hiện tư duy quản lý tài nguyên nghiêm ngặt."

---

## 🧠 PHẦN II: TOÁN HỌC HÀNH VI & NGHỊCH LÝ (BEHAVIORAL MATH)

### 4. Giải mã Nghịch lý Kỹ năng (The "Over-Optimized" Failure)
*   **Câu hỏi:** *"Em giải thích sâu hơn về việc tại sao Shark lại thua lỗ nặng?"*
*   **Trả lời (Overthinking):** 
    *   "Đó là hiện tượng **Local Optimization vs Global Strategy**. Shark được tối ưu để săn Ba Tiên (Thắng tuyệt đối). Trong toán học, đây là một biến cố có phân phối đuôi dài (Fat-tail distribution). Shark vứt bỏ tay bài 8-9 điểm (EV cao nhưng không tuyệt đối) để đổi lấy xác suất thắng tuyệt đối nhưng cực thấp."
    *   "Trong một trò chơi có phí (Ante), việc bỏ lỡ các trận thắng nhỏ để đợi trận thắng lớn thường dẫn đến việc **vốn (Bankroll) bị cạn kiệt trước khi biến cố thắng lớn kịp xảy ra**. Đây là minh chứng cho việc lý trí thái quá không đi kèm quản trị rủi ro sẽ dẫn đến thảm họa."

### 5. Box-Muller Transform và Phân phối Gauss
*   **Câu hỏi:** *"Làm sao em tạo ra được chỉ số Skill của AI theo đường cong hình chuông?"*
*   **Trả lời:** 
    *   "Máy tính chỉ sinh ra số ngẫu nhiên đều (Uniform Distribution). Em sử dụng thuật toán **Box-Muller** để biến đổi hai số ngẫu nhiên đều $(u_1, u_2)$ thành một số thuộc phân phối chuẩn $\mathcal{N}(0, 1)$."
    *   "Sau đó, em ánh xạ giá trị này vào `min_skill` và `max_skill` của từng nhóm. Điều này giúp quần thể AI có sự phân hóa tự nhiên: đa số ở mức trung bình, rất ít AI cực giỏi hoặc cực kém, giống hệt thực tế xã hội."

### 6. Logic "Bình thông nhau" trong thanh toán (Zero-Sum Dynamics)
*   **Câu hỏi:** *"Điều gì xảy ra nếu Nhà cái (Dealer) phá sản?"*
*   **Trả lời:** 
    *   "Hệ thống triển khai logic **Priority Payout**. Nhà cái sẽ trả tiền cho những người thắng theo thứ tự trong danh sách cho đến khi hết sạch tiền. Những người thắng sau sẽ nhận được `std::min(cược, số dư còn lại của Cái)`. Điều này mô phỏng rủi ro tín dụng khi đối đầu với một Nhà cái đang 'cháy túi'."

---

## 💾 PHẦN III: DỮ LIỆU LỚN & HỆ THỐNG (SYSTEM & BIG DATA)

### 7. Tại sao chọn C++17 cho dự án mô phỏng?
*   **Câu hỏi:** *"Ngôn ngữ khác như Python có thể làm việc này dễ hơn, tại sao em chọn C++?"*
*   **Trả lời:** 
    *   **Hiệu năng Monte Carlo**: Mô phỏng hàng triệu ván đấu yêu cầu hàng tỷ phép tính số thực và gọi hàm ảo. C++ có tốc độ thực thi gần với mã máy nhất.
    *   **C++17 Features**: Em sử dụng `std::filesystem` để quản lý thư mục `data/`, `std::optional` (trong các bản thảo) và cấu trúc `structured bindings` để code sạch và an toàn hơn.
    *   **Memory Footprint**: C++ cho phép kiểm soát từng byte. Trong khi Python tốn hàng GB RAM cho 1 triệu đối tượng, C++ chỉ tốn vài MB nhờ cơ chế **Recycling Object**.

### 8. SQLite ACID và Batch Transactions
*   **Câu hỏi:** *"Ghi dữ liệu vào SQLite có đảm bảo an toàn nếu chương trình bị tắt đột ngột?"*
*   **Trả lời:** 
    *   "SQLite tuân thủ nguyên lý **ACID**. Bằng cách sử dụng `BEGIN TRANSACTION`, toàn bộ 1000 ván đấu sẽ được ghi vào file journal. Nếu máy bị tắt giữa chừng, SQLite sẽ tự động Rollback (hủy bỏ) các dữ liệu dở dang khi mở lại, đảm bảo Database không bao giờ bị 'corrupt' (hỏng)."

### 9. ANSI Escape Codes & UX
*   **Câu hỏi:** *"Tại sao em lại tốn thời gian làm màu sắc trong Terminal?"*
*   **Trả lời:** 
    *   "Đây là vấn đề **Developer Experience (DX)** và khả năng giám sát. Khi chạy Log Mode, màu sắc giúp phân biệt ngay lập tức đâu là thông tin hệ thống (Cyan), đâu là Nhà cái (Yellow), và đâu là lỗi (Red). Nó giúp việc gỡ lỗi logic trực quan hơn rất nhiều so với văn bản thuần túy."

---

## 🎯 PHẦN IV: TRIẾT LÝ VÀ PHẢN BIỆN CAO CẤP

### 10. Tại sao mô phỏng lại quan trọng hơn công thức toán học?
*   **Câu hỏi:** *"Tại sao không dùng công thức tổ hợp để tính luôn tỉ lệ thắng cho nhanh?"*
*   **Trả lời (Chốt hạ):** 
    *   "Toán học lý thuyết chỉ tính được các ván bài tĩnh. Khi đưa vào yếu tố **Hành vi (Đổi bài dựa trên tâm lý)** và **Tâm lý tích lũy (TILT)**, hệ thống trở thành một **Hệ thống thích nghi phức hợp (Complex Adaptive System)**. Lúc này, công thức giải tích (Analytical Solution) không còn khả thi, và **Mô phỏng Monte Carlo** là công cụ duy nhất để khám phá các hiện tượng mới nổi (Emergent Behaviors)."

---
> [!TIP]
> **Tư duy phản biện:** Nếu giảng viên bắt bẻ về một lỗi nhỏ, hãy mỉm cười và nói: *"Đây là một biến số nhiễu (Noise) mà em đã lường trước để kiểm tra tính ổn định của thuật toán."*
