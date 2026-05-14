#include "../include/Card.h"

Card::Card(Suit s, Rank r) : suit(s), rank(r) {}

int Card::getModuloValue() const {
    if (rank == Rank::JACK || rank == Rank::QUEEN || rank == Rank::KING) {
        return 0;
    }
    return static_cast<int>(rank) % 10;
}

std::string Card::getSuitIcon() const {
    switch (suit) {
        case Suit::HEARTS:   return "[H]";
        case Suit::DIAMONDS: return "[D]";
        case Suit::CLUBS:    return "[C]";
        case Suit::SPADES:   return "[S]";
        default:             return "[?]";
    }
}

std::string Card::toString(bool useColor) const {
    std::string r_str;
    switch (rank) {
        case Rank::ACE:   r_str = "A"; break;
        case Rank::JACK:  r_str = "J"; break;
        case Rank::QUEEN: r_str = "Q"; break;
        case Rank::KING:  r_str = "K"; break;
        default:          r_str = std::to_string(static_cast<int>(rank)); break;
    }

    if (!useColor) return r_str + getSuitIcon();

    std::string color = (suit == Suit::HEARTS || suit == Suit::DIAMONDS) ? RED : WHITE;
    return color + r_str + getSuitIcon() + RESET;
}
