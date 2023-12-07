#pragma once

struct Card{
    unsigned char rank;  // 2-14
    unsigned char suit;  // 0-3  (0 = Diamonds, 1 = Hearts, 2 = Spades, 3 = Clubs)

    std::string toString() const;
};

class Deck{
    public:
        Deck() noexcept;
        void shuffle() noexcept;
        Card draw();
        std::string toString(const std::string sep="\n") const;

        ~Deck(){
            delete[] cards;
        };
    private:
        Card* cards;
        unsigned char len;
};