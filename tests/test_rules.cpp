#include "../include/Card.h"
#include "../include/Player.h"
#include "../include/AIPlayer.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

void testScoring() {
    std::cout << "[Test] Scoring Rules...";
    Card c1(Suit::SPADES, Rank::ACE); // 1
    Card c2(Suit::HEARTS, Rank::TWO); // 2
    Card c3(Suit::CLUBS, Rank::JACK); // 0
    
    // Manual scoring logic test
    int score = (c1.getScore() + c2.getScore() + c3.getScore()) % 10;
    assert(score == 3);
    std::cout << " OK\n";
}

void testBaTien() {
    std::cout << "[Test] Ba Tien Rule...";
    // AIPlayer(const std::string& name, int balance, float skill, float conf, float greed, float k, float gamma, Archetype arch)
    auto p = std::make_shared<AIPlayer>("Tester", 10000, 0.5f, 0.0f, 0.5f, 1.0f, 2.0f, Archetype::NORMAL);
    
    p->receiveCard(Card(Suit::SPADES, Rank::JACK));
    p->receiveCard(Card(Suit::HEARTS, Rank::QUEEN));
    p->receiveCard(Card(Suit::CLUBS, Rank::KING));
    
    p->updateCachedValues();
    assert(p->isBaTien() == true);
    std::cout << " OK\n";
}

int main() {
    std::cout << "--- BAI CAO CAI UNIT TESTS ---\n";
    try {
        testScoring();
        testBaTien();
        std::cout << "ALL TESTS PASSED!\n";
    } catch (...) {
        std::cout << "TESTS FAILED!\n";
        return 1;
    }
    return 0;
}
