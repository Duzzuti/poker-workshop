#pragma once
#include "data_structs.h"

/// @brief Player class contains functionalities for a player in the game
/// @note Player is an abstract class
class Player {
   public:
    /// @brief Default constructor
    /// @exception Guarantee No-throw
    constexpr Player() noexcept = default;

    /// @brief Gets the name of the player
    /// @return The name of the player in the format: "<playerPosNum>:<name>"
    /// @exception Guarantee No-throw
    /// @see MAX_PLAYER_GET_NAME_LENGTH for the maximum length of the returned string
    const char* getName() const noexcept;

    /// @brief Sets the playerPosNum of the player
    /// @param num The playerPosNum of the player
    /// @exception Guarantee Strong
    /// @throws std::invalid_argument If the number is too high (>= MAX_PLAYERS)
    void setPlayerPosNum(const u_int8_t num) {
        if (num >= MAX_PLAYERS) {
            PLOG_FATAL << "Player position number too high";
            throw std::invalid_argument("Player position number too high");
        }
        this->playerPosNum = num;
    };

    /// @brief Gets the playerPosNum of the player
    /// @return The playerPosNum of the player
    /// @exception Guarantee No-throw
    constexpr u_int8_t getPlayerPosNum() const noexcept { return this->playerPosNum; };

    /// @brief Sets the hand of the player
    /// @param card1 First card
    /// @param card2 Second card
    /// @exception Guarantee No-throw
    void setHand(const Card card1, const Card card2) noexcept { this->hand = std::pair<Card, Card>(card1, card2); };

    /// @brief Gets the hand of the player
    /// @return The hand of the player
    /// @exception Guarantee No-throw
    constexpr const std::pair<Card, Card> getHand() const noexcept { return this->hand; };

    /// @brief Simulates one turn of the player
    /// @param data The data of the game
    /// @param blindOption If true, the player has the blind option (can only call, raise or all-in)
    /// @return The Action of the player
    /// @exception Guarantee No-throw
    /// @note The method is const because it should not change the state of the player
    /// @note This method should be implemented by the child classes
    /// @see Data
    /// @see Action
    virtual Action turn(const Data& data, const bool blindOption = false) const noexcept = 0;

    /// @brief Default destructor
    virtual ~Player() noexcept = default;

   protected:
    /// @brief Player constructor with player name
    /// @param name The name of the player (has to be at most MAX_PLAYER_NAME_LENGTH long)
    /// @exception Guarantee Strong
    /// @throws std::invalid_argument If the name is too long
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the name
    Player(const char* name);

    /// @brief Creates a player name with the player number and name
    /// @param name A name for the player
    /// @param playerNum A number for the player
    /// @param appendNum If true, the player number will be appended to the name
    /// @return The player name in the format: "<name><playerNum>" or "<name>"
    /// @exception Guarantee No-throw
    /// @note Cuts of the name if it is too long, so that the returned string is at most MAX_PLAYER_NAME_LENGTH long
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the returned string
    static const char* createPlayerName(const char* name, const u_int8_t playerNum, const bool appendNum) noexcept;

    /// @brief The name of the player
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the name
    char name[MAX_PLAYER_NAME_LENGTH]{};

   private:
    /// @brief The hand of the player
    /// @see Card
    std::pair<Card, Card> hand;

    /// @brief Player table position number
    u_int8_t playerPosNum = 0;
};