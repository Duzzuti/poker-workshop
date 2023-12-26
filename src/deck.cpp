#include "deck.h"

#include "logger.h"

std::string Card::toString() const {
    std::string str = "";
    switch (+this->suit) {
        case 0:
            // Diamonds
            str += "♢";
            break;
        case 1:
            // Hearts
            str += "♡";
            break;
        case 2:
            // Spades
            str += "♠";
            break;
        case 3:
            // Clubs
            str += "♣";
            break;

        default:
            PLOG_FATAL << "Invalid suit: " << +this->suit;
            throw std::logic_error("Invalid suit");
            break;
    }
    switch (+this->rank) {
        case 2 ... 9:
            str += this->rank + '0';
            break;
        case 10:
            str += "T";
            break;
        case 11:
            str += "J";
            break;
        case 12:
            str += "Q";
            break;
        case 13:
            str += "K";
            break;
        case 14:
            str += "A";
            break;
        default:
            PLOG_FATAL << "Invalid rank: " << +this->rank;
            throw std::logic_error("Invalid rank");
            break;
    }
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
    std::random_shuffle(&this->cards[0], &this->cards[52]);
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

std::string Deck::toString(const std::string sep) const {
    // print deck
    std::string str = "";
    for (u_int8_t i = 0; i < this->len; i++) {
        str += this->cards[i].toString() + sep;
    }
    return str;
}

Card Deck::getRandomCardExcept(const Card cards[], const u_int8_t cardsLen, const int8_t suit, const u_int8_t ranks[], const u_int8_t rankLen) noexcept {
    // get random card from deck except cards in vector
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
