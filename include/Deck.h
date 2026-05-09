#ifndef DECK_H
#define DECK_H

#include <vector>
#include <random>
#include "Card.h"

class Deck {
private:
    std::vector<Card> cards;

public:
    int drawIndex = 0;
    Deck();
    void shuffle();
    void reset();
    Card drawCard();
    bool isEmpty() const;
};

#endif // DECK_H
