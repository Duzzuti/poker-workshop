#pragma once
#include "player.h"

/// @brief The maximum length of the sequence of actions that the test player can have
const constexpr u_int16_t TEST_PLAYER_ACTION_SIZE = 10;

/// @brief A player that will play a given sequence of actions
/// @note This player is used for testing purposes only
class TestPlayer : public Player {
   public:
    /// @brief Constructor with custom name
    /// @param name The name of the player
    /// @exception Guarantee Strong
    /// @throw std::invalid_argument If the name is too long
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the name
    TestPlayer(const char* name) : Player(name){};

    /// @brief Constructor with default name and custom playerNum
    /// @param playerNum The number of the player which will be appended to the default name
    /// @exception Guarantee No-throw
    /// @note Cuts of the name if it is too long, so that the returned string is at most MAX_PLAYER_NAME_LENGTH long
    /// @see MAX_PLAYER_NAME_LENGTH for the maximum length of the saved name
    TestPlayer(u_int8_t playerNum = 0) noexcept : Player(createPlayerName("TestPlayer", playerNum)){};

    /// @brief Simulates one turn of the rand player
    /// @param data The data of the game
    /// @param onlyRaise If true, the player can only raise or call
    /// @return The Action of the player
    /// @exception Guarantee No-throw
    /// @note The method is const because it should not change the state of the player
    /// @note This method is overridden from the Player class
    /// @note The next action from the sequence is returned
    /// @see Player::turn()
    /// @see Data
    /// @see Action
    Action turn(const Data& data, const bool onlyRaise = false) const noexcept override;

    /// @brief Sets the sequence of actions that will be returned by the turn method
    /// @param actions sequence of actions
    /// @param length length of the sequence
    /// @exception Guarantee Strong
    /// @throws std::invalid_argument if the length is too long
    /// @see TEST_PLAYER_ACTION_SIZE for the maximum length of the sequence
    /// @see Action
    void setActions(const Action actions[], const u_int16_t length) {
        if (length > TEST_PLAYER_ACTION_SIZE) {
            throw std::invalid_argument("Too many actions");
        }
        std::memcpy(this->actions, actions, length * sizeof(Action));
        // reverse the sequence so that the first action is the last index
        // this is done so that the the end of the sequence can be detected (index == 0)
        std::reverse(this->actions, this->actions + length);
        actionIndex = length;
    }

   private:
    /// @brief The sequence of actions that will be returned by the turn method
    Action actions[TEST_PLAYER_ACTION_SIZE];

    /// @brief The index of the last returned action from the sequence
    /// @note The index - 1 is the index of the next action that will be returned
    /// @note The index is equal to zero if the sequence is finished
    mutable u_int16_t actionIndex = 0;
};