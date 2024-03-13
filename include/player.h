#pragma once
#include "data_structs.h"

/// @brief Player class contains functionalities for a player in the game
/// @note Player is an abstract class
class Player {
   public:
    /// @brief Default constructor
    /// @param index The index of the player
    /// @exception Guarantee No-throw
    Player(u_int8_t index) noexcept : index(index) {};

    /// @brief Gets the name of the player
    /// @return The name of the player in the format: "<playerPosNum>:<name>"
    /// @exception Guarantee No-throw
    /// @see MAX_PLAYER_GET_NAME_LENGTH for the maximum length of the returned string
    virtual const char* getName() const noexcept final;

    /// @brief Sets the playerPosNum of the player
    /// @param num The playerPosNum of the player
    /// @exception Guarantee No-throw
    virtual void setPlayerPosNum(u_int8_t num) noexcept final { this->playerPosNum = num; };

    /// @brief Sets the hand of the player
    /// @param card1 First card
    /// @param card2 Second card
    /// @exception Guarantee No-throw
    virtual void setHand(const Card card1, const Card card2) noexcept final { this->hand = std::pair<Card, Card>(card1, card2); };

    /// @brief Gets the hand of the player
    /// @return The hand of the player
    /// @exception Guarantee No-throw
    virtual const std::pair<Card, Card> getHand() const noexcept final { return this->hand; };

    /// @brief Simulates one turn of the player
    /// @param data The data of the game
    /// @param onlyRaise If true, the player can only raise or call
    /// @return The Action of the player
    /// @exception Guarantee No-throw
    /// @note The method is const because it should not change the state of the player
    /// @note This method should be implemented by the child classes
    /// @see Data
    /// @see Action
    virtual Action turn(const Data& data, const bool onlyRaise = false) const noexcept = 0;

    /// @brief Default destructor
    virtual ~Player() noexcept = default;

    /// @brief The players index
    const u_int8_t index;

   protected:
    /// @brief Player constructor with player name
    /// @param name The name of the player (has to be at most MAX_PLAYER_NAME_LENGTH long)
    /// @param index The index of the player
    /// @exception Guarantee Strong
    /// @throws std::invalid_argument If the name is too long
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the name
    Player(const char* name, u_int8_t index);

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
    char name[MAX_PLAYER_NAME_LENGTH];

   private:
    /// @brief The hand of the player
    /// @see Card
    std::pair<Card, Card> hand;

    /// @brief Player position number
    u_int8_t playerPosNum = 0;
};