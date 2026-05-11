# Bài Cào Cái Simulator v3.0 - Research & Analysis Guide

Chào ông bạn! Hệ thống đã được nâng cấp lên phiên bản **v3.0 (Academic Edition)** với kiến trúc lập trình hướng đối tượng (OOP) chuẩn mực, sẵn sàng cho việc bảo vệ đồ án và phân tích nghiên cứu chuyên sâu.

## 1. Tính năng mới (v3.0)
*   **Encapsulation (Bao đóng)**: Bảo vệ dữ liệu người chơi tuyệt đối thông qua Access Modifiers (`protected`).
*   **Smart Memory Management**: Sử dụng `std::shared_ptr` và `std::unique_ptr` để quản lý bộ nhớ, loại bỏ hoàn toàn lỗi Memory Leak.
*   **Deterministic Simulation**: Hỗ trợ nạp `seed` từ `config.ini` hoặc nhập tay để tái lập chính xác kết quả mô phỏng (Reproducibility).
*   **Unit Testing**: Tích hợp bộ kiểm thử `tests/test_rules.cpp` để xác minh luật chơi.
*   **Robust Build System**: Script `build_all.bat` tự động đồng bộ cả 2 phiên bản CSV và SQLite với khả năng báo lỗi thông minh.

## 2. Cấu trúc Dữ liệu (Data Structure)
Dữ liệu được xuất ra thư mục `data/` dưới hai định dạng chính:

### A. SQLite Database (`simulation_results.db`)
Cấu trúc gồm 2 bảng chính:
*   **`rounds`**: Tổng quan ván đấu (ID, Dealer, Pot, Winners, Scores Summary).
*   **`swaps`**: Chi tiết quyết định AI (RoundID, Player, Turn, Satisfaction, Desire, Probability, Result).

### B. Các file CSV & Reports
*   `*_bankroll_history.csv`: Theo dõi biến động tài sản của từng người chơi.
*   `*_summary.txt`: Báo cáo nghiên cứu tổng quát kèm tham số mô phỏng (bao gồm cả Seed).

## 3. Các nhóm AI (Archetypes)
Được cấu hình trong `config.ini`:
1.  **SHARK**: Kỹ năng cao, lý trí, tối ưu hóa lợi nhuận dài hạn. (Tập trung quanh `maxSkill`).
2.  **MANIAC**: Rất hung hãn, dễ bị ảnh hưởng bởi tâm lý "TILT". (Tập trung quanh `minSkill`).
3.  **NIT**: Cực kỳ thận trọng, ưu tiên bảo toàn vốn. (Tập trung quanh mức trung bình).

> [!NOTE]
> Hệ thống sử dụng mô hình **Gaussian Mixture Model (GMM)** để tạo ra một quần thể người chơi có tính phân hóa xã hội cao, thay vì chỉ là một dải kỹ năng ngẫu nhiên đơn thuần.

## 4. Hướng dẫn Build & Chạy
1.  Đảm bảo đã cài đặt `g++` (MinGW) và `sqlite3`.
2.  Chạy file `build_all.bat` để biên dịch.
3.  Chạy `game.exe` (CSV) hoặc `game_sql.exe` (SQLite + CSV).

---
*Mọi logic về thuật toán AI Sigmoid và phân bổ xác suất được giữ nguyên 100% so với bản v2.0 để đảm bảo tính nhất quán của dữ liệu nghiên cứu.*
