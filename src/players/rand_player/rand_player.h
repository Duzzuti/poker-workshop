#pragma once
#include "player.h"

/// @brief A player that will play randomly
/// @note This player will make random valid moves
class RandPlayer : public Player {
   public:
    /// @brief Constructor with custom name
    /// @param name The name of the player
    /// @param index The index of the player
    /// @exception Guarantee Strong
    /// @throw std::invalid_argument If the name is too long
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the name
    RandPlayer(const char* name, const u_int8_t index) : Player(name, index){};

    /// @brief Constructor with default name and custom playerNum
    /// @param playerNum The number of the player which will be appended to the default name
    /// @param appendNum If true, the player number will be appended to the default name
    /// @param index The index of the player
    /// @exception Guarantee No-throw
    /// @note Cuts of the name if it is too long, so that the returned string is at most MAX_PLAYER_NAME_LENGTH long
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the saved name
    RandPlayer(const u_int8_t playerNum, const bool appendNum, const u_int8_t index) noexcept : Player(createPlayerName("RandPlayer", playerNum, appendNum), index){};

    /// @brief Simulates one turn of the rand player
    /// @param data The data of the game
    /// @param onlyRaise If true, the player can only raise or call
    /// @return The Action of the player
    /// @exception Guarantee No-throw
    /// @note The method is const because it should not change the state of the player
    /// @note This method is overridden from the Player class
    /// @see Player::turn()
    /// @see Data
    /// @see Action
    Action turn(const Data& data, const bool onlyRaise = false) const noexcept override;
};