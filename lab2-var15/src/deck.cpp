#include "deck.h"

std::vector<Card> CreateDeck() {
    std::vector<Card> deck;
    deck.reserve(52);
    for (int denom = 0; denom < 13; ++denom) {
        for (int suit = 0; suit < 4; ++suit) {
            deck.push_back(Card{denom});
        }
    }

    return deck;
}