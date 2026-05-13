# TÀI LIỆU KỸ THUẬT TỔNG QUAN (TECHNICAL MASTER GUIDE)

Đây là tài liệu hướng dẫn cách biên dịch, vận hành và khai thác dữ liệu từ hệ thống mô phỏng Bài Cào Cái.

## 1. Cấu trúc Tài liệu (Modular Docs)
Để tiện cho việc tra cứu, tài liệu được chia làm 4 phần:
1. [Luật chơi (Game Rules)](./GAME_RULES.md): Cách tính điểm và phân định thắng thua.
2. [Luồng vận hành (Game Flow)](./GAME_FLOW.md): Quy trình State Machine và các bước chia bài/đổi bài.
3. [Phân loại AI (Player Archetypes)](./PLAYER_ARCHETYPES.md): Giải thích các tham số toán học (Sigmoid, Tilt) điều khiển AI.
4. **Hướng dẫn Kỹ thuật (Tài liệu này)**: Build app và cấu trúc dữ liệu.

---

## 2. Hướng dẫn Build & Chạy App

### 2.1. Yêu cầu hệ thống
- **Compiler**: GCC (MinGW cho Windows) hỗ trợ chuẩn C++17.
- **Thư viện**: SQLite3 (đã đính kèm sẵn trong source code).

### 2.2. Cách Build (Windows)
Cách nhanh nhất là sử dụng file batch đã soạn sẵn:
1. Mở Terminal/CMD tại thư mục dự án.
2. Chạy lệnh: `.\build_all.bat`
3. Sau khi hoàn tất, hệ thống sẽ tạo ra file `game_sql.exe`.

*Nếu muốn build thủ công bằng CMake:*
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### 2.3. Cách Vận hành
Chạy file `.exe` vừa tạo. Các file dữ liệu sẽ được sinh ra trong thư mục `data/` (hoặc cùng cấp với file exe).
- Chỉnh sửa `config.ini` để thay đổi tính cách các nhóm AI trước khi chạy.

---

## 3. Cấu trúc Dữ liệu cho Analysis

Hệ thống hỗ trợ xuất dữ liệu ra SQLite (`.db`) để truy vấn và CSV để nạp vào Pandas.

### 3.1. SQLite Database Schema
Bao gồm 2 bảng chính liên kết với nhau qua `round_id`:

#### Bảng `rounds` (Tổng quan ván đấu)
- `id`: ID ván đấu.
- `dealer`: Tên người làm Cái.
- `pot`: Tổng tiền cược.
- `winners`: Số lượng người thắng.
- `scores`: Chuỗi tóm tắt điểm (Ví dụ: "Dealer:5 Player1:7...").

#### Bảng `swaps` (Chi tiết hành vi AI)
- `player`: Tên AI.
- `turn`: Lượt đổi (1, 2, 3).
- `satisfaction`: Chỉ số thỏa mãn (0-1).
- `desire`: Chỉ số thèm muốn đổi bài.
- `probability`: Xác suất quyết định cuối cùng.
- `swapped`: `1` nếu đã đổi, `0` nếu giữ nguyên.

---

## 4. Gợi ý Analysis Recipe (Python)

Dữ liệu có thể được nạp vào Pandas để phân tích như sau:

```python
import sqlite3
import pandas as pd

# Kết nối DB
conn = sqlite3.connect('simulation_results.db')

# Lấy dữ liệu ván đấu
df_rounds = pd.read_sql_query("SELECT * FROM rounds", conn)

# Lấy dữ liệu hành vi AI
df_swaps = pd.read_sql_query("SELECT * FROM swaps", conn)

# Tính ROI trung bình của từng nhóm AI
# (Cần join với bankroll_history.csv)
```

## 5. Đảm bảo Chất lượng (Quality Assurance & Testing)

Để phục vụ bảo vệ đồ án và đảm bảo tính chính xác của mô hình toán học, hệ thống đã được chuẩn hóa và trang bị bộ công cụ kiểm thử tự động.

### 5.1. Chuẩn hóa Cấu trúc (Technical Debt Cleanup)
- **Standardized Includes**: Toàn bộ đường dẫn thư viện đã được đưa về dạng project-root (ví dụ: `#include "Player.h"`) thay vì relative paths (`../include/`). Điều này giúp mã nguồn dễ dàng tương thích với mọi IDE và compiler.
- **Header Guard Verification**: Đảm bảo không xảy ra lỗi định nghĩa chồng chéo khi mở rộng quy mô dự án.

### 5.2. Hệ thống Kiểm thử (Automated Testing)
Dự án cung cấp 2 cấp độ kiểm tra:

1. **Unit Testing (Kiểm thử Logic)**:
   - File: `tests/test_logic.cpp`
   - Mục tiêu: Kiểm tra độ chính xác của các thuật toán: tính điểm Bài Cào, nhận diện "Ba Tiên", quyết định săn "Ba Tiên" của AI, và cơ chế Tilt.
   - Cách chạy: `.\build_tests.bat` -> chạy `test_logic.exe`.

2. **System Integration Test (Kiểm thử Hệ thống)**:
   - File: `tests/system_test_suite.py`
   - Mục tiêu: Chạy giả lập thực tế 20 ván, kiểm tra việc sinh file CSV, tạo SQLite DB, và tính toàn vẹn của dữ liệu xuất ra.
   - Cách chạy: `.\build_tests.bat` sẽ tự động kích hoạt bộ test này.

---

Hy vọng tài liệu này giúp ích cho việc nghiên cứu và phân tích dữ liệu AI của bạn.
