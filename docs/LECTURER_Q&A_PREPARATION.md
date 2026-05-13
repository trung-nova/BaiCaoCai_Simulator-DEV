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

## 10. Phân tích Độ phức tạp (Complexity Analysis)
*   **Vấn đề:** Hiệu năng của hệ thống khi mở rộng quy mô.
*   **Câu hỏi dự kiến:** *"Độ phức tạp của thuật toán trong một ván đấu là bao nhiêu?"*
*   **Trả lời:** 
    *   "Về cơ bản, một ván đấu có độ phức tạp thời gian là **O(N)** với N là số lượng người chơi."
    *   "Tuy nhiên, trong giai đoạn `TradingState` (Đổi bài), việc ghép cặp ngẫu nhiên yêu cầu xáo trộn (Shuffle) danh sách ứng viên, có độ phức tạp **O(N)**. Mỗi người thực hiện tối đa 3 lượt đổi, nên tổng quát vẫn là tuyến tính."
    *   "Về không gian, em sử dụng cơ chế **Recycling Object** (Tái sử dụng đối tượng), nên độ phức tạp không gian chỉ là **O(N)** để lưu trữ trạng thái người chơi, không tăng thêm theo số ván đấu."

## 11. Khả năng mở rộng & Scalability
*   **Vấn đề:** Ứng dụng thực tế của kiến trúc.
*   **Câu hỏi dự kiến:** *"Nếu muốn mô phỏng 1 tỷ ván đấu thay vì 1 triệu, hệ thống của em có gặp nút thắt (bottleneck) nào không?"*
*   **Trả lời:** 
    *   "Nút thắt lớn nhất sẽ là I/O (Ghi dữ liệu xuống ổ đĩa). Trong phiên bản v3.0, em đã tối ưu bằng cách sử dụng **Batch Transactions** cho SQLite (Gom 1000 ván vào một lần ghi) và **Streaming Buffer** cho CSV."
    *   "Nếu lên quy mô 1 tỷ ván, em sẽ chuyển sang kiến trúc **Multi-threading** (Đa luồng), trong đó mỗi luồng quản lý một `GameManager` độc lập và sử dụng một `ConcurrentQueue` để ghi dữ liệu tập trung."

## 12. Nguyên lý Clean Code & SOLID
*   **Vấn đề:** Chất lượng mã nguồn.
*   **Câu hỏi dự kiến:** *"Dự án này tuân thủ các nguyên lý SOLID như thế nào?"*
*   **Trả lời:** 
    *   **Single Responsibility**: Mỗi lớp State chỉ xử lý logic của một pha duy nhất. `DatabaseManager` chỉ lo việc ghi dữ liệu.
    *   **Liskov Substitution**: `AIPlayer` và `HumanPlayer` có thể thay thế hoàn toàn cho `Player` mà không làm hỏng logic của `GameManager`.
    *   **Dependency Inversion**: `GameManager` phụ thuộc vào interface `GameState` (trừu tượng) chứ không phụ thuộc vào các lớp cụ thể như `BettingState`.

## 13. Chỉ số AI & Hành vi (AI Metrics & Behavior)
*   **Vấn đề:** Hiểu về các thông số điều khiển "bộ não" của AI.
*   **Câu hỏi dự kiến:** *"Các chỉ số như Skill, Confidence hay k ảnh hưởng thế nào đến cách AI chơi bài?"*
*   **Trả lời:** 
    *   "Mỗi AI được đặc trưng bởi các chỉ số tâm lý học hành vi: **Skill** (quyết định chơi theo lý trí hay cảm tính), **k** (độ nhạy cảm cảm xúc), và **Midpoint** (ngưỡng hài lòng)."
    *   "Cơ chế cốt lõi là **Hàm Sigmoid**: Nó tính toán mức độ hài lòng ($S$) của AI dựa trên điểm số hiện tại. Nếu $S$ thấp và chỉ số **Aggression** (hung hăng) cao, AI sẽ có xác suất đổi bài lớn hơn."
    *   "Đặc biệt, cơ chế **TILT (Mất bình tĩnh)** sẽ kích hoạt khi AI thua liên tiếp, làm giảm Skill và tăng Confidence ảo, khiến AI đưa ra các quyết định liều lĩnh để gỡ vốn — mô phỏng chính xác tâm lý con bạc thực tế."

## 14. Lý thuyết Trò chơi ứng dụng (Applied Game Theory)
*   **Vấn đề:** Ý nghĩa khoa học rút ra từ kết quả mô phỏng.
*   **Câu hỏi dự kiến:** *"Lý thuyết trò chơi nào được rút ra từ dự án này?"*
*   **Trả lời:** 
    *   **Trò chơi có Tổng âm (Negative-Sum Game):** Kết quả Win Rate hội tụ về ~42% chứng minh lợi thế toán học tuyệt đối của Nhà cái (House Edge). Chiến thuật tối ưu không phải là thắng đậm mà là quản trị rủi ro để tồn tại lâu nhất.
    *   **Hữu dụng hữu hạn (Bounded Rationality):** Dự án chứng minh con người không bao giờ chơi tối ưu 100% vì bị giới hạn bởi năng lực tính toán và cảm xúc (thể hiện qua hàm Sigmoid).
    *   **Sự tiến hóa của chiến thuật (ESS):** Qua hàng triệu ván đấu, nhóm **Shark** (lý trí) luôn có tỉ lệ sống sót và ROI cao nhất, chứng minh rằng trong môi trường rủi ro, sự kỷ luật và lý trí là chiến thuật ổn định nhất (Evolutionary Stable Strategy)."

## 15. Quy trình Kiểm định & Làm sạch (Verification & Technical Debt)
*   **Vấn đề:** Tính chuyên nghiệp và ổn định của mã nguồn.
*   **Câu hỏi dự kiến:** *"Làm sao em đảm bảo dự án này không có lỗi logic tiềm ẩn khi chạy hàng triệu ván đấu?"*
*   **Trả lời:** 
    *   **Chuẩn hóa Mã nguồn (Standardization):** "Em đã thực hiện một đợt làm sạch nợ kỹ thuật (Technical Debt Cleanup), chuẩn hóa toàn bộ cấu trúc include theo project-root. Điều này giúp loại bỏ hoàn toàn các lỗi xung quanh việc liên kết file (Linker errors)."
    *   **Kiểm thử Đa tầng (Multi-layer Testing):** "Hệ thống có bộ test tự động gồm: **Unit Test** (kiểm tra logic toán học của từng quân bài, cách tính điểm) và **Integration Test** (sử dụng Python script để kiểm tra việc ghi dữ liệu SQLite/CSV có bị lỗi hay mất mát thông tin không)."
    *   **Báo cáo Phân tích (Automated Reporting):** "Mỗi lượt chạy đều sinh ra Summary Report với cơ chế tính toán độ dài chuỗi tự động, đảm bảo dữ liệu trình bày luôn thẳng hàng và chính xác, giúp người dùng (nhà nghiên cứu) dễ dàng kiểm tra nhanh các con số bất thường."
