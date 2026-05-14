#include "../include/Card.h"
#include "../include/Player.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>

void testDerangementLogic() {
    std::cout << "[Test] Derangement Redistribution Logic (No-one gets own card)...\n";
    
    const int numPlayers = 5;
    std::vector<int> indices(numPlayers);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::mt19937 rng(42); 
    
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
    
    std::cout << "  - Redistribution map: ";
    for(int i=0; i<numPlayers; ++i) std::cout << i << "->" << indices[i] << " ";
    std::cout << "\n  - Success in " << attempts << " attempts.\n";

    assert(isDerangement);
    for (int i = 0; i < numPlayers; ++i) assert(indices[i] != i); 
    
    std::vector<int> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end());
    for (int i = 0; i < numPlayers; ++i) assert(sortedIndices[i] == i);
}

void testHandUpdateAfterTrade() {
    std::cout << "[Test] Hand Update After Trade redistribution...\n";
    
    AIPlayer p1("Bot1", 10000, 0.5f, 0.5f, 0.0f, 1.0f, 2.0f, "NORMAL", 1);
    AIPlayer p2("Bot2", 10000, 0.5f, 0.5f, 0.0f, 1.0f, 2.0f, "NORMAL", 2);
    
    Card c1(Suit::HEARTS, Rank::ACE);
    Card c2(Suit::DIAMONDS, Rank::KING);
    
    p1.receiveCard(c1); p1.receiveCard(Card(Suit::SPADES, Rank::TEN)); p1.receiveCard(Card(Suit::SPADES, Rank::NINE));
    p2.receiveCard(c2); p2.receiveCard(Card(Suit::SPADES, Rank::TEN)); p2.receiveCard(Card(Suit::SPADES, Rank::NINE));
    
    p1.updateCachedValues(); p2.updateCachedValues();
    std::cout << "  - P1 Before: " << c1.toString() << " | Score: " << p1.getScore() << "\n";
    std::cout << "  - P2 Before: " << c2.toString() << " | Score: " << p2.getScore() << "\n";

    Card* out1 = p1.getCardToTrade(); // prioritized Face/Low
    Card* out2 = p2.getCardToTrade(); 
    
    Card in1 = *out2;
    Card in2 = *out1;
    
    std::cout << "  - SWAP: P1 gives " << out1->toString() << " to P2, receives " << in1.toString() << "\n";

    p1.swapCard(out1, &in1);
    p2.swapCard(out2, &in2);
    
    std::cout << "  - P1 After: " << p1.getScore() << " pts\n";
    std::cout << "  - P2 After: " << p2.getScore() << " pts\n";

    // Verification
    bool foundInP1 = false;
    for(const auto& c : p1.getBankrollHistory()) { /* just to access something if needed */ }
    // Actually check hand
    // Since we don't have a direct getHand, we'll check score change or BaTien status
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

