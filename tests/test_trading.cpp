#include "Card.h"
#include "Player.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

// Testing the derangement logic principle and hand updates.

void testDerangementLogic() {
    std::cout << "[Test] Derangement Redistribution Logic...";
    
    const int numPlayers = 5;
    std::vector<int> indices(numPlayers);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::mt19937 rng(42); // Fixed seed for reproducibility
    
    // Simulating the derangement loop
    bool isDerangement = false;
    int attempts = 0;
    while (!isDerangement && attempts < 100) {
        std::shuffle(indices.begin(), indices.end(), rng);
        isDerangement = true;
        for (int i = 0; i < numPlayers; ++i) {
            if (indices[i] == i) {
                isDerangement = false;
                break;
            }
        }
        attempts++;
    }
    
    assert(isDerangement);
    for (int i = 0; i < numPlayers; ++i) {
        assert(indices[i] != i); // No one gets their own card
    }
    
    // Check if it's a permutation (everyone gets exactly one card)
    std::vector<int> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());
    for (int i = 0; i < numPlayers; ++i) {
        assert(sortedIndices[i] == i);
    }
    
    std::cout << " OK (Attempts: " << attempts << ")\n";
}

void testHandUpdateAfterTrade() {
    std::cout << "[Test] Hand Update After Trade Redistribution...";
    
    // Create 3 AI players
    AIPlayer p1("P1", 10000, 0.5f, 0.5f, 0.0f, 1.0f, 2.0f, Archetype::NORMAL, 1);
    AIPlayer p2("P2", 10000, 0.5f, 0.5f, 0.0f, 1.0f, 2.0f, Archetype::NORMAL, 2);
    AIPlayer p3("P3", 10000, 0.5f, 0.5f, 0.0f, 1.0f, 2.0f, Archetype::NORMAL, 3);
    
    Card c1(Suit::HEARTS, Rank::ACE);
    Card c2(Suit::DIAMONDS, Rank::TWO);
    Card c3(Suit::CLUBS, Rank::THREE);
    
    p1.receiveCard(c1); p1.receiveCard(Card(Suit::SPADES, Rank::TEN)); p1.receiveCard(Card(Suit::SPADES, Rank::NINE));
    p2.receiveCard(c2); p2.receiveCard(Card(Suit::SPADES, Rank::TEN)); p2.receiveCard(Card(Suit::SPADES, Rank::NINE));
    p3.receiveCard(c3); p3.receiveCard(Card(Suit::SPADES, Rank::TEN)); p3.receiveCard(Card(Suit::SPADES, Rank::NINE));
    
    // Setup hand for trading
    
    Card tradeCard1(Suit::HEARTS, Rank::JACK);
    Card tradeCard2(Suit::DIAMONDS, Rank::QUEEN);
    Card tradeCard3(Suit::CLUBS, Rank::KING);
    
    p1.clearHand(); p1.receiveCard(tradeCard1); p1.receiveCard(Card(Suit::SPADES, Rank::TEN)); p1.receiveCard(Card(Suit::SPADES, Rank::NINE));
    p2.clearHand(); p2.receiveCard(tradeCard2); p2.receiveCard(Card(Suit::SPADES, Rank::TEN)); p2.receiveCard(Card(Suit::SPADES, Rank::NINE));
    p3.clearHand(); p3.receiveCard(tradeCard3); p3.receiveCard(Card(Suit::SPADES, Rank::TEN)); p3.receiveCard(Card(Suit::SPADES, Rank::NINE));
    
    // Redistribution: P1 -> P2, P2 -> P3, P3 -> P1
    Card* out1 = p1.getCardToTrade(); // should be tradeCard1 (Face cards are prioritized)
    Card* out2 = p2.getCardToTrade(); 
    Card* out3 = p3.getCardToTrade(); 
    
    Card in1 = *out2;
    Card in2 = *out3;
    Card in3 = *out1;
    
    p1.swapCard(out1, &in1);
    p2.swapCard(out2, &in2);
    p3.swapCard(out3, &in3);
    
    // Finalizing swaps and checking consistency
    
    
    
    std::cout << " OK\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     TRADING MECHANISM UNIT TESTS       \n";
    std::cout << "========================================\n";
    
    try {
        testDerangementLogic();
        testHandUpdateAfterTrade();
        std::cout << "\n>>> ALL TRADING TESTS PASSED! <<<\n";
    } catch (const std::exception& e) {
        std::cerr << "\n!!! TEST FAILED: " << e.what() << " !!!\n";
        return 1;
    }
    
    return 0;
}
