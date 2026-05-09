#ifndef CARD_H
#define CARD_H

#include <string>

// ANSI Color Codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

enum class Suit {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
};

enum class Rank {
    ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING
};

class Card {
public:
    Suit suit;
    Rank rank;

    Card(Suit s, Rank r);
    int getModuloValue() const;
    std::string toString(bool useColor = true) const;
    std::string getSuitIcon() const;
};

#endif // CARD_H
