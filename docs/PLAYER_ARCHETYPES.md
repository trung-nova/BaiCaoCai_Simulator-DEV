# PHÂN LOẠI VÀ TÂM LÝ AI (PLAYER ARCHETYPES)

Hệ thống mô phỏng Bài Cào Cái cho phép tùy biến không giới hạn các loại cá tính AI thông qua file cấu hình.

## 1. Cơ chế Archetype Động (Dynamic Profiles)

Khác với các hệ thống truyền thống, danh sách các nhóm AI không bị giới hạn trong mã nguồn. Hệ thống sẽ tự động quét file `config.ini` và nhận diện các Section mới làm tên cho Archetype.

**Ví dụ cấu hình trong `config.ini`:**
```ini
[SHARK]
k=2.5
gamma=2.2
greed_threshold=0.1
min_skill=0.8
max_skill=1.0
```

## 2. Các tham số tâm lý điều khiển AI

Mỗi Archetype được định nghĩa bởi các tham số toán học sau:

| Tham số | Ý nghĩa tâm lý |
| :--- | :--- |
| **`k`** | **Độ nhạy cảm cảm xúc**: `k` càng cao, AI càng phản ứng cực đoan với điểm số (vui/buồn nhanh). |
| **`gamma`** | **Độ ngại rủi ro (Risk Aversion)**: `gamma` cao khiến AI khó thỏa mãn, đòi hỏi điểm cao hơn mới dám "Dằn". |
| **`greed_threshold`** | **Tính tham lam**: Quyết định mức độ tăng áp lực muốn đổi bài (`Trade Desire`) qua các lượt. |
| **`min/max_skill`** | **Khoảng kỹ năng**: Xác định năng lực tính toán lý trí của nhóm AI đó. |

---

## 3. Mô hình Quyết định (Decision Model)

AI đưa ra quyết định dựa trên sự kết hợp giữa **Lý trí** và **Cảm xúc**:

1.  **Hàm Sigmoid (Cảm xúc)**: Tính độ thỏa mãn $S$ dựa trên điểm hiện tại.
    - $S \approx 1$: Bài đẹp, rất hài lòng.
    - $S \approx 0$: Bài xấu, muốn đổi ngay.
2.  **Tính toán Lý trí**: AI giỏi (`skill` cao) sẽ biết rằng bài 5-6 điểm vẫn có cơ hội thắng và sẽ cân nhắc đổi bài dựa trên xác suất thực tế thay vì cảm xúc nhất thời.
3.  **Hệ quả**: Quyết định cuối cùng là một biến ngẫu nhiên có trọng số, mô phỏng sự không chắc chắn của con người (không phải lúc nào cũng chọn phương án tối ưu).

---

## 4. Trạng thái TILT (Cay cú)

Hệ thống ghi nhận và mô phỏng trạng thái mất kiểm soát của AI khi:
- Thua liên tiếp quá nhiều ván.
- Hoặc số dư tài khoản sụt giảm nghiêm trọng.

**Khi TILT:** AI sẽ bị giảm kỹ năng (`skill`) và tăng sự tự tin mù quáng (`confidence`), dẫn đến các quyết định đổi bài phi lý trí và thường gây thiệt hại nặng nề hơn cho tài khoản.
