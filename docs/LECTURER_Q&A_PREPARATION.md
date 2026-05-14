# CHIẾN LƯỢC PHẢN BIỆN: GIẢI ĐÁP CÁC CÂU HỎI HÓA (LECTURER Q&A)

Tài liệu này chuẩn bị cho bạn những câu trả lời "vừa lòng" nhất cho giảng viên, tập trung vào các quyết định kỹ thuật quan trọng.

---

## 🏗️ KIẾN TRÚC & OOP

### 1. Tại sao em lại chuyển từ SQLite sang CSV?
- **Câu trả lời**: 
    - "Để tăng tính **Minh bạch (Transparency)** và **Di động (Portability)** của dữ liệu nghiên cứu. CSV cho phép giảng viên hoặc người dùng kiểm tra dữ liệu ngay lập tức bằng Excel hoặc Notepad mà không cần cài đặt trình quản lý Database."
    - "Việc sử dụng CSV cũng giúp loại bỏ các phụ thuộc thư viện bên ngoài (External Dependencies), giúp dự án có thể build và chạy trên bất kỳ máy tính nào chỉ với trình biên dịch C++ chuẩn."

### 2. Em xử lý Archetype bằng String thay vì Enum, liệu có làm chậm chương trình?
- **Câu trả lời**: 
    - "Về mặt hiệu năng, việc tra cứu trong `std::map<std::string, ...>` tốn thời gian hơn `enum` một chút, nhưng trong bối cảnh mô phỏng xã hội, tính **Linh hoạt (Flexibility)** quan trọng hơn."
    - "Cơ chế này cho phép hệ thống trở nên **Data-Driven** hoàn toàn. Chúng ta có thể thêm hàng trăm loại AI mới chỉ bằng cách sửa file cấu hình mà không cần biên dịch lại mã nguồn. Đây là minh chứng cho nguyên lý **Open/Closed** trong OOP."

### 3. State Pattern giúp gì cho việc mở rộng luật chơi?
- **Câu trả lời**: 
    - "Nếu muốn thêm luật chơi mới như 'Vòng cược bổ sung' hoặc 'Gấp đôi tiền cược', em chỉ cần tạo một lớp kế thừa từ `GameState` và chèn vào luồng chuyển trạng thái hiện tại. Logic của các giai đoạn cũ hoàn toàn không bị ảnh hưởng."

---

## 🧠 TOÁN HỌC & AI

### 4. Tại sao AI của em lại có lúc đưa ra quyết định sai lầm (Sub-optimal)?
- **Câu trả lời**: 
    - "Đó là chủ ý thiết kế để mô phỏng **Tính hữu dụng hữu hạn (Bounded Rationality)** của con người. Con người không phải là máy tính tối ưu 100%. Bằng cách sử dụng xác suất kết hợp với hàm Sigmoid, AI sẽ có những 'sai số' hành vi, làm cho mô hình mô phỏng trở nên thực tế hơn."

### 5. Cơ chế TILT (Cay cú) hoạt động như thế nào?
- **Câu trả lời**: 
    - "Đây là một **State-based behavior**. Khi các điều kiện về tài chính hoặc lịch sử thua cuộc bị vi phạm, thuộc tính `isTilt` của Player sẽ kích hoạt. Lúc này, các tham số đầu vào của hàm Sigmoid bị bẻ cong (giảm Skill, tăng Confidence ảo), tạo ra các quyết định liều lĩnh - một hiện tượng rất phổ biến trong cờ bạc."

---

## 💾 HỆ THỐNG & DỮ LIỆU

### 6. Làm sao em đảm bảo dữ liệu giữa 4 file CSV luôn khớp nhau?
- **Câu trả lời**: 
    - "Toàn bộ việc ghi dữ liệu được quản lý tập trung bởi lớp `GameManager`. Mọi thông tin đều được đóng gói trong các tệp CSV dùng chung một **Timestamp** duy nhất cho mỗi phiên chạy, đảm bảo tính nhất quán (Consistency) tuyệt đối khi nạp vào các công cụ phân tích dữ liệu."

### 7. Tính tất định (Deterministic) có ý nghĩa gì trong nghiên cứu?
- **Câu trả lời**: 
    - "Nhờ cơ chế Seeding phân cấp, nếu giảng viên phát hiện một lỗi logic ở ván thứ 1000, em có thể nhập lại đúng Seed đó để tái hiện lại chính xác 100% tình huống đó để gỡ lỗi. Điều này cực kỳ quan trọng trong nghiên cứu khoa học để đảm bảo tính **Kiểm chứng (Verifiability)**."
