#include "Deck.h"
#include <algorithm>
#include <stdexcept>
#include <chrono>

static unsigned deck_seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 global_rng(deck_seed);

Deck::Deck() {
    for (int s = 0; s < 4; ++s) {
        for (int r = 1; r <= 13; ++r) {
            cards.push_back(Card(static_cast<Suit>(s), static_cast<Rank>(r)));
        }
    }
}

void Deck::shuffle() {
    std::shuffle(cards.begin(), cards.end(), global_rng);
}

void Deck::reset() {
    drawIndex = 0;
    shuffle();
}

Card Deck::drawCard() {
    if (drawIndex >= (int)cards.size()) {
        throw std::out_of_range("Deck is empty");
    }
    return cards[drawIndex++];
}

bool Deck::isEmpty() const {
    return drawIndex >= (int)cards.size();
}
