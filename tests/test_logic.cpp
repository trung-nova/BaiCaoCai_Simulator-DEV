#include "Card.h"
#include "Player.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <stdexcept>

void testSigmoidLogic() {
    std::cout << "[Test] AI Sigmoid Satisfaction Logic...";
    
    // Create a player with specific k and gamma
    // float skillLevel, float confidenceLevel, float tradeDesire, float k, float gamma
    AIPlayer p("TestBot", 10000, 1.0f, 0.5f, 0.0f, 2.0f, 2.0f, Archetype::SHARK, 123);
    
    // Midpoint calculation: (5.0 - (conf * 2)) + (gamma - 2)
    // (5.0 - 1.0) + (2.0 - 2.0) = 4.0
    
    float s4 = p.getSatisfaction(4); // Should be approx 0.5
    assert(std::abs(s4 - 0.5f) < 0.01f);
    
    float s9 = p.getSatisfaction(9); // Should be very high (> 0.9)
    assert(s9 > 0.9f);
    
    float s0 = p.getSatisfaction(0); // Should be very low (< 0.1)
    assert(s0 < 0.1f);
    
    std::cout << " OK\n";
}

void testTiltTriggering() {
    std::cout << "[Test] AI Tilt State Transitions...";
    AIPlayer p("Tilter", 10000, 1.0f, 0.5f, 0.0f, 2.0f, 2.0f, Archetype::NORMAL, 123);
    
    assert(p.getIsTilt() == false);
    
    // Trigger via losses
    p.setConsecutiveLosses(5);
    p.updateTiltStatus(nullptr, 1);
    assert(p.getIsTilt() == true);
    
    // Recovery
    p.setConsecutiveLosses(0);
    p.updateTiltStatus(nullptr, 2);
    assert(p.getIsTilt() == false);
    
    // Trigger via balance drop (starts at 10000, 70% is 7000)
    p.setBalance(6000);
    p.updateTiltStatus(nullptr, 3);
    assert(p.getIsTilt() == true);
    
    std::cout << " OK\n";
}

void testBaTienHuntingLogic() {
    std::cout << "[Test] Ba Tien Hunting Decision...";
    // Skill > 0.7 required
    AIPlayer p("Hunter", 10000, 0.9f, 0.5f, 0.0f, 2.0f, 2.0f, Archetype::SHARK, 123);
    
    // Give 2 face cards and 1 number card (not Ace)
    p.receiveCard(Card(Suit::HEARTS, Rank::JACK));
    p.receiveCard(Card(Suit::DIAMONDS, Rank::QUEEN));
    p.receiveCard(Card(Suit::CLUBS, Rank::NINE)); // Score is 9, but should hunt Ba Tien
    
    Card* toTrade = p.getCardToTrade();
    assert(toTrade != nullptr);
    assert(toTrade->rank == Rank::NINE); // Should choose to trade the 9
    
    std::cout << " OK\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     BAI CAO CAI LOGIC UNIT TESTS       \n";
    std::cout << "========================================\n";
    
    try {
        testSigmoidLogic();
        testTiltTriggering();
        testBaTienHuntingLogic();
        std::cout << "\n>>> ALL LOGIC TESTS PASSED! <<<\n";
    } catch (const std::exception& e) {
        std::cerr << "\n!!! TEST FAILED: " << e.what() << " !!!\n";
        return 1;
    } catch (...) {
        std::cerr << "\n!!! UNKNOWN TEST FAILURE !!!\n";
        return 1;
    }
    
    return 0;
}
