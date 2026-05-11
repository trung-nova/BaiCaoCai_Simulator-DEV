# TÀI LIỆU CHUẨN BỊ THUYẾT TRÌNH (DÀNH CHO GIẢNG VIÊN)
**Dự án: Mô phỏng Bài Cào Cái v3.0 (Academic Edition)**

Tài liệu này tổng hợp các điểm kỹ thuật cao cấp đã được triển khai trong phiên bản v3.0 và các phương án trả lời để ghi điểm tuyệt đối với Giảng viên.

---

## 1. Tính Bao đóng & Đóng gói (Encapsulation)
*   **Vấn đề:** Tránh việc truy cập dữ liệu tùy tiện từ bên ngoài.
*   **Câu hỏi dự kiến:** *"Làm sao em đảm bảo tính đóng gói trong dự án này?"*
*   **Trả lời:** 
    *   "Trong phiên bản v3.0, em đã đưa toàn bộ các thuộc tính nhạy cảm của `Player` (số dư, bài trên tay, chỉ số kỹ năng) về phạm vi `protected`."
    *   "Em sử dụng cơ chế `friend class` cho `GameManager` và các `State` trong State Machine. Điều này cho phép các lớp quản lý logic có quyền truy cập đặc biệt để điều hành ván đấu, trong khi vẫn ngăn chặn các đối tượng bên ngoài can thiệp sai lệch vào trạng thái người chơi."

## 2. Quản lý Bộ nhớ (Modern Memory Management)
*   **Vấn đề:** Tránh rò rỉ bộ nhớ (Memory Leak) và lỗi con trỏ lạc (Dangling Pointers).
*   **Câu hỏi dự kiến:** *"Em quản lý con trỏ và bộ nhớ động như thế nào?"*
*   **Trả lời:** 
    *   "Em đã loại bỏ hoàn toàn việc quản lý con trỏ thô bằng tay. Thay vào đó, em sử dụng **Smart Pointers** của chuẩn C++11/14/17."
    *   "Cụ thể: `std::shared_ptr` được dùng cho danh sách người chơi vì chúng được chia sẻ giữa Manager và các States. `std::unique_ptr` được dùng cho các trạng thái trong State Machine (Design Pattern: State) để đảm bảo tại mỗi thời điểm chỉ có một trạng thái nắm quyền sở hữu và tự giải phóng khi chuyển đổi."

## 3. Tính Lặp lại & Kiểm chứng (Reproducibility)
*   **Vấn đề:** Tính khoa học của dữ liệu mô phỏng.
*   **Câu hỏi dự kiến:** *"Kết quả mô phỏng của em có thể tái lập (reproducible) để kiểm chứng không?"*
*   **Trả lời:** 
    *   "Có ạ. Em đã triển khai hệ thống **Deterministic Seed Control**. Người dùng có thể nạp một Seed cố định từ file `config.ini` hoặc nhập từ bàn phím."
    *   "Hạt giống này sẽ kiểm soát toàn bộ chuỗi số ngẫu nhiên từ việc chia bài đến quyết định của AI, đảm bảo nếu dùng cùng một Seed, kết quả mô phỏng sẽ giống hệt nhau 100%."

## 4. Kiểm thử Đơn vị (Unit Testing)
*   **Vấn đề:** Xác minh độ chính xác của logic cốt lõi.
*   **Câu hỏi dự kiến:** *"Làm sao em chắc chắn các luật chơi đặc biệt như Ba Tiên hay tính điểm 10 luôn đúng?"*
*   **Trả lời:** 
    *   "Em đã xây dựng một bộ Unit Test riêng biệt tại thư mục `tests/test_rules.cpp`."
    *   "Bộ test này sử dụng các khẳng định (`assert`) để kiểm tra các trường hợp biên (edge cases) như: bộ bài 3 lá hình (Ba Tiên), bộ bài tổng điểm là 10 (0 điểm), và các logic hoán vị bài giữa các người chơi."

## 5. Thiết kế State Machine (Design Pattern)
*   **Vấn đề:** Sự mạch lạc của luồng điều khiển.
*   **Câu hỏi dự kiến:** *"Tại sao em lại tách logic ván đấu ra các lớp Betting, Dealing, Trading, Eval?"*
*   **Trả lời:** 
    *   "Đây là ứng dụng của **State Pattern**. Việc tách biệt này giúp mã nguồn tuân thủ nguyên lý **Single Responsibility** (Mỗi lớp chỉ làm một nhiệm vụ)."
    *   "Nó giúp việc bảo trì và mở rộng luật chơi dễ dàng hơn. Ví dụ, nếu muốn thêm lượt 'Tố' (Betting) bổ sung, em chỉ cần tạo thêm một State mới mà không cần sửa đổi cấu trúc lớn của hệ thống."

---

## 6. BÀI NÓI MẪU CHO PHẦN AI (HEURISTIC VS MACHINE LEARNING)
*Phần này cực kỳ quan trọng để giảng viên thấy ông có tư duy hệ thống và hiểu về Data Science.*

**Giảng viên hỏi:** *"Tại sao em không dùng các thuật toán AI hiện đại như Reinforcement Learning để Bot tự học cách thắng?"*

**Ông trả lời:**
"Thưa Thầy/Cô, em chọn mô hình Heuristic dựa trên hàm Sigmoid vì 3 lý do chiến thuật:
1.  **Mục tiêu là Mô phỏng Quần thể (Population Simulation):** Nếu dùng Học máy tối ưu, các Bot sẽ đều đánh giống nhau sau một thời gian. Mô hình của em cho phép giả lập sự đa dạng: có người chơi lý trí (Shark), có người chơi hung hãn (Maniac) và người chơi thận trọng (Nit).
2.  **Tính Giải thích được (Explainability):** Trong nghiên cứu dữ liệu, 'Hộp đen' (Black Box) của Neural Network rất khó giải thích. Với hàm Sigmoid, em có thể vẽ biểu đồ và chỉ ra chính xác 'điểm gãy' tâm lý của từng loại AI dựa trên các tham số $k$ và $\gamma$.
3.  **Hiệu suất:** Mô hình toán học này cho phép em mô phỏng 1 triệu ván đấu trong vài giây trên máy tính cá nhân, giúp việc thu thập dữ liệu lớn (Big Data) để phân tích thống kê trở nên khả thi hơn rất nhiều so với dùng Model nặng."

## 9. Phân phối Kỹ năng (Gaussian Mixture Model)
*   **Vấn đề:** Tại sao kỹ năng không rải đều mà lại tập trung thành các nhóm?
*   **Câu hỏi dự kiến:** *"Em giải thích thế nào về mô hình phân bổ kỹ năng của các AI trong hệ thống?"*
*   **Trả lời:** 
    *   "Hệ thống không sử dụng một phân phối chuẩn đơn lẻ mà sử dụng **Gaussian Mixture Model (GMM)**."
    *   "Dựa trên các Archetype (Shark, Maniac, Nit), hệ thống tạo ra một **phân phối tam đỉnh (Tri-modal Distribution)**. Mỗi đỉnh đại diện cho một tầng lớp kỹ năng trong xã hội người chơi."
    *   "Thông số `Concentration` (Nồng độ) cho phép kiểm soát độ phân hóa: Concentration càng cao, ranh giới giữa các nhóm kỹ năng càng rõ rệt; Concentration thấp sẽ tạo ra một dải kỹ năng liên tục và chồng lấn, phản ánh các thị trường chơi bài có tính cạnh tranh khác nhau."
