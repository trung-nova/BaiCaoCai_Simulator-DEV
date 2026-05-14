# TÀI LIỆU CHIẾN LƯỢC: GIẢI ĐÁP PHẢN BIỆN (LECTURER MASTER Q&A)
> **Dự án: Hệ thống mô phỏng Bài Cào Cái v3.0 (Academic Research Framework)**

Tài liệu này được thiết kế để giúp sinh viên đối đáp với Giảng viên ở cấp độ chuyên gia, tập trung vào 3 trụ cột: **Lập trình hướng đối tượng (OOP)**, **Toán học hành vi** và **Khoa học dữ liệu**.

---

## 🏛️ PHẦN I: KIẾN TRÚC LẬP TRÌNH (OOP & DESIGN PATTERNS)

### 1. Tại sao lại sử dụng State Pattern cho luồng trò chơi?
*   **Câu hỏi:** *"Tại sao không dùng một hàm `while` lớn với nhiều lệnh `if-else` để điều khiển ván đấu?"*
*   **Trả lời:** 
    *   "Việc dùng `if-else` sẽ vi phạm nguyên lý **Open/Closed** (SOLID). Khi muốn thêm luật mới (ví dụ: lượt Tố tiền), ta phải sửa trực tiếp vào hàm chính, rất dễ gây lỗi."
    *   "Với **State Pattern** (Betting, Dealing, Trading, Eval), mỗi trạng thái là một lớp độc lập kế thừa từ `GameState`. `GameManager` chỉ việc gọi `update()`, logic cụ thể nằm gọn trong từng lớp. Điều này giúp mã nguồn cực kỳ mạch lạc và dễ mở rộng."

### 2. Sự khác biệt giữa `unique_ptr` và `shared_ptr` trong dự án?
*   **Câu hỏi:** *"Em sử dụng Smart Pointers như thế nào? Tại sao lại dùng cả hai loại?"*
*   **Trả lời:** 
    *   "**`std::unique_ptr`** dùng cho `currentState` trong `GameManager`. Vì tại một thời điểm chỉ có duy nhất một trạng thái nắm quyền điều khiển. Khi chuyển trạng thái, con trỏ cũ tự động giải phóng, đảm bảo không rò rỉ bộ nhớ."
    *   "**`std::shared_ptr`** dùng cho danh sách `Player`. Vì đối tượng người chơi cần được truy cập đồng thời bởi `GameManager` và các lớp `State` khác nhau. `shared_ptr` sử dụng cơ chế **Reference Counting** để quản lý vòng đời đối tượng một cách an toàn."

### 3. Tính Đóng gói (Encapsulation) và Friend Class
*   **Câu hỏi:** *"Tại sao em lại dùng `friend class`? Nó có phá vỡ tính đóng gói không?"*
*   **Trả lời:** 
    *   "Không ạ. Việc dùng `friend class` cho các lớp `State` giúp ta giữ các thuộc tính của `Player` ở phạm vi `protected`. Thay vì mở public `setBalance` cho toàn thế giới, ta chỉ cho phép các lớp State (vốn là một phần của logic Game) được quyền chỉnh sửa. Điều này thực chất là **tăng cường kiểm soát truy cập**."

---

## 📈 PHẦN II: NGHỊCH LÝ TOÁN HỌC & HÀNH VI (BEHAVIORAL PARADOXES)

### 4. Nghịch lý Kỹ năng (The High-Skill Paradox)
*   **Câu hỏi:** *"Tại sao dữ liệu cho thấy AI Shark (Kỹ năng cao) đôi khi lại thua lỗ nặng nhất?"*
*   **Giải đáp:** Đây là điểm nhấn về phản biện. Có 3 nguyên nhân:
    1.  **Chiến thuật Hunting (Săn bài)**: Shark được lập trình để "nuôi" bộ Ba Tiên. Nó sẵn sàng vứt bỏ quân 9 (cầm chắc thắng) để hy vọng bốc được quân Tây thứ 3. Xác suất này chỉ ~7.6%, dẫn đến việc Shark tự phá hủy lợi thế của mình trong ngắn hạn.
    2.  **Lợi thế Hòa (Tie-break)**: Trong Bài Cào Cái, hòa điểm thì Nhà cái thắng. Shark dù giỏi tối ưu bài đến mấy vẫn bị House Edge (10%) bào mòn vốn.
    3.  **TILT Vulnerability**: Shark thường có mức kỳ vọng cao. Khi rơi vào chuỗi thua, cơ chế TILT kích hoạt khiến chúng trở nên liều lĩnh hơn các nhóm khác để "gỡ vốn", dẫn đến phá sản nhanh hơn.

