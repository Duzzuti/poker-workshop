#pragma once
#include <memory>

#include "player.h"

/// @brief Simulates a set of poker games
/// @note Holds all required Data
class Game {
    /// @brief Used for unit testing
    friend class GameTest;

   public:
    /// @brief Creates a Game object with the given Config
    /// @param config Holds all important settings that the simulation should consider
    /// @exception Guarantee No-throw
    /// @see Config
    Game(const Config& config) noexcept : config(config), players(new std::unique_ptr<Player>[config.numPlayers]) {}

    /// @brief Runs the simulation
    /// @param initPlayers Whether the method is responsible for initializing the players
    /// @exception Guarantee None
    /// @throws std::invalid_argument if the player name is too long
    /// @throws std::logic_error if the deck is empty
    /// @throws std::logic_error if the player could not call a matched bet
    /// @throws std::logic_error if the player choose an action which is not covered
    /// @note The simulation is changing the data field. This is where the results are stored.
    /// @note Undefined behavior if the players are not initialized externally and initPlayers is set to false
    void run(const bool initPlayers = true);

    /// @brief Frees the allocated memory for the players
    /// @exception Guarantee No-throw
    ~Game() { delete[] this->players; }

   private:
    /// @brief Gets a string with the current player info in the format: "pos:name[chips <+/-> diff]"
    /// @param playerPos The player position or MAX_PLAYERS for the current player
    /// @param chipsDiff The chip difference which is shown in the string or 0 for not showing
    /// @return The string with the player info
    /// @exception Guarantee No-throw
    /// @note The difference is used to show how the player´s chips changed
    /// @note If chipsDiff is 0 then the output format is: "pos:name[chips]"
    /// @see MAX_GET_PLAYER_INFO_LENGTH for the maximum length of the returned string
    const char* getPlayerInfo(u_int8_t playerPos = MAX_PLAYERS, const int64_t chipsDiff = 0) const noexcept;

    /// @brief Shuffles the players, sets their positions and sets the winners array
    /// @exception Guarantee No-throw
    /// @note The number of wins is stored in the player objects, it is extracted once at the game start and stored in the winners array
    /// @see Player
    void initPlayerOrder() noexcept;

    /// @brief Starts a round by shuffling the deck, setting the dealer and the blinds and dealing the cards
    /// @param firstRound True if this is the first round of the game
    /// @exception Guarantee Basic
    /// @throws std::logic_error if the deck is empty
    /// @note firstRound is used to determine if the dealer should be set to 0 or to the next player after the last dealer
    void startRound(const bool firstRound);

    /// @brief Sets the blinds for the round by betting the small and big blind automatically
    /// @return An OutEnum which indicates if the game or round should continue
    /// @exception Guarantee No-throw
    /// @note The heads up rule is considered
    /// @note If there are players who are out due to the blinds, the game could end
    /// @see OutEnum
    OutEnum setBlinds() noexcept;

    /// @brief Sets up the data for a bet round (preflop, flop, turn, river)
    /// @exception Guarantee No-throw
    /// @note Resets the bet round data and skips to the next player after the dealer
    void setupBetRound() noexcept;

    /// @brief Runs a single bet round (preflop, flop, turn, river)
    /// @return An OutEnum which indicates if the game or round should continue
    /// @exception Guarantee Basic
    /// @throws std::logic_error if the player could not call a matched bet
    /// @throws std::logic_error if the player choose an action which is not covered
    /// @note The betting is done in a loop until all players have checked, called or folded
    /// @note Due to the fact that players fold or could be out, the game or round could end
    /// @see OutEnum
    OutEnum betRound();

    /// @brief Checks if the bet round should continue
    /// @param firstChecker The first player who checked in this bet round
    /// @return True if the bet round should continue
    /// @exception Guarantee No-throw
    /// @note The bet round should continue if the current player has not checked or called the current bet
    /// @note The live big blind rule is considered
    bool betRoundContinue(const u_int8_t firstChecker) const noexcept;

    /// @brief Checks if the current player is active (not out or folded)
    /// @return True if the current player is active
    /// @exception Guarantee No-throw
    bool currentPlayerActive() const noexcept;

    /// @brief Checks if special condition is met that the current player can only raise or call
    /// @return True if the current player can only raise or call
    /// @exception Guarantee No-throw
    /// @note This is used to consider the live big blind rule
    bool currentPlayerCanOnlyRaiseOrCall() const noexcept;

