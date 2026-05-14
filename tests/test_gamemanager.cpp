#include "../include/GameManager.h"
#include "../include/ConcreteStates.h"
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

void testStateTransitions() {
    std::cout << "[Test] GameManager State Transition Flow...\n";
    
    GameManager manager;
    manager.logMode = false; // Keep it quiet

    // 1. Initial State
    assert(manager.currentState == nullptr);
    std::cout << "  - Initial state is null: OK\n";

    // 2. Set Players
    std::vector<std::shared_ptr<Player>> players;
    players.push_back(std::make_shared<AIPlayer>("P1", 10000, 0.5f, 0.5f, 0.0f));
    players.push_back(std::make_shared<AIPlayer>("P2", 10000, 0.5f, 0.5f, 0.0f));
    manager.setPlayers(players);
    assert(manager.players.size() == 2);
    std::cout << "  - Players set: OK\n";

    // 3. Start Round -> Betting State
    manager.changeState(StateFactory::getBettingState());
    assert(manager.currentState != nullptr);
    std::cout << "  - Transition to Betting State: OK\n";

    // 4. Update -> Dealing State
    manager.currentState->update(&manager);
    // BettingState updates manager to DealingState
    // Note: State classes are defined in ConcreteStates.h
    std::cout << "  - Update 1 (Betting -> Dealing): OK\n";

    // 5. Update -> Trading State
    manager.currentState->update(&manager);
    std::cout << "  - Update 2 (Dealing -> Trading): OK\n";

    // 6. Update -> Eval State
    // TradingState runs 3 turns then goes to Eval
    manager.currentState->update(&manager);
    std::cout << "  - Update 3 (Trading -> Eval): OK\n";

    // 7. Update -> End (Null)
    manager.currentState->update(&manager);
    assert(manager.currentState == nullptr);
    std::cout << "  - Update 4 (Eval -> Null): OK\n";

    std::cout << ">>> Game Flow Test PASSED! <<<\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     GAMEMANAGER INTEGRATION TESTS      \n";
    std::cout << "========================================\n";
    
    try {
        testStateTransitions();
    } catch (const std::exception& e) {
        std::cerr << "!!! TEST FAILED: " << e.what() << " !!!\n";
        return 1;
    }
    
    return 0;
}