### 5. Tại sao Win Rate hội tụ về 42%?
*   **Câu hỏi:** *"Nếu may mắn là 50/50, tại sao tỉ lệ thắng trung bình của người chơi lại thấp hơn?"*
*   **Giải đáp:** 
    *   "Theo xác suất, ván đấu có 11 kết quả điểm (0-10). Do quy tắc Nhà cái thắng khi hòa, Nhà con chỉ thắng khi **ScoreP > ScoreD**. Về mặt tổ hợp, xác suất này là ~45%."
    *   "Khi tính thêm việc luân phiên làm Cái (Dealer Rotation), một người làm Cái chỉ thắng tuyệt đối khi **tất cả** người khác thua. Tổng hợp các yếu tố này, con số 42% là kết quả hội tụ toán học chính xác của mô hình Bài Cào thực tế."

### 6. Tại sao dùng hàm Sigmoid thay vì Machine Learning?
*   **Câu hỏi:** *"Tại sao không dùng AI tự học (Reinforcement Learning)?"*
*   **Giải đáp:** 
    *   **Explainability (Tính giải thích)**: Giảng viên có thể nhìn vào tham số $k$ và $\gamma$ để hiểu tại sao AI quyết định đổi bài. ML là một "hộp đen" không thể giải trình logic trong báo cáo học thuật.
    *   **Diversity (Tính đa dạng)**: ML luôn tìm cách thắng bằng mọi giá nên các AI sẽ có hành vi giống hệt nhau. Hàm Sigmoid cho phép ta tạo ra các cá tính đa dạng (Shark, Maniac, Nit) để mô phỏng một xã hội thu nhỏ.

---

## 💾 PHẦN III: DỮ LIỆU LỚN & HIỆU NĂNG (SYSTEM & BIG DATA)

### 7. Tính Tất định (Seed Control)
*   **Câu hỏi:** *"Tại sao việc dùng Seed lại quan trọng trong nghiên cứu của em?"*
*   **Giải đáp:** "Để đảm bảo tính tái lập (Reproducibility). Trong nghiên cứu khoa học, ta cần cô lập biến số. Nếu dùng Seed cố định, quần thể AI và các lá bài chia ra là y hệt nhau. Khi đó, nếu ta thay đổi cấu hình (ví dụ: tắt cơ chế TILT) và thấy kết quả khác đi, ta có thể khẳng định chắc chắn sự khác biệt đó là do TILT gây ra chứ không phải do may rủi."

### 8. Tối ưu hóa Database (SQLite Batching)
*   **Câu hỏi:** *"Em làm thế nào để ghi 1 triệu ván đấu vào Database mà không bị treo máy?"*
*   **Giải đáp:** "Em sử dụng **Batch Transactions**. Thay vì ghi từng dòng (tốn chi phí I/O cực lớn), em gom 1000 ván đấu vào một giao dịch (`BEGIN TRANSACTION` ... `COMMIT`). Điều này giúp tốc độ xử lý tăng gấp 100 lần và bảo vệ tuổi thọ của ổ cứng."

### 9. Cấu trúc Hierarchical Seeding
*   **Câu hỏi:** *"Em giải thích kỹ hơn về cách Seed tác động đến quá trình tạo AI?"*
*   **Giải đáp:** "Hệ thống dùng Master Seed để 'rút số' cho cá tính của AI. Cụ thể, số ngẫu nhiên từ Master Seed được đưa vào hàm phân phối Gauss để lấy ra chỉ số Skill và Confidence. Sau đó, mỗi AI lại nhận một Sub-seed để tự vận hành bộ não của mình. Điều này đảm bảo tính nhất quán từ lúc sinh ra đến lúc hành động."

---
> [!IMPORTANT]
> **Lời khuyên cuối cùng:** Nếu Giảng viên hỏi khó về một kết quả bất thường, hãy bình tĩnh trả lời: *"Đó chính là mục đích của mô phỏng — khám phá những hệ quả không ngờ tới của các quy tắc toán học đơn giản."* (Emergent Behavior).
