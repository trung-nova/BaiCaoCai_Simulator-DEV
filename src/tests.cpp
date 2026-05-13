#include <iostream>
#include <cassert>
#include <vector>
#include "Card.h"
#include "Player.h"

// Simple Mock for Player to test score logic
class TestPlayer : public AIPlayer {
public:
    TestPlayer() : AIPlayer("Test", 10000, 1.0f, 1.0f, 0.0f) {}
};

void testCardModulo() {
    Card c1(Suit::HEARTS, Rank::ACE);
    assert(c1.getModuloValue() == 1);

    Card c2(Suit::DIAMONDS, Rank::TEN);
    assert(c2.getModuloValue() == 0);

    Card c3(Suit::CLUBS, Rank::JACK);
    assert(c3.getModuloValue() == 0);
    
    std::cout << "[PASS] testCardModulo\n";
}

void testHandScore() {
    TestPlayer p;
    
    // Normal case: 1 + 2 + 3 = 6
    p.clearHand();
    p.receiveCard(Card(Suit::HEARTS, Rank::ACE));
    p.receiveCard(Card(Suit::DIAMONDS, Rank::TWO));
    p.receiveCard(Card(Suit::CLUBS, Rank::THREE));
    assert(p.getScore() == 6);

    // Modulo case: 7 + 8 + 9 = 24 -> 4
    p.clearHand();
    p.receiveCard(Card(Suit::HEARTS, Rank::SEVEN));
    p.receiveCard(Card(Suit::DIAMONDS, Rank::EIGHT));
    p.receiveCard(Card(Suit::CLUBS, Rank::NINE));
    assert(p.getScore() == 4);

    // Ba Tien case: J, Q, K
    p.clearHand();
    p.receiveCard(Card(Suit::HEARTS, Rank::JACK));
    p.receiveCard(Card(Suit::DIAMONDS, Rank::QUEEN));
    p.receiveCard(Card(Suit::CLUBS, Rank::KING));
    assert(p.isBaTien() == true);
    assert(p.getScore() == 10);
    
    std::cout << "[PASS] testHandScore\n";
}

void testSatisfaction() {
    TestPlayer p;
    // Score 9 should have high satisfaction
    float s9 = p.getSatisfaction(9);
    float s0 = p.getSatisfaction(0);
    assert(s9 > s0);
    assert(s9 > 0.8f);
    
    std::cout << "[PASS] testSatisfaction\n";
}

int main() {
    std::cout << "--- RUNNING UNIT TESTS ---\n";
    testCardModulo();
    testHandScore();
    testSatisfaction();
    std::cout << "--- ALL TESTS PASSED ---\n";
    return 0;
}
