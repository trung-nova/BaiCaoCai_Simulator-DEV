# KIẾN TRÚC HƯỚNG ĐỐI TƯỢNG (OOP ARCHITECTURE)

Tài liệu này giải thích cách hệ thống được xây dựng dựa trên các nguyên lý OOP để đảm bảo tính **Mở rộng (Extensibility)** và **Linh hoạt (Flexibility)**.

---

## 1. Sơ đồ lớp (Class Diagram) - Phiên bản chuẩn hóa

```mermaid
classDiagram
    %% --- Giao diện Trạng thái (State Pattern) ---
    class GameState {
        <<interface>>
        +handle(context: GameManager)* void
    }

    class BettingState { +handle(context: GameManager) void }
    class DealingState { +handle(context: GameManager) void }
    class TradingState { +handle(context: GameManager) void }
    class EvalState { +handle(context: GameManager) void }

    GameState <|-- BettingState
    GameState <|-- DealingState
    GameState <|-- TradingState
    GameState <|-- EvalState

    %% --- Lớp trừu tượng Người chơi (Polymorphism) ---
    class Player {
        <<abstract>>
        #name: string
        #archetype: string
        #hand: vector~Card~
        #balance: int
        #skillLevel: float
        #confidenceLevel: float
        #isTilt: bool
        +wantsToTrade()* TradeDecision
        +getScore() int
        +isBaTien() bool
    }

    class AIPlayer {
        -rng: mt19937
        +wantsToTrade() TradeDecision
    }

    class HumanPlayer {
        +wantsToTrade() TradeDecision
    }

    Player <|-- AIPlayer
    Player <|-- HumanPlayer

    %% --- Lớp Điều khiển Trung tâm ---
    class GameManager {
        +players: vector~shared_ptr~Player~~
        +currentState: unique_ptr~GameState~
        +archetypeConfigs: map~string, ArchetypeConfig~
        +roundCount: int
        +simulationSeed: long long
        +changeState(newState: GameState) void
        +playRound() void
        +startStreaming() void
        +logAIConfigs() void
    }

    %% --- Lớp Dữ liệu & Tiện ích ---
    class Card {
        +suit: Suit
        +rank: Rank
    }

    class Deck {
        -cards: vector~Card~
        +shuffle() void
        +drawCard() Card
    }

    %% --- Mối quan hệ ---
    GameManager *-- Player : Quản lý (1..N)
    GameManager *-- GameState : Trạng thái hiện tại
    GameManager o-- Deck : Sử dụng
    Player *-- Card : Nắm giữ (3)
    Deck *-- Card : Chứa đựng (52)
```

---

## 2. Các điểm cải tiến về Thiết kế (Design Highlights)

### 2.1. Dynamic Archetype System (Cơ chế Archetype Động)
Thay vì sử dụng `enum` cứng nhắc, hệ thống sử dụng **`std::string`** làm định danh cho các loại cá tính AI. 
- **Lợi ích**: Người dùng có thể thêm bất kỳ loại AI mới nào vào file `config.ini` (ví dụ: `[WHALE]`, `[GAMBLER]`) mà không cần sửa đổi mã nguồn.
- **Kỹ thuật**: Sử dụng `std::map<std::string, ArchetypeConfig>` để lưu trữ tham số cấu hình.

### 2.2. State Pattern (Mẫu trạng thái)
Vòng đời của một ván bài được tách bạch thành các lớp `State` riêng biệt. Điều này giúp loại bỏ các cấu trúc `if-else` phức tạp trong `GameManager` và cho phép mở rộng luật chơi (ví dụ: thêm vòng cược) cực kỳ dễ dàng.

### 2.3. Polymorphism & Smart Pointers
- Sử dụng `std::shared_ptr<Player>` để quản lý đa hình giữa Người và Máy.
- `std::unique_ptr<GameState>` đảm bảo quản lý bộ nhớ an toàn, tự động giải phóng trạng thái cũ khi chuyển sang trạng thái mới.

---

## 3. Quản lý Dữ liệu (Data Streaming)
Dữ liệu được đẩy trực tiếp ra các tệp CSV thông qua các luồng (`std::ofstream`) được quản lý bởi `GameManager`. Việc gỡ bỏ SQLite giúp hệ thống nhẹ hơn và loại bỏ các phụ thuộc thư viện bên ngoài (External Dependencies), tăng tính di động (Portability) của phần mềm.
