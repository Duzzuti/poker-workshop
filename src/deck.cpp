#include "deck.h"
#include "logger.h"

std::string Card::toString() const{
    std::string str = "";
    switch (+this->suit){
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
    switch (+this->rank){
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


Deck::Deck() noexcept{
    // generate poker deck
    this->len = 52;
    this->cards = new Card[this->len];
    unsigned char i = 0;
    for(unsigned char suit = 0; suit < 4; suit++){
        for(unsigned char rank = 2; rank < 15; rank++){
            this->cards[i].rank = rank;
            this->cards[i].suit = suit;
            i++;
        }
    }
}

void Deck::shuffle() noexcept{
    // shuffle deck
    for (unsigned char i = this->len - 1; i > 0; --i) {
        unsigned char j = this->dist(this->rng) % (i + 1); // Generate a random index within the unshuffled part of the array

        // Swap characters at indices i and j
        Card temp = this->cards[i];
        this->cards[i] = this->cards[j];
        this->cards[j] = temp;
    }
}

Card Deck::draw(){
    // draw card from deck
    if(this->len == 0){
        PLOG_FATAL << "Deck is empty";
        throw std::logic_error("Deck is empty");
    }
    this->len--;
    return this->cards[this->len];
}

std::string Deck::toString(const std::string sep) const{
    // print deck
    std::string str = "";
    for(unsigned char i = 0; i < this->len; i++){
        str += this->cards[i].toString() + sep;
    }
    return str;
}

