#include "deck.h"

#include "logger.h"

const char* Card::toString() const {
    static char str[CARD_STR_LEN];  // Increased size to accommodate null terminator and Unicode symbol

    if (this->suit > 3) {
        PLOG_FATAL << "Invalid suit: " << this->suit;
        throw std::logic_error("Invalid suit");
    }

    std::strcpy(str, SUIT_SYMBOLS[this->suit]);

    if (this->rank < 2 || this->rank > 14) {
        PLOG_FATAL << "Invalid rank: " << this->rank;
        throw std::logic_error("Invalid rank");
    }

    str[3] = CARD_RANKS[this->rank - 2];

    str[4] = '\0';  // Null-terminate the string

    return str;
}

Deck::Deck() noexcept {
    // generate poker deck
    u_int8_t i = 0;
    for (u_int8_t suit = 0; suit < 4; suit++) {
        for (u_int8_t rank = 2; rank < 15; rank++) {
            this->cards[i].rank = rank;
            this->cards[i].suit = suit;
            i++;
        }
    }
}

void Deck::shuffle() noexcept {
    // shuffle deck
    std::random_shuffle(&this->cards[0], &this->cards[CARD_NUM]);
}

Card Deck::draw() {
    // draw card from deck
    if (this->len == 0) {
        PLOG_FATAL << "Deck is empty";
        throw std::logic_error("Deck is empty");
    }
    this->len--;
    return this->cards[this->len];
}

const char* Deck::toString(const char sep) const {
    // print deck
    static char str[CARD_NUM * (CARD_STR_LEN + 1)];
    for (u_int8_t i = 0; i < this->len; i++) {
        std::strncat(str, this->cards[i].toString(), CARD_STR_LEN);
        std::strncat(str, &sep, 1);
    }
    return str;
}

Card Deck::getRandomCardExcept(const Card cards[], const u_int8_t cardsLen, const int8_t suit, const u_int8_t ranks[], const u_int8_t rankLen) noexcept {
    // get random card from deck except cards in array
    // or with suit if suit != -1
    // or with ranks if ranks.size() > 0
    while (true) {
        Card card{.rank = (u_int8_t)((std::rand() % 13) + 2), .suit = (u_int8_t)(std::rand() % 4)};
        if (std::find(cards, &cards[cardsLen], card) == &cards[cardsLen] && (suit == -1 || card.suit != suit) && (!rankLen || std::find(ranks, &ranks[rankLen], card.rank) == &ranks[rankLen])) {
            return card;
        }
    }
}

Card Deck::getRandomCardExceptCardsWith(const Card exceptionCards[], const u_int8_t cardsLen, const int8_t suit, const int8_t rank) noexcept {
    // get random card from deck with suit if suit != -1 and rank if rank != -1

    Card card;
    do {
        if (suit == -1 && rank == -1)  // get random card
            card = Card{.rank = (u_int8_t)((std::rand() % 13) + 2), .suit = (u_int8_t)(std::rand() % 4)};
        else if (suit == -1)  // get random card with rank
            card = Card{.rank = (u_int8_t)rank, .suit = (u_int8_t)(std::rand() % 4)};
        else if (rank == -1)  // get random card with suit
            card = Card{.rank = (u_int8_t)((std::rand() % 13) + 2), .suit = (u_int8_t)suit};
        else  // get card with suit and rank
            card = Card{.rank = (u_int8_t)rank, .suit = (u_int8_t)suit};
    } while (std::find(exceptionCards, &exceptionCards[cardsLen], card) != &exceptionCards[cardsLen]);
    return card;
}
