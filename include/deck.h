#pragma once

#include <random>

struct Card{
    unsigned char rank;  // 2-14
    unsigned char suit;  // 0-3  (0 = Diamonds, 1 = Hearts, 2 = Spades, 3 = Clubs)

    std::string toString() const;

    friend bool operator==(const Card& lhs, const Card& rhs) noexcept{
        return lhs.rank == rhs.rank && lhs.suit == rhs.suit;
    };
};

class Deck{
public:
    Deck() noexcept;
    void shuffle() noexcept;
    Card draw();
    std::string toString(const std::string sep="\n") const;

    static Card getRandomCardExcept(const std::vector<Card>& cards, const int8_t suit=-1, const std::vector<u_int8_t> ranks={}) noexcept;

    ~Deck(){
        delete[] cards;
    };
private:
    Card* cards;
    unsigned char len;
    std::random_device dev;
    std::mt19937 rng{dev()};
    std::uniform_int_distribution<std::mt19937::result_type> dist{0, INT32_MAX}; // distribution in max u_int32 range
};