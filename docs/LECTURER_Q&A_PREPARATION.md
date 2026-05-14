# TỔNG HỢP NGHỊCH LÝ & BỘ CÂU HỎI PHẢN BIỆN (MASTER Q&A GUIDE)
> **Dành cho buổi bảo vệ Đồ án OOP - Hệ thống mô phỏng Bài Cào Cái v3.0**

---

## 📂 PHẦN I: CÁC NGHỊCH LÝ VÀ BĂN KHOĂN TOÁN HỌC

### 1. Nghịch lý Kỹ năng (The High-Skill Paradox)
*   **Câu hỏi:** *"Tại sao trong kết quả mô phỏng, có những AI 'Shark' kỹ năng 0.9 lại thua lỗ nặng hơn cả AI 'Normal'?"*
*   **Giải đáp:** 
    1.  **Chiến thuật Săn Ba Tiên**: Shark "quá thông minh" nên thường vứt bỏ các bộ bài 8-9 điểm để theo đuổi bộ Ba Tiên (xác suất thắng tuyệt đối nhưng tỉ lệ bốc trúng cực thấp ~7%). Việc đánh đổi EV (giá trị kỳ vọng) dương lấy một cơ hội mong manh là nguyên nhân gây cháy túi.
    2.  **Lợi thế Hòa (House Edge)**: House Edge 10% (Hòa nhà cái thắng) là rào cản toán học không thể vượt qua. Kỹ năng chỉ giúp tối ưu hóa ván đấu, không thể thắng được quy luật của sàn đấu trong dài hạn.

### 2. Sự bất thường của Tỉ lệ thắng (Win Rate ~42%)
*   **Câu hỏi:** *"Tại sao Win Rate trung bình chỉ khoảng 42% mà không phải 50% hay cao hơn?"*
*   **Giải đáp:** 
    1.  Do quy tắc **Tie-break (Hòa nhà cái thắng)**.
    2.  Khi làm Nhà con, xác suất thắng lý thuyết chỉ là ~45%. Khi luân phiên làm Nhà cái, tỉ lệ thắng của một người trước tất cả những người còn lại là cực thấp. Tổng hợp lại, con số 42% là phản ánh chính xác sự "bào mòn" của nhà cái đối với người chơi.

### 3. Tác động của cơ chế TILT
*   **Câu hỏi:** *"Cơ chế TILT có làm hỏng tính logic của mô phỏng không?"*
*   **Giải đáp:** "Không, trái lại nó làm mô phỏng thực tế hơn. TILT mô phỏng hiện tượng **Gambler's Fallacy** – khi người chơi thua quá nhiều sẽ mất lý trí (giảm Skill) nhưng lại tự tin ảo (tăng Confidence) để đánh liều. Đây là biến số tâm lý giúp giải thích tại sao người chơi giỏi vẫn có thể phá sản."

---

## 🛠️ PHẦN II: KIẾN TRÚC OOP & KỸ THUẬT C++

### 4. Tính Tất định và Seed (Determinism)
*   **Câu hỏi:** *"Tại sao em lại nhấn mạnh vào việc sử dụng Seed?"*
*   **Giải đáp:** "Để đảm bảo tính khoa học. Trong nghiên cứu, chúng ta cần cô lập biến số. Nếu dùng Seed cố định, quần thể AI và các quân bài chia ra là y hệt nhau. Khi đó, nếu ta thay đổi 1 tham số (ví dụ: tắt TILT) và thấy kết quả khác đi, ta có thể khẳng định 100% sự khác biệt đó là do tham số đó gây ra."

### 5. Tại sao dùng Heuristic (Sigmoid) thay vì Machine Learning?
*   **Câu hỏi:** *"Tại sao không dùng Reinforcement Learning cho AI tự học?"*
*   **Giải đáp:** 
    1.  **Tính giải thích được (Explainability)**: Giảng viên có thể nhìn vào hàm Sigmoid để hiểu tại sao AI quyết định như vậy. ML là "hộp đen", rất khó bảo vệ về mặt thuật toán.
    2.  **Tính đa dạng**: ML sẽ hội tụ về 1 cách đánh tối ưu duy nhất. Heuristic cho phép giả lập nhiều cá tính (Shark, Maniac, Nit) cùng lúc để quan sát sự tương tác.

### 6. Quản lý bộ nhớ và Smart Pointers
*   **Câu hỏi:** *"Em xử lý vấn đề rò rỉ bộ nhớ như thế nào khi chạy hàng triệu ván đấu?"*
*   **Giải đáp:** "Em sử dụng **Smart Pointers** (`std::shared_ptr` và `std::unique_ptr`). Các đối tượng Player và GameState sẽ tự động được giải phóng khi không còn tham chiếu, đảm bảo RAM không tăng lên theo số ván đấu (O(N) về không gian)."

---

## 📊 PHẦN III: DỮ LIỆU LỚN & PHÂN TÍCH (BIG DATA)

### 7. Tối ưu hóa ghi dữ liệu (SQLite Batching)
*   **Câu hỏi:** *"Ghi hàng triệu dòng vào Database có làm chậm hệ thống không?"*
*   **Giải đáp:** "Có, nếu ghi từng dòng. Do đó em sử dụng **Batch Transactions**: Gom 1000 ván đấu vào một lần commit. Điều này giúp tăng tốc độ ghi lên gấp 50-100 lần so với thông thường."

### 8. Phân phối kỹ năng (Gaussian Mixture Model)
*   **Câu hỏi:** *"Tại sao kỹ năng AI không rải đều từ 0 đến 1?"*
*   **Giải đáp:** "Vì trong thực tế, người chơi thường tập trung thành các nhóm trình độ. Em sử dụng mô hình GMM để tạo ra các 'đỉnh' kỹ năng quanh mức 0.4 (Maniac), 0.7 (Nit) và 0.9 (Shark), phản ánh đúng cấu trúc xã hội của một sòng bài thực tế."

---
> [!TIP]
> **Mẹo ghi điểm:** Khi trả lời, hãy luôn liên hệ giữa **Code (OOP)** -> **Toán học (Sigmoid/Seed)** -> **Ý nghĩa thực tế (Tâm lý học)**. Điều này cho thấy bạn hiểu sâu sắc mọi khía cạnh của dự án.
