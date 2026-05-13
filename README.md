# 🎴 Bài Cào Cái Simulator v3.0
> **Academic Research Framework for Behavioral Game Theory & AI Simulation**

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)]()

## 📌 Tổng quan
**Bài Cào Cái Simulator** là một hệ thống mô phỏng Monte Carlo hiệu năng cao, được thiết kế để nghiên cứu hành vi ra quyết định rủi ro trong trò chơi Bài Cào. Hệ thống sử dụng mô hình toán học (Sigmoid Utility Functions) để giả lập tâm lý người chơi, hỗ trợ việc phân tích dữ liệu quy mô lớn cho các bài toán kinh tế lượng và lý thuyết trò chơi.

### 🌟 Tính năng Nghiên cứu Trọng tâm
*   **Behavioral AI Modeling**: Sử dụng hàm **Logistic Sigmoid** và **Dual-Process Theory** để mô phỏng sự hài lòng, thèm muốn và sự bộc phát cảm xúc (TILT).
*   **Deterministic Simulation**: Cơ chế **Seeding** phân cấp giúp tái lập chính xác 100% mọi kịch bản mô phỏng để kiểm chứng khoa học.
*   **High-Frequency Data Pipeline**: Xuất dữ liệu đa kênh (CSV, SQLite) với tốc độ cao, sẵn sàng cho phân tích Big Data (Python/Pandas/R).
*   **Archetype System**: Phân loại đối tượng AI thành các nhóm cá tính (Shark, Maniac, Nit) dựa trên phân phối chuẩn (GMM).

---

## 📂 Documentation Portal (Cổng thông tin tài liệu)

Để đảm bảo tính chi tiết và dễ tra cứu cho hội đồng bảo vệ, tài liệu được phân tách thành các module chuyên biệt:

### 🛠️ Kiến trúc & Kỹ thuật
*   [**Kiến trúc OOP & Design Patterns**](./OOP_ARCHITECTURE.md): Chi tiết về Class Diagram, State Pattern và quản lý bộ nhớ.
*   [**Quy trình vận hành (Game Flow)**](./GAME_FLOW.md): Sơ đồ máy trạng thái (State Machine) điều khiển ván đấu.
*   [**Tài liệu Kỹ thuật Tổng quát**](./TECHNICAL_DOCUMENTATION.md): Hướng dẫn biên dịch, cài đặt và cấu trúc hệ thống.

### 📊 Toán học & Dữ liệu
*   [**Mô hình Toán học & Hành vi**](./MATHEMATICAL_ANALYSIS.md): Chi tiết các hàm Sigmoid, chỉ số TILT và xác suất quyết định.
*   [**Từ điển Dữ liệu (Data Dictionary)**](./DATA_DICTIONARY.md): Định nghĩa chi tiết các trường thông tin trong SQLite và CSV.
*   [**Phân loại AI (Archetypes)**](./PLAYER_ARCHETYPES.md): Giải thích các nhóm nhân vật (Shark, Maniac, Nit).

### 🎓 Học thuật & Bảo vệ
*   [**Luật chơi (Game Rules)**](./GAME_RULES.md): Quy tắc tính điểm và lợi thế nhà cái (House Edge).
*   [**Bộ câu hỏi phản biện (Q&A)**](./LECTURER_Q&A_PREPARATION.md): Chuẩn bị cho các câu hỏi từ giảng viên về kiến trúc và thuật toán.

---

## 🏗️ Cấu trúc thư mục (Project Structure)
```text
.
├── include/            # Header files (.h) - Định nghĩa Class và Interface
├── src/                # Source files (.cpp) - Triển khai logic
├── data/               # Output directory - Lưu trữ SQLite (.db) và CSV
├── tests/              # Unit tests - Kiểm thử logic tính điểm và AI
├── config.ini          # File cấu hình tham số quần thể AI
├── build_all.bat       # Script biên dịch nhanh trên Windows
└── MATHEMATICAL_ANALYSIS.md ... # Hệ thống tài liệu chi tiết
```

---

## 🚀 Quick Start
1.  **Biên dịch**: Chạy `.\build_all.bat` (Yêu cầu G++ hỗ trợ C++17).
2.  **Vận hành**: Chạy `game_sql.exe` để bắt đầu mô phỏng và lưu vào Database.
3.  **Phân tích**: Truy cập thư mục `data/` để lấy dữ liệu thô phục vụ nghiên cứu.

---
**Disclaimer**: Dự án phục vụ mục đích nghiên cứu học thuật về tâm lý học hành vi.

