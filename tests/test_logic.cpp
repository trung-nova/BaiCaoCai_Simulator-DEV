#include "../include/Card.h"
#include "../include/Player.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <iomanip>

void testCardModulo() {
    std::cout << "[Test] Card Modulo & Scoring Rules...\n";
    
    struct TestCase { Rank r; int expected; std::string name; };
    std::vector<TestCase> cases = {
        {Rank::ACE, 1, "Ace"},
        {Rank::TWO, 2, "Two"},
        {Rank::NINE, 9, "Nine"},
        {Rank::TEN, 0, "Ten"},
        {Rank::JACK, 0, "Jack"},
        {Rank::QUEEN, 0, "Queen"},
        {Rank::KING, 0, "King"}
    };

    for (const auto& tc : cases) {
        Card c(Suit::HEARTS, tc.r);
        int val = c.getModuloValue();
        std::cout << "  - " << std::left << std::setw(10) << tc.name << ": Got " << val << " (Expected " << tc.expected << ")";
        if (val == tc.expected) std::cout << " [PASSED]\n";
        else { std::cout << " [FAILED]\n"; assert(false); }
    }
}

void testHandScoring() {
    std::cout << "[Test] Hand Scoring Logic (Modulo 10)...\n";
    
    AIPlayer p("Scorer", 10000, 0.5f, 0.5f, 0.0f, 1.0f, 2.0f, Archetype::NORMAL, 123);
    
    // Test 1: 1 + 2 + 3 = 6
    p.clearHand();
    p.receiveCard(Card(Suit::HEARTS, Rank::ACE));
    p.receiveCard(Card(Suit::HEARTS, Rank::TWO));
    p.receiveCard(Card(Suit::HEARTS, Rank::THREE));
    p.updateCachedValues();
    std::cout << "  - Hand: A, 2, 3 | Score: " << p.getScore() << " (Expected 6)\n";
    assert(p.getScore() == 6);
    assert(p.isBaTien() == false);

    // Test 2: 9 + 5 + 7 = 21 -> 1
    p.clearHand();
    p.receiveCard(Card(Suit::HEARTS, Rank::NINE));
    p.receiveCard(Card(Suit::HEARTS, Rank::FIVE));
    p.receiveCard(Card(Suit::HEARTS, Rank::SEVEN));
    p.updateCachedValues();
    std::cout << "  - Hand: 9, 5, 7 | Score: " << p.getScore() << " (Expected 1)\n";
    assert(p.getScore() == 1);

    // Test 3: Jack, Queen, King -> Ba Tien (10 pts)
    p.clearHand();
    p.receiveCard(Card(Suit::HEARTS, Rank::JACK));
    p.receiveCard(Card(Suit::DIAMONDS, Rank::QUEEN));
    p.receiveCard(Card(Suit::CLUBS, Rank::KING));
    p.updateCachedValues();
    std::cout << "  - Hand: J, Q, K | Score: " << p.getScore() << " (Expected 10, Ba Tien)\n";
    assert(p.getScore() == 10);
    assert(p.isBaTien() == true);
}

void testSigmoidLogic() {
    std::cout << "[Test] AI Sigmoid Satisfaction Logic...\n";
    
    AIPlayer p("TestBot", 10000, 1.0f, 0.5f, 0.0f, 2.0f, 2.0f, Archetype::SHARK, 123);
    
    float s4 = p.getSatisfaction(4); 
    float s9 = p.getSatisfaction(9); 
    float s0 = p.getSatisfaction(0); 

    std::cout << "  - Score 0: " << std::fixed << std::setprecision(4) << s0 << " (Very Low)\n";
    std::cout << "  - Score 4: " << std::fixed << std::setprecision(4) << s4 << " (Approx 0.5)\n";
    std::cout << "  - Score 9: " << std::fixed << std::setprecision(4) << s9 << " (Very High)\n";

    assert(s0 < 0.1f);
    assert(std::abs(s4 - 0.5f) < 0.05f);
    assert(s9 > 0.9f);
}

void testTiltTriggering() {
    std::cout << "[Test] AI Tilt State Transitions...\n";
    AIPlayer p("Tilter", 10000, 1.0f, 0.5f, 0.0f, 2.0f, 2.0f, Archetype::NORMAL, 123);
    
    assert(p.getIsTilt() == false);
    
    p.setConsecutiveLosses(5);
    p.updateTiltStatus(nullptr, 1);
    std::cout << "  - Triggered by 5 losses: " << (p.getIsTilt() ? "YES" : "NO") << "\n";
    assert(p.getIsTilt() == true);
    
    p.setConsecutiveLosses(0);
    p.updateTiltStatus(nullptr, 2);
    std::cout << "  - Recovered after loss reset: " << (p.getIsTilt() ? "NO" : "YES") << "\n";
    assert(p.getIsTilt() == false);
    
    p.setBalance(6000);
    p.updateTiltStatus(nullptr, 3);
    std::cout << "  - Triggered by balance drop (<70%): " << (p.getIsTilt() ? "YES" : "NO") << "\n";
    assert(p.getIsTilt() == true);
}

void testBaTienHuntingLogic() {
    std::cout << "[Test] Ba Tien Hunting Decision...\n";
    AIPlayer p("Hunter", 10000, 0.9f, 0.5f, 0.0f, 2.0f, 2.0f, Archetype::SHARK, 123);
    
    p.receiveCard(Card(Suit::HEARTS, Rank::JACK));
    p.receiveCard(Card(Suit::DIAMONDS, Rank::QUEEN));
    p.receiveCard(Card(Suit::CLUBS, Rank::NINE)); 
    
    Card* toTrade = p.getCardToTrade();
    std::cout << "  - Hand: J, Q, 9 | Decision: Trade " << (toTrade ? toTrade->toString() : "None") << "\n";
    assert(toTrade != nullptr);
    assert(toTrade->rank == Rank::NINE); 
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     BAI CAO CAI LOGIC UNIT TESTS       \n";
    std::cout << "========================================\n";
    
    try {
        testCardModulo();
        testHandScoring();
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