    /// @brief Simulates a single non out player turn
    /// @param firstChecker The first player who checked in this bet round
    /// @return An OutEnum which indicates if the game or round should continue
    /// @exception Guarantee Strong
    /// @throws std::logic_error if the player choose an action which is not covered
    /// @note The player`s turn is called to get the action
    /// @note The action is checked and executed
    /// @note The player could be out or folded, the game or round could end
    /// @see OutEnum
    /// @see Player::turn
    OutEnum playerTurn(u_int8_t& firstChecker);

    /// @brief Simulates a single non out player turn where the player can only raise or call
    /// @return An OutEnum which indicates if the game or round should continue
    /// @exception Guarantee Strong
    /// @throws std::logic_error if the player could not call a matched bet
    /// @note Used to consider the live big blind rule
    /// @note The player`s turn is called to get the action
    /// @note The action is checked and executed
    /// @note The player could be out or folded, the game or round could end
    /// @see OutEnum
    /// @see Player::turn
    OutEnum playerTurnOnlyRaise();

    /// @brief The current player bets the amount and the next player is selected
    /// @param amount The total amount that the player bets
    /// @param isBlind True if the player is betting the small or big blind
    /// @return True if the bet was successful
    /// @exception Guarantee No-throw
    /// @note The amount is the total amount that the player bets (e.g. if the player has to call 200 but he already bet 100 => amount is still 200)
    /// @note if the player is betting the small or big blind, the amount is not checked against the minimum bet
    /// @note If the betting is not successful, no data is changed
    bool bet(const u_int64_t amount, const bool isBlind = false) noexcept;

    /// @brief Marks the current player as out and selects the next player
    /// @param reason The reason why the player is out, is used for logging
    /// @return An OutEnum which indicates if the game or round should continue
    /// @exception Guarantee No-throw
    /// @note If only one player is remaining in this round or game, the game or round ends
    /// @see OutEnum
    OutEnum playerOut(const char* reason) noexcept;

    /// @brief Marks the current player as folded and selects the next player
    /// @return An OutEnum which indicates if the game or round should continue
    /// @exception Guarantee No-throw
    /// @note If only one player is remaining in this round, the round ends
    /// @see OutEnum
    OutEnum playerFolded() noexcept;

    /// @brief Checks if the game or round should continue
    /// @return An OutEnum which indicates if the game or round should continue
    /// @exception Guarantee No-throw
    /// @note The game should continue if there are at least two players who are not out
    /// @note The round should continue if there are at least two players who are not out and have not folded
    /// @see OutEnum
    OutEnum getOutEnum() const noexcept;

    /// @brief Simulates the preflop betting round
    /// @exception Guarantee Basic
    /// @throws std::logic_error if the player could not call a matched bet
    /// @throws std::logic_error if the player choose an action which is not covered
    /// @note Calls the betRound() method
    /// @see betRound()
    void preflop();

    /// @brief Simulates the flop betting round
    /// @exception Guarantee Basic
    /// @throws std::logic_error if the player could not call a matched bet
    /// @throws std::logic_error if the player choose an action which is not covered
    /// @throws std::logic_error if the deck is empty
    /// @note Draws the flop community cards and calls the betRound() method
    /// @see betRound()
    void flop();

    /// @brief Simulates the turn betting round
    /// @exception Guarantee Basic
    /// @throws std::logic_error if the player could not call a matched bet
    /// @throws std::logic_error if the player choose an action which is not covered
    /// @throws std::logic_error if the deck is empty
    /// @note Draws the turn community card and calls the betRound() method
    /// @see betRound()
    void turn();

    /// @brief Simulates the river betting round
    /// @exception Guarantee Basic
    /// @throws std::logic_error if the player could not call a matched bet
    /// @throws std::logic_error if the player choose an action which is not covered
    /// @throws std::logic_error if the deck is empty
    /// @note Draws the river community card and calls the betRound() method
    /// @see betRound()
    void river();

    /// @brief The Config object which holds all settings for the simulation
    /// @note This is passed to the constructor
    /// @see Config
    const Config config;

    /// @brief The players array which holds all players for the simulation
    /// @note This is allocated in the constructor on the heap because the Player object is abstract
    /// @see Player
    std::unique_ptr<Player>* players;

    /// @brief The Data object which holds all data and results of the simulation
    /// @see Data
    Data data;

    /// @brief The Deck object which holds all cards for the simulation
    /// @note This is initialized every round
    /// @see Deck
    Deck deck;
};
