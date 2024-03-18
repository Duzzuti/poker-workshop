#pragma once
#include "check_player/check_player.h"
#include "game.h"
#include "rand_player/rand_player.h"

/// @brief Contains the expected result of a test
struct ResultData {
    /// @brief A bool for each player that is true if the player is out of the game
    bool outPlayers[MAX_PLAYERS];
    /// @brief A bool for each player that is true if the player has folded
    bool foldedPlayers[MAX_PLAYERS];
    /// @brief The number of players that are not out of the game
    u_int8_t nonOutPlayers = 0;
    /// @brief The number of active players in the game (not out and not folded)
    u_int8_t numActivePlayers = 0;
    /// @brief The amount of wins for each player
    u_int32_t gameWins[MAX_PLAYERS];
    /// @brief The pot at the end of the game
    u_int64_t pot;
    /// @brief The chips of the players at the end of the game
    u_int64_t playerChips[MAX_PLAYERS];
    /// @brief The state of the bet round at the end of the game
    BetRoundState betRoundState;
};

/// @brief Contains the configuration for a test
struct TestConfig {
    /// @brief The name of the google test class
    std::string className = "";
    /// @brief The name of the google test
    std::string testName = "";
    /// @brief The number of players in the game
    u_int8_t numPlayers;
    /// @brief The small blind of the game
    u_int64_t smallBlind;
    /// @brief The starting chips of the players
    std::array<u_int64_t, MAX_PLAYERS> playerChips;
    /// @brief The hands of the players
    std::pair<Card, Card> playerHands[MAX_PLAYERS];
    /// @brief Stores the cards that have been drawn from the deck and should not be drawn again
    Card drawnCards[MAX_DRAWN_CARDS];
    /// @brief The community cards
    Card communityCards[5];
    /// @brief The actions for each player
    std::vector<Action> playerActions[MAX_PLAYERS];
    /// @brief The expected output of the game
    ResultData resultData;
};

/// @brief Contains all test configurations for a file
struct FileConfig {
    /// @brief The name of the test file where the test should be stored
    std::string fileName = "test_gametest.cpp";
    /// @brief The name of the cmake test that should be created
    std::string cmakeTestName = "test";
    /// @brief The configurations for the tests in the file
    std::vector<TestConfig> config;
};

/// @brief Mocks the Game class for testing
class GameTest : public Game {
   public:
    /// @copydoc Game::Game(const Config&)
    GameTest(const Config& config) noexcept : Game(config) {}

    /// @brief Sets the players of the game
    /// @param players The players of the game should be the same length as Config.numPlayers
    /// @exception Guarantee No-throw
    /// @note The players are moved into the GameTest object
    /// @note Undefined behavior if the length of players is not equal to config.numPlayers
    void setPlayers(std::unique_ptr<Player> players[]) noexcept {
        for (int i = 0; i < this->config.numPlayers; i++) {
            this->players[i] = std::move(players[i]);
        }
    }

    /// @brief Sets the players of the game to a mix of different players that are not for human or testing
    /// @exception Guarantee No-throw
    /// @note Requires Config.numPlayers to be 5, undefined behavior otherwise
    /// @see CheckPlayer
    /// @see RandPlayer
    void setRealisticPlayers() noexcept {
        // set different players
        this->players[0] = std::move(std::make_unique<CheckPlayer>(std::string("a", MAX_PLAYER_NAME_LENGTH).c_str()));
        this->players[1] = std::move(std::make_unique<RandPlayer>(2));
        this->players[2] = std::move(std::make_unique<CheckPlayer>(3));
        this->players[3] = std::move(std::make_unique<RandPlayer>(4));
        this->players[4] = std::move(std::make_unique<RandPlayer>(5));
    }

    /// @brief Sets up the deck for the game, so that the whished cards will be drawn
    /// @param playerHands The desired hands of the players
    /// @param numPlayers The number of players (length of playerHands)
    /// @param communityCards The desired community cards
    /// @exception Guarantee Basic
    /// @throw std::invalid_argument if there are more than 52 cards in the desired deck
    /// @note Undefined behavior if the length of playerHands is not equal to numPlayers
    /// @note The order will be: player 1 hand, player 2 hand, ..., player numPlayers hand, community cards
    /// @note REQUIRES: shufflePlayers and shuffleDeck to be false in Config
    /// @see Deck::putCard()
    void buildDeck(const std::pair<Card, Card> playerHands[], const u_int8_t numPlayers, const Card communityCards[5]) {
        this->deck = Deck{};
        u_int8_t deckInd = 0;
        for (u_int8_t i = 0; i < numPlayers; i++) {
            this->deck.putCard(playerHands[i].first, deckInd++);
            this->deck.putCard(playerHands[i].second, deckInd++);
        }
        for (u_int8_t i = 0; i < 5; i++) {
            this->deck.putCard(communityCards[i], deckInd++);
        }
    }

    /// @copydoc Game::initPlayerOrder()
    void initPlayerOrder() noexcept { Game::initPlayerOrder(); }

    /// @copydoc Game::startRound()
    void startRound(const bool firstRound) { Game::startRound(firstRound); }

    /// @copydoc Game::setBlinds()
    void setBlinds() noexcept { Game::setBlinds(); }

    /// @copydoc Game::setupBetRound()
    void setupBetRound() noexcept { Game::setupBetRound(); }

    /// @copydoc Game::betRound()
    OutEnum betRound() { return Game::betRound(); }

    /// @copydoc Game::bet()
    bool bet(const u_int64_t amount) noexcept { return Game::bet(amount); }

    /// @copydoc Game::playerOut()
    OutEnum playerOut(const char* reason) noexcept { return Game::playerOut(reason); }

    /// @copydoc Game::playerFolded()
    OutEnum playerFolded() noexcept { return Game::playerFolded(); }

    /// @copydoc Game::getOutEnum()
    OutEnum getOutEnum() const noexcept { return Game::getOutEnum(); }

    /// @copydoc Game::preflop()
    void preflop() { Game::preflop(); }

    /// @copydoc Game::flop()
    void flop() { Game::flop(); }

    /// @copydoc Game::turn()
    void turn() { Game::turn(); }

    /// @copydoc Game::river()
    void river() { Game::river(); }

    /// @copydoc Game::getPlayerInfo()
    const char* getPlayerInfo(u_int8_t playerPos = MAX_PLAYERS, const int64_t chipsDiff = 0) const noexcept { return Game::getPlayerInfo(playerPos, chipsDiff); }

    /// @copydoc Game::data
    Data& getData() noexcept { return this->data; }

    /// @copydoc Game::players
    std::unique_ptr<Player>*& getPlayers() noexcept { return this->players; }

    /// @copydoc Game::deck
    Deck& getDeck() noexcept { return this->deck; }
};