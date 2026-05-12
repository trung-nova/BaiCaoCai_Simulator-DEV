# LUẬT CHƠI BÀI CÀO CÁI (SIMULATION RULES)

Tài liệu này giải thích quy tắc trò chơi được lập trình trong hệ thống mô phỏng, phục vụ cho việc hiểu dữ liệu đầu ra.

## 1. Cấu trúc Bộ bài (The Deck)
Hệ thống sử dụng bộ bài Tây tiêu chuẩn 52 lá.
- **Chất (Suit)**: Cơ (Hearts), Rô (Diamonds), Chuồn (Clubs), Bích (Spades).
- **Số (Rank)**: Từ Ace (1) đến King (13).

## 2. Cách tính Điểm (Scoring Logic)
Điểm số của mỗi người chơi được tính dựa trên tổng giá trị của 3 lá bài trên tay.

### 2.1. Giá trị từng lá bài
- **Lá bài số (Ace - 9)**: Giá trị tương ứng từ 1 đến 9.
- **Lá bài hình (10, Jack, Queen, King)**: Giá trị là **0**.

### 2.2. Công thức tính tổng
Điểm số cuối cùng là chữ số hàng đơn vị của tổng giá trị 3 lá bài (phép chia lấy dư cho 10).
> **Score = (Card1 + Card2 + Card3) % 10**

*Ví dụ:* 
- 7 + 8 + 2 = 17 => **7 điểm**.
- Ace + 9 + King = 1 + 9 + 0 = 10 => **0 điểm** (Bù).

### 2.3. Bộ bài đặc biệt: "Ba Tiên"
Nếu 3 lá bài trên tay đều thuộc nhóm các lá bài hình (**Jack, Queen, hoặc King**), bộ bài đó được gọi là **Ba Tiên**.
- **Giá trị**: Trong hệ thống mô phỏng, Ba Tiên được gán giá trị **10** (để dễ dàng so sánh trong code).
- **Quy tắc tổ hợp**: Bất kỳ sự kết hợp nào giữa J, Q, K đều được tính. Ví dụ: {J, Q, K}, {J, J, Q}, hoặc cả 3 lá giống nhau như {J, J, J}, {Q, Q, Q}, {K, K, K} đều là Ba Tiên.
- Đây là bộ bài mạnh nhất, thắng mọi bộ bài điểm (từ 0 đến 9 điểm).
- Nếu cả Nhà Cái và Người chơi đều có Ba Tiên, Nhà Cái vẫn thắng (theo quy tắc House Edge).

---

## 3. Luật Thắng - Thua (Win/Loss Conditions)
Hệ thống mô phỏng theo cơ chế **Nhà Cái (Dealer)** đối đầu với các **Nhà Con (Players)**.

- **Người chơi Thắng**: Nếu điểm của người chơi **cao hơn** điểm của Nhà Cái.
- **Nhà Cái Thắng**: Nếu điểm của Nhà Cái **cao hơn hoặc bằng** điểm của người chơi.
  > **Lưu ý quan trọng**: Trường hợp **Hòa điểm**, Nhà Cái luôn là người chiến thắng. Đây là "lợi thế nhà cái" (House Edge) cốt lõi.

---

## 4. Đặt cược và Thanh toán (Betting & Payout)
- **Tiền xâu (Ante)**: Mỗi ván đấu, các Nhà Con phải đặt 1 đơn vị tiền cược (mặc định là 1 chip).
- **Thanh toán**: 
  - Nếu người chơi thắng: Nhận lại 1 chip đặt cược và nhận thêm 1 chip từ Nhà Cái (Lãi 1).
  - Nếu người chơi thua: Mất 1 chip đã đặt cược vào tay Nhà Cái.
- **Rủi ro phá sản**: Nếu Nhà Cái không còn đủ tiền để trả cho tất cả những người thắng, Nhà Cái sẽ trả hết số tiền còn lại của mình theo thứ tự ưu tiên (ai thắng trước nhận trước), người thắng sau có thể không nhận được tiền.
