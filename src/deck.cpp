#include "deck.h"

const char* Card::toString() const {
    // Size should accommodate null terminator and Unicode symbol
    static char str[CARD_STR_LEN];

    // Invalid suit
    if (this->suit > 3) {
        PLOG_FATAL << "Invalid suit: " << +this->suit;
        throw std::logic_error("Invalid suit");
    }
    // Add the suit symbol to the string (unicode)
    std::strncpy(str, SUIT_SYMBOLS[this->suit], 3);

    // Invalid rank
    if (this->rank < 2 || this->rank > 14) {
        PLOG_FATAL << "Invalid rank: " << +this->rank;
        throw std::logic_error("Invalid rank");
    }

    // Add the rank to the string
    str[3] = CARD_RANKS[this->rank - 2];

    // Null-terminate the string
    str[4] = '\0';

    return str;
}

Deck::Deck() noexcept {
    // generate a 53 cards poker deck
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
    // if deck is empty throw exception
    if (this->len == 0) {
        PLOG_FATAL << "Deck is empty";
        throw std::logic_error("Deck is empty");
    }
    this->len--;
    return this->cards[this->len];
}

const char* Deck::toString(const char sep) const {
    // print deck by concatenating all cards in deck with separator
    static char str[CARD_NUM * (CARD_STR_LEN + 1)];
    for (u_int8_t i = 0; i < this->len; i++) {
        std::strncat(str, this->cards[i].toString(), CARD_STR_LEN);
        std::strncat(str, &sep, 1);
    }
    return str;
}

Card Deck::getRandomCard() noexcept { return Card{.rank = (u_int8_t)((std::rand() % 13) + 2), .suit = (u_int8_t)(std::rand() % 4)}; }

Card Deck::getRandomCardExcept(const Card cards[], const u_int8_t cardsLen, const int8_t suit, const u_int8_t ranks[], const u_int8_t rankLen) noexcept {
    // get random card from deck except cards in array
    // except with suit if suit != -1
    // except with ranks if ranks.size() > 0
    while (true) {
        // gets random card
        Card card{.rank = (u_int8_t)((std::rand() % 13) + 2), .suit = (u_int8_t)(std::rand() % 4)};
        // check conditions
        if ((suit == -1 || card.suit != suit) && (!rankLen || std::find(ranks, &ranks[rankLen], card.rank) == &ranks[rankLen]) && std::find(cards, &cards[cardsLen], card) == &cards[cardsLen]) {
            return card;
        }
    }
}

Card Deck::getRandomCardExceptCardsWith(const Card exceptionCards[], const u_int8_t cardsLen, const int8_t suit, const int8_t rank) noexcept {
    // get random card from deck with suit if suit != -1 and rank if rank != -1
    // except cards in array
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
        // check if card is in exceptionCards
    } while (std::find(exceptionCards, &exceptionCards[cardsLen], card) != &exceptionCards[cardsLen]);
    return card;
}

void Deck::putCard(const Card card, const u_int8_t cardPos) {
    // find card in deck and swap it with the card at cardPos
    int8_t pos = this->len - cardPos - 1;
    if (pos < 0) {
        PLOG_FATAL << "Invalid card position: there are only " << this->len << " cards in the deck; trying to put a card at position " << cardPos;
        throw std::invalid_argument("Invalid card position: there are only " + std::to_string(this->len) + " cards in the deck; trying to put a card at position " + std::to_string(cardPos));
    }
    for (u_int8_t i = 0; i < this->len; i++) {
        if (this->cards[i] == card) {
            std::swap(this->cards[i], this->cards[pos]);
            return;
        }
    }
    PLOG_FATAL << "Card not found in remaining deck (deck might be not complete or card is invalid): " << card.toString();
    throw std::invalid_argument("Card not found in remaining deck");
}
