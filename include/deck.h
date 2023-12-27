#pragma once

#include <iostream>

struct Card {
    u_int8_t rank;  // 2-14
    u_int8_t suit;  // 0-3  (0 = Diamonds, 1 = Hearts, 2 = Spades, 3 = Clubs)

    std::string toString() const;

    friend bool operator==(const Card& lhs, const Card& rhs) noexcept { return lhs.rank == rhs.rank && lhs.suit == rhs.suit; };
    friend bool operator!=(const Card& lhs, const Card& rhs) noexcept { return !(lhs == rhs); };
};

class Deck {
   public:
    Deck() noexcept;
    void shuffle() noexcept;
    Card draw();
    std::string toString(const std::string sep = "\n") const;

    static Card getRandomCardExcept(const Card cards[], const u_int8_t cardsLen, const int8_t suit = -1, const u_int8_t ranks[] = {}, const u_int8_t rankLen = 0) noexcept;
    static Card getRandomCardExceptCardsWith(const Card exceptionCards[], const u_int8_t cardsLen, const int8_t suit = -1, const int8_t rank = -1) noexcept;

    friend bool operator==(const Deck& lhs, const Deck& rhs) noexcept {
        if (lhs.len != rhs.len) return false;
        for (u_int8_t i = 0; i < lhs.len; i++) {
            if (lhs.cards[i] != rhs.cards[i]) return false;
        }
        return true;
    };
    friend bool operator!=(const Deck& lhs, const Deck& rhs) noexcept { return !(lhs == rhs); };

   private:
    Card cards[52];
    u_int8_t len = 52;
};