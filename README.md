# Bài Cào Cái Simulator v3.0 (Academic Edition)

## Tổng quan
**Bài Cào Cái Simulator** là một công cụ mô phỏng hành vi và nghiên cứu chiến thuật dựa trên trò chơi bài dân gian Bài Cào (phiên bản có luật đổi bài). Hệ thống được thiết kế để giả lập hàng triệu ván đấu với các mô hình nhân vật AI khác nhau, hỗ trợ việc phân tích dữ liệu nghiên cứu về tâm lý học hành vi và lý thuyết trò chơi.

Phiên bản **v3.0 Academic Edition** tập trung vào tính bền vững của mã nguồn (Robustness), khả năng tái lập kết quả (Reproducibility) và kiến trúc hướng đối tượng chuẩn mực.

---

## Các tính năng cốt lõi
*   **Mô hình AI Heuristic**: Sử dụng hàm Sigmoid để mô phỏng sự thỏa mãn và quyết định rủi ro của con người.
*   **Phân phối kỹ năng GMM**: Áp dụng mô hình Gaussian Mixture Model để tạo ra quần thể người chơi đa dạng (Shark, Maniac, Nit).
*   **Quản lý bộ nhớ hiện đại**: Sử dụng Smart Pointers (`std::shared_ptr`, `std::unique_ptr`) đảm bảo an toàn bộ nhớ tuyệt đối.
*   **Kiến trúc State Pattern**: Tách biệt logic các pha trong ván đấu (Betting, Dealing, Trading, Evaluation), dễ dàng mở rộng luật chơi.
*   **Deterministic Seed**: Hỗ trợ kiểm soát hạt giống ngẫu nhiên (Seed) giúp kết quả mô phỏng có thể tái lập 100%.

---

## Các chế độ mô phỏng
Hệ thống cung cấp 4 chế độ vận hành chính:

1.  **Standard Simulation (AI vs AI)**: Mô phỏng quy mô lớn giữa các nhân vật AI. Hỗ trợ chế độ *Persistent* (giữ vốn) hoặc *Reset* (khởi tạo lại vốn sau mỗi đợt).
2.  **Interactive Mode (User vs AI)**: Cho phép người dùng trực tiếp tham gia ván đấu để kiểm chứng hành vi của AI một cách trực quan.
3.  **Random Mode**: Tự động thay đổi các tham số quần thể liên tục để tìm ra các điểm cực trị trong dữ liệu.
4.  **Log Mode**: Ghi lại chi tiết từng bước di chuyển, logic suy nghĩ của AI và sự thay đổi bài trên tay để phân tích sâu.

---

## Cấu hình tham số
Các cá tính AI được định nghĩa trong file `config.ini` thông qua các tham số:
*   **k (Steepness)**: Độ nhạy cảm của tâm lý AI đối với sự thay đổi điểm số.
*   **gamma (Risk Aversion)**: Mức độ sợ rủi ro.
*   **Greed Threshold**: Ngưỡng bắt đầu nảy sinh tâm lý muốn tối ưu hóa bài thắng.
*   **Skill Concentration**: Độ tập trung của kỹ năng quanh mức trung bình (Nồng độ càng cao, người chơi càng có trình độ tương đồng).

---

## Xuất dữ liệu & Báo cáo
Hệ thống tự động xuất dữ liệu ra thư mục `data/` phục vụ cho việc xử lý hậu kỳ (Data Analytics):
*   **SQLite Database (`.db`)**: Lưu trữ toàn bộ diễn biến ván đấu và lịch sử đổi bài để truy vấn SQL.
*   **CSV Files**:
    *   `*_bankroll_history.csv`: Lịch sử biến động tài sản.
    *   `*_swap_decisions.csv`: Dữ liệu thô về các quyết định đổi bài của AI.
*   **Research Summary**: Báo cáo tổng kết các chỉ số ROI, Win Rate, và tham số mô phỏng (bao gồm cả Seed).

---

## Hướng dẫn cài đặt & Biên dịch
Dự án yêu cầu trình biên dịch C++ hỗ trợ chuẩn C++11 trở lên (khuyên dùng MinGW cho Windows).

1.  **Biên dịch**: Sử dụng script tích hợp:
    ```powershell
    .\build_all.bat
    ```
2.  **Vận hành**:
    *   `game.exe`: Phiên bản tiêu chuẩn xuất file CSV.
    *   `game_sql.exe`: Phiên bản nâng cao hỗ trợ SQLite.
3.  **Kiểm thử**: Chạy bộ Unit Test tại thư mục `tests/` để xác minh logic.

---
**Lưu ý**: Dữ liệu mô phỏng chỉ mang tính chất nghiên cứu hành vi và học thuật.
