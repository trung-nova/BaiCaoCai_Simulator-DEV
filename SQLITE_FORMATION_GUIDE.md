# HƯỚNG DẪN DỮ LIỆU SQLITE (SQLITE DATA FORMATION GUIDE - CHI TIẾT)

Tài liệu này chi tiết hóa cấu trúc quan hệ, thuật toán lưu trữ và ý nghĩa của từng tham số trong cơ sở dữ liệu SQLite của dự án mô phỏng.

---

## 1. Bản chất quan hệ dữ liệu (Relational Logic)

Hệ thống sử dụng SQLite để duy trì **Tính nhất quán (Consistency)** giữa các tầng dữ liệu khác nhau. Mối quan hệ được hình thành qua `RoundID`:

### 1.1. Bảng `rounds` (Master Table)
Mỗi dòng đại diện cho một ván bài đã kết thúc.
- **id**: Khóa chính (Primary Key). Tăng dần theo thời gian thực của simulation.
- **dealer**: Tên người chơi giữ vai trò Nhà Cái. Thuật toán chọn Nhà Cái là chọn ngẫu nhiên trong danh sách người chơi chưa bị loại.
- **pot**: Tổng tiền trong quỹ. Thuật toán: $Pot = \text{Số người chơi} \times Ante$.
- **winners**: Số người thắng Nhà Cái. Dùng để tính toán tỉ lệ thắng tổng quát.
- **scores**: Thuật toán Serialization chuyển đổi đối tượng `Map<string, int>` thành chuỗi văn bản. 
    - *Ví dụ*: `Dealer:8, AI_1:9, AI_2:4` -> Cho biết AI_1 thắng, AI_2 thua.

### 1.2. Bảng `swaps` (Detail Table)
Lưu trữ chi tiết từng lượt ra quyết định.
- **round_id**: Khóa ngoại (Foreign Key) liên kết chặt chẽ với bảng `rounds`.
- **satisfaction / desire / probability**: (Đã giải thích chi tiết thuật toán tại file CSV Guide).
- **decision**: Lưu trữ dưới dạng số nguyên (0: Skip, 1: Trade, 2: Stay).
- **score_before / score_after**: Chụp ảnh điểm số trước và sau sự kiện để đo lường "Giá trị gia tăng" của hành động đổi bài.
- **card_out / card_in**: Lưu vết lá bài được trao đổi.

---

## 2. Các Thuật toán Lưu trữ và Hình thành dữ liệu

### 2.1. Thuật toán Tối ưu hóa Ghi (High-Performance Logging)
Vì SQLite là hệ quản trị dữ liệu dạng file, việc ghi từng dòng sẽ tạo ra độ trễ I/O rất lớn.
- **Thuật toán**: **Page Caching & Transactional Batching**.
- **Quy trình**: 
    1. Khi khởi động simulation: Gọi `BEGIN TRANSACTION`.
    2. Trong quá trình chơi: Các hàm `insertSwap`, `insertRound` chỉ ghi dữ liệu vào Memory Page của SQLite.
    3. Sau 1000 ván hoặc khi kết thúc: Gọi `COMMIT` (End Transaction).
- **Lợi ích**: Giúp hệ thống có thể chạy mô phỏng 100,000 ván bài chỉ trong vài giây mà không bị treo máy.

### 2.2. Thuật toán Hình thành `Round Summary` (Scores String)
Để lưu điểm số của 10-17 người chơi vào 1 cột duy nhất mà vẫn dễ đọc:
- **Thuật toán**: **String Concatenation with Delimiters**.
- **Quy trình**: Duyệt qua danh sách Player -> Lấy tên + ":" + Điểm số -> Nối bằng dấu phẩy. 
- **Mục đích**: Giảm thiểu việc phải tạo thêm một bảng `scores` riêng biệt, giúp truy vấn tổng quát nhanh hơn.

---

## 3. Ý nghĩa tham số trong Phân tích học thuật (Academic Insight)

Dữ liệu SQLite được hình thành để trả lời các câu hỏi nghiên cứu sau:

### 3.1. Phân tích Hiệu quả Chiến thuật (Strategy ROI)
- **Tham số**: `score_after - score_before`.
- **Câu hỏi**: "Nhóm AI nào (Shark/Maniac) có tỉ lệ tăng điểm sau khi đổi bài cao nhất?"

### 3.2. Phân tích Sự tự tin thái quá (Overconfidence Analysis)
- **Tham số**: `probability` vs `satisfaction`.
- **Câu hỏi**: "AI có quyết định đổi bài (`decision=1`) ngay cả khi điểm đã cao (`satisfaction > 0.8`) không? Nếu có, đó là do tham số tâm lý nào chi phối?"

### 3.3. Phân tích Dằn bài (Stay Behavior)
- **Tham số**: `decision = 2`.
- **Câu hỏi**: "Tại lượt mấy thì AI thường quyết định dằn bài nhất? Điểm số trung bình khi dằn bài là bao nhiêu?"

---

## 4. Bảo mật và Tính toàn vẹn (ACID)
Dữ liệu SQLite đảm bảo:
- **Atomicity**: Một ván bài hoặc là được lưu đầy đủ (cả round và swap), hoặc là không lưu gì nếu xảy ra lỗi.
- **Consistency**: Điểm số ghi trong `swaps` và `rounds` luôn phải khớp nhau.
- **Isolation**: Các tiến trình ghi dữ liệu không làm ảnh hưởng đến hiệu năng tính toán của AI.
- **Durability**: Dữ liệu được ghi xuống đĩa cứng bền vững, có thể mở bằng bất kỳ trình xem SQLite nào (ví dụ: DB Browser for SQLite).
