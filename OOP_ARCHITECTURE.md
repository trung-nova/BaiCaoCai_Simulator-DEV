# KIẾN TRÚC HƯỚNG ĐỐI TƯỢNG (OOP ARCHITECTURE)

Tài liệu này giải thích cách hệ thống được xây dựng dựa trên các nguyên lý lập trình hướng đối tượng và các mẫu thiết kế (Design Patterns) để đảm bảo tính mở rộng và dễ bảo trì.

---

## 1. Sơ đồ lớp (Class Diagram)

Hệ thống được thiết kế theo mô hình phân lớp rõ ràng, tách biệt giữa dữ liệu (`Card`, `Deck`), thực thể (`Player`) và điều khiển (`GameManager`, `GameState`).

```mermaid
classDiagram
    class GameManager {
        +players: vector~shared_ptr~Player~~
        +currentState: unique_ptr~GameState~
        +deck: Deck
        +changeState(newState: GameState)
        +playRound()
    }

    class Player {
        <<abstract>>
        #name: string
        #hand: vector~Card~
        #balance: int
        #skillLevel: float
        +wantsToTrade()* bool
        +getScore() int
    }

    class AIPlayer {
        +wantsToTrade() bool
    }

    class HumanPlayer {
        +wantsToTrade() bool
    }

    class GameState {
        <<interface>>
        +handle(context: GameManager)*
    }

    class BettingState { +handle() }
    class DealingState { +handle() }
    class TradingState { +handle() }
    class EvalState { +handle() }

    GameManager *-- Player : manages
    GameManager *-- GameState : has current
    Player <|-- AIPlayer : inherits
    Player <|-- HumanPlayer : inherits
    GameState <|-- BettingState
    GameState <|-- DealingState
    GameState <|-- TradingState
    GameState <|-- EvalState
    Player *-- Card : holds
```

---

## 2. Các mẫu thiết kế (Design Patterns)

### 2.1. State Pattern (Mẫu trạng thái)
Đây là "xương sống" của hệ thống. Thay vì sử dụng các câu lệnh `if-else` hoặc `switch-case` khổng lồ để kiểm tra xem game đang ở giai đoạn nào, chúng ta tách mỗi giai đoạn thành một lớp riêng biệt kế thừa từ `GameState`.

- **Ưu điểm**: 
    - Tuân thủ nguyên lý **Open/Closed**: Dễ dàng thêm luật chơi mới (ví dụ: vòng cược bổ sung) bằng cách thêm một State mới mà không sửa code cũ.
    - Logic của từng pha được đóng gói gọn gàng, dễ debug.

### 2.2. Strategy Pattern & Polymorphism (Chiến lược & Đa hình)
Hành vi của người chơi (đặc biệt là logic đổi bài) được thực hiện thông qua cơ chế **Virtual Methods**. 

- `Player` là một lớp trừu tượng (Abstract Class).
- `AIPlayer` triển khai logic dựa trên hàm Sigmoid.
- `HumanPlayer` triển khai logic dựa trên nhập liệu từ bàn phím.
- **Tính đa hình**: `GameManager` gọi `player->wantsToTrade()` mà không cần biết đó là người hay máy.

---

## 3. Quản lý bộ nhớ & Tính an toàn (Memory Management)

Hệ thống sử dụng **Smart Pointers** (C++11/17) để loại bỏ hoàn toàn lỗi rò rỉ bộ nhớ (Memory Leaks):

*   `std::shared_ptr<Player>`: Cho phép nhiều thành phần (Manager, States) cùng tham chiếu đến một người chơi mà vẫn tự động giải phóng khi không còn ai dùng.
*   `std::unique_ptr<GameState>`: Đảm bảo tại một thời điểm chỉ có một trạng thái duy nhất tồn tại và được quản lý vòng đời bởi `GameManager`.

---

## 4. Encapsulation (Tính đóng gói)

- Các thuộc tính nhạy cảm như `balance`, `skillLevel` được để ở mức `protected` hoặc `private`.
- Chỉ các lớp "bạn bè" (`friend class`) như các `State` mới có quyền can thiệp sâu vào dữ liệu người chơi, đảm bảo tính toàn vẹn của dữ liệu trong suốt quá trình mô phỏng.
