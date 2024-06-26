#pragma once
#include "config.h"

/// @brief Represents one of the 52 cards in a poker deck
struct Card {
    /// @brief The rank of the card (2-14; 11 = Jack, 12 = Queen, 13 = King, 14 = Ace)
    u_int8_t rank;
    /// @brief The suit of the card (0-3; 0 = Diamonds, 1 = Hearts, 2 = Spades, 3 = Clubs)
    u_int8_t suit;

    /// @brief Gets the string representation of the card
    /// @return The string representation of the card
    /// @exception Guarantee Strong
    /// @throws std::logic_error if the rank or suit is invalid
    /// @see CARD_STR_LEN for the length of the returned string
    const char* toString() const;

    /// @brief Checks if two cards are equal
    /// @param lhs First card
    /// @param rhs Second card
    /// @return True if the cards are equal, false otherwise
    /// @exception Guarantee No-throw
    friend constexpr bool operator==(const Card& lhs, const Card& rhs) noexcept { return lhs.rank == rhs.rank && lhs.suit == rhs.suit; };

    /// @brief Checks if two cards are not equal
    /// @param lhs First card
    /// @param rhs Second card
    /// @return True if the cards are not equal, false otherwise
    /// @exception Guarantee No-throw
    friend constexpr bool operator!=(const Card& lhs, const Card& rhs) noexcept { return !(lhs == rhs); };
};

/// @brief Represents a deck of 52 cards
/// @see Card
class Deck {
    friend class GameTest;

   public:
    /// @brief Generates a new deck of 52 cards
    /// @exception Guarantee No-throw
    constexpr Deck() noexcept {
        // generate a 52 cards poker deck
        u_int8_t i = 0;
        for (u_int8_t suit = 0; suit < 4; suit++) {
            for (u_int8_t rank = 2; rank < 15; rank++) {
                this->cards[i].rank = rank;
                this->cards[i].suit = suit;
                i++;
            }
        }
    }

    /// @brief Shuffles the deck randomly
    /// @exception Guarantee No-throw
    void shuffle() noexcept { std::random_shuffle(&this->cards[0], &this->cards[CARD_NUM]); }

    /// @brief Resets the deck to its initial state
    /// @exception Guarantee No-throw
    /// @note The deck will be in the same order as it was before drawing the first card
    constexpr void reset() noexcept { this->len = CARD_NUM; };

    /// @brief Draws the top card from the deck
    /// @return The top card from the deck
    /// @exception Guarantee Strong
    /// @throws std::logic_error if the deck is empty
    Card draw();

    /// @brief Gets the string representation of the deck
    /// @param sep The separator character between each card
    /// @return The string representation of the deck
    /// @exception Guarantee Strong
    /// @throws std::logic_error if one of the cards in the deck is invalid
    /// @note The string representation is a concatenation of the string representations of all cards in the deck
    const char* toString(const char sep = '\n') const;

    /// @brief Gets a random card
    /// @return A random card
    /// @exception Guarantee No-throw
    /// @note The method is static, it does not interact with the deck. It is considering every card
    /// @note The method is used for debug purposes
    /// @see getRandomCardExcept() if you want to make a more exclusive selection
    static Card getRandomCard() noexcept;

    /// @brief Gets a random card that matches the given criteria
    /// @param cards The cards to EXCLUDE from the selection
    /// @param cardsLen The number of cards in the cards array
    /// @param suit The suit that should be EXCLUDED (-1-3; -1 = None, 0 = Diamonds, 1 = Hearts, 2 = Spades, 3 = Clubs)
    /// @param ranks The ranks that should be EXCLUDED (2-14; 11 = Jack, 12 = Queen, 13 = King, 14 = Ace)
    /// @param rankLen The number of ranks in the ranks array
    /// @return A random card that matches the given criteria
    /// @exception Guarantee No-throw
    /// @note The method is static, it does not interact with the deck. It is considering every card
    /// @note The method is used for debug purposes
    /// @see getRandomCardExceptCardsWith() if you want to make a more inclusive selection
    static Card getRandomCardExcept(const Card cards[], const u_int8_t cardsLen, const int8_t suit = -1, const u_int8_t ranks[] = {}, const u_int8_t rankLen = 0) noexcept;

    /// @brief Gets a random card that is not in the drawnCards array and adds it to the drawnCards array
    /// @param drawnCards The cards to EXCLUDE from the selection
    /// @return A random card that is not in the drawnCards array
    /// @exception Guarantee No-throw
    /// @note The method is static, it does not interact with the deck. It is considering every card
    /// @note The method is used for debug/testing purposes
    /// @note The drawn card will be added to the drawnCards array
    static Card getRandomCardExceptAdd(std::vector<Card>& drawnCards) noexcept;

    /// @brief Gets a random card that matches the given criteria
    /// @param exceptionCards The cards to EXCLUDE from the selection
    /// @param cardsLen The number of cards in the exceptionCards array
    /// @param suit The suit that the card should have (-1-3; -1 = Any, 0 = Diamonds, 1 = Hearts, 2 = Spades, 3 = Clubs)
    /// @param rank The rank that the card should have (-1, 2-14; -1 = Any, 11 = Jack, 12 = Queen, 13 = King, 14 = Ace)
    /// @return A random card that matches the given criteria
    /// @exception Guarantee No-throw
    /// @note The method is static, it does not interact with the deck. It is considering every card
    /// @note The method is used for debug purposes
    /// @see getRandomCardExcept() if you want to make a more exclusive selection
    static Card getRandomCardExceptCardsWith(const Card exceptionCards[], const u_int8_t cardsLen, const int8_t suit = -1, const int8_t rank = -1) noexcept;

    /// @brief Checks if two decks are equal
    /// @param lhs First deck
    /// @param rhs Second deck
    /// @return True if the decks are equal, false otherwise
    /// @exception Guarantee No-throw
    friend constexpr bool operator==(const Deck& lhs, const Deck& rhs) noexcept {
        // If the length is different, the decks are not equal
        if (lhs.len != rhs.len) return false;
        for (u_int8_t i = 0; i < lhs.len; i++) {
            if (lhs.cards[i] != rhs.cards[i]) return false;
        }
        return true;
    };

    /// @brief Checks if two decks are not equal
    /// @param lhs First deck
    /// @param rhs Second deck
    /// @return True if the decks are not equal, false otherwise
    /// @exception Guarantee No-throw
    friend constexpr bool operator!=(const Deck& lhs, const Deck& rhs) noexcept { return !(lhs == rhs); };

   private:
    /// @brief Searches the card in the remaining deck and swaps it with the card at the given position
    /// @param card The card to put at the given position
    /// @param cardPos The position to put the card. 0 is the next card to draw, 1 is the card after that, etc.
    /// @exception Guarantee Strong
    /// @throws std::invalid_argument if the card is not in the remaining deck or invalid
    /// @throws std::invalid_argument if the cardPos is greater than the remaining deck length
    /// @note The method is accessed in GameTest for testing purposes
    /// @note The card will be swapped with the cardPos-th next drawn card in the deck
    /// @note The card needs to be in the remaining deck
    /// @note The cardPos needs to be less than the remaining deck length
    void putCard(const Card card, const u_int8_t cardPos);

    /// @brief The cards in the deck
    Card cards[CARD_NUM]{};
    /// @brief The number of cards in the deck
    /// @note Used to indicate the top card in the deck
    u_int8_t len = CARD_NUM;
};