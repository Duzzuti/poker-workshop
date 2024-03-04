#pragma once

/// @brief Represents a type of action that a player has to take in their turn
enum class Actions {
    /// @brief The player wants to discard their hand and leave the round
    FOLD,
    /// @brief The player wants to match the current bet
    CALL,
    /// @brief The player wants to increase the current bet
    RAISE,
    /// @brief The player wants to pass their turn
    CHECK,
    /// @brief The player wants to open the betting round with a bet
    BET,
};

/// @brief Represents a card suit
enum CardSuits {
    /// @brief Diamonds suit
    DIAMONDS = 0,
    /// @brief Hearts suit
    HEARTS,
    /// @brief Spades suit
    SPADES,
    /// @brief Clubs suit
    CLUBS,
};

/// @brief Represents a 5 cards poker hand type
enum HandKinds {
    /// @brief Used as a placeholder for an invalid or undefined hand
    NO_HAND = 0,
    /// @brief The hand does not contain any valuable combination
    HIGH_CARD,
    /// @brief The hand contains a single pair
    PAIR,
    /// @brief The hand contains two pairs
    TWO_PAIR,
    /// @brief The hand contains three cards of the same rank
    THREE_OF_A_KIND,
    /// @brief The hand contains five cards of ascending rank
    STRAIGHT,
    /// @brief The hand contains five cards of the same suit
    FLUSH,
    /// @brief The hand contains three cards of the same rank and another pair
    FULL_HOUSE,
    /// @brief The hand contains four cards of the same rank
    FOUR_OF_A_KIND,
    /// @brief The hand contains five cards of ascending rank and the same suit
    STRAIGHT_FLUSH,
    /// @brief The hand contains A, K, Q, J and 10 of the same suit
    ROYAL_FLUSH,
};

/// @brief Represents the current state of a betting round (preflop, flop, turn, river)
enum class BetRoundState {
    /// @brief There are no community cards yet
    PREFLOP,
    /// @brief The first three community cards are on the table
    FLOP,
    /// @brief The fourth community card is on the table
    TURN,
    /// @brief The fifth community card is on the table
    RIVER
};

/// @brief Represents the state of the whole round
enum class OutEnum {
    /// @brief The round is still ongoing
    ROUND_CONTINUE,
    /// @brief The GAME is won by a player
    GAME_WON,
    /// @brief The ROUND and pot is won by a player
    ROUND_WON,
};

/// @brief This class is used to convert the enum values to strings for logging purposes
class EnumToString {
   public:
    /// @brief Converts OutEnum to a corresponding string
    /// @param outEnum The enum value to convert
    /// @return The string representation of the enum value
    /// @exception Guarantee No-throw
    static const char* enumToString(const OutEnum outEnum) noexcept {
        switch (outEnum) {
            case OutEnum::ROUND_CONTINUE:
                return "CONTINUE";

            case OutEnum::GAME_WON:
                return "GAME WON";

            case OutEnum::ROUND_WON:
                return "ROUND WON";

            default:
                return "INVALID OutEnum value";
        }
    }

    /// @brief Converts Actions to a corresponding string
    /// @param action The enum value to convert
    /// @return The string representation of the enum value
    /// @exception Guarantee No-throw
    static const char* enumToString(const Actions action) noexcept {
        switch (action) {
            case Actions::FOLD:
                return "FOLD";

            case Actions::CALL:
                return "CALL";

            case Actions::RAISE:
                return "RAISE";

            case Actions::CHECK:
                return "CHECK";

            case Actions::BET:
                return "BET";

            default:
                return "INVALID Actions value";
        }
    }

    /// @brief Converts CardSuits to a corresponding string
    /// @param suit The enum value to convert
    /// @return The string representation of the enum value
    /// @exception Guarantee No-throw
    static const char* enumToString(const CardSuits suit) noexcept {
        switch (suit) {
            case CardSuits::DIAMONDS:
                return "DIAMONDS";

            case CardSuits::HEARTS:
                return "HEARTS";

            case CardSuits::SPADES:
                return "SPADES";

            case CardSuits::CLUBS:
                return "CLUBS";

            default:
                return "INVALID CardSuits value";
        }
    }

    /// @brief Converts HandKinds to a corresponding string
    /// @param handKind The enum value to convert
    /// @return The string representation of the enum value
    /// @exception Guarantee No-throw
    static const char* enumToString(const HandKinds handKind) noexcept {
        switch (handKind) {
            case HandKinds::NO_HAND:
                return "NO HAND";

            case HandKinds::HIGH_CARD:
                return "HIGH CARD";

            case HandKinds::PAIR:
                return "PAIR";

            case HandKinds::TWO_PAIR:
                return "TWO PAIR";

            case HandKinds::THREE_OF_A_KIND:
                return "THREE OF A KIND";

            case HandKinds::STRAIGHT:
                return "STRAIGHT";

            case HandKinds::FLUSH:
                return "FLUSH";

            case HandKinds::FULL_HOUSE:
                return "FULL HOUSE";

            case HandKinds::FOUR_OF_A_KIND:
                return "FOUR OF A KIND";

            case HandKinds::STRAIGHT_FLUSH:
                return "STRAIGHT FLUSH";

            case HandKinds::ROYAL_FLUSH:
                return "ROYAL FLUSH";

            default:
                return "INVALID HandKinds value";
        }
    }

    /// @brief Converts BetRoundState to a corresponding string
    /// @param betRoundState The enum value to convert
    /// @return The string representation of the enum value
    /// @exception Guarantee No-throw
    static const char* enumToString(const BetRoundState betRoundState) noexcept {
        switch (betRoundState) {
            case BetRoundState::PREFLOP:
                return "PREFLOP";

            case BetRoundState::FLOP:
                return "FLOP";

            case BetRoundState::TURN:
                return "TURN";

            case BetRoundState::RIVER:
                return "RIVER";

            default:
                return "INVALID BetRoundState value";
        }
    }
};