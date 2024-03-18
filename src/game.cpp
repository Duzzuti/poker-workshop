#include "game.h"

#include <algorithm>

#include "check_player/check_player.h"
#include "hand_strengths.h"
#include "human_player/human_player.h"
#include "rand_player/rand_player.h"

void Game::run(const bool initPlayers) {
    // config players
    // init players
    if (initPlayers) {
        this->players[0] = std::move(std::make_unique<CheckPlayer>(1));
        this->players[1] = std::move(std::make_unique<RandPlayer>(2));
        this->players[2] = std::move(std::make_unique<CheckPlayer>(3));
        this->players[3] = std::move(std::make_unique<RandPlayer>(4));
        this->players[4] = std::move(std::make_unique<RandPlayer>(5));
    }

    this->data.numPlayers = this->config.numPlayers;
    // reset winners
    std::memset(this->data.gameData.gameWins, 0, sizeof(this->data.gameData.gameWins));
    std::memset(this->data.gameData.chipWins, 0, sizeof(this->data.gameData.chipWins));
    std::memset(this->data.gameData.chipWinsAmount, 0, sizeof(this->data.gameData.chipWinsAmount));

    char winnerString[MAX_POT_DIST_STRING_LENGTH];

    // run for the number of games specified in the config
    for (u_int64_t game = 0; game < this->config.numGames; game++) {
        // ONE GAME
        // shuffle players
        PLOG_DEBUG << "Starting game " << game;
        this->initPlayerOrder();
        this->data.gameData.numNonOutPlayers = this->config.numPlayers;
        // reset player out
        std::memset(this->data.gameData.playerOut, 0, sizeof(this->data.gameData.playerOut));
        for (u_int8_t i = 0; i < this->config.numPlayers; i++) this->data.gameData.playerChips[i] = this->config.startingChips[i];
        int16_t round = -1;

        while (this->data.gameData.numNonOutPlayers > 1) {
            if (this->config.maxRounds >= 0 && round >= this->config.maxRounds - 1) {
                // find the player with the most chips
                this->data.nextActivePlayer();
                u_int8_t firstActivePlayer = this->data.betRoundData.playerPos;
                u_int8_t maxChipsPlayers[MAX_PLAYERS] = {firstActivePlayer};
                u_int8_t maxChipsPlayersCount = 1;
                this->data.nextActivePlayer();
                while (this->data.betRoundData.playerPos != firstActivePlayer) {
                    if (this->data.getChips() > this->data.gameData.playerChips[maxChipsPlayers[0]]) {
                        maxChipsPlayers[0] = this->data.betRoundData.playerPos;
                        maxChipsPlayersCount = 1;
                    } else if (this->data.getChips() == this->data.gameData.playerChips[maxChipsPlayers[0]]) {
                        maxChipsPlayers[maxChipsPlayersCount++] = this->data.betRoundData.playerPos;
                    }
                    this->data.nextActivePlayer();
                }
                // set the players with the most chips as the winner
                for (u_int8_t i = 0; i < maxChipsPlayersCount; i++) this->data.gameData.gameWins[maxChipsPlayers[i]]++;
                this->data.roundData.result = OutEnum::GAME_WON;
                winnerString[0] = '\0';
                for (u_int8_t i = 0; i < maxChipsPlayersCount; i++) {
                    // depending MAX_POT_DIST_STRING_LENGTH
                    std::strncat(winnerString, this->getPlayerInfo(maxChipsPlayers[i]), MAX_GET_PLAYER_INFO_LENGTH);
                    if (i != maxChipsPlayersCount - 1) std::strcat(winnerString, ", ");
                }
                PLOG_INFO << "Game " << game << " ended in round " << round << "\nWINNER IS " << winnerString << "\n\n";
                break;
            }
            // ONE ROUND
            round++;
            if (this->config.shuffleDeck)
                this->deck = Deck();
            else
                this->deck.reset();
            this->data.roundData.result = OutEnum::ROUND_CONTINUE;
            this->data.roundData.numActivePlayers = this->data.gameData.numNonOutPlayers;
            PLOG_DEBUG << "Starting round " << round;
            this->startRound(round == 0);
            // PREFLOP
            this->preflop();
            // FLOP
            this->flop();
            // TURN
            this->turn();
            // RIVER
            this->river();

            if (this->data.roundData.result == OutEnum::ROUND_WON) {
                // switch to the winner
                this->data.nextActivePlayer();
                PLOG_DEBUG << "Pot of " << this->data.roundData.pot << " won by " << this->getPlayerInfo(MAX_PLAYERS, this->data.roundData.pot) << ". Starting new round";
                this->data.gameData.playerChips[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                this->data.gameData.chipWins[this->data.betRoundData.playerPos]++;
                this->data.gameData.chipWinsAmount[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                continue;
            } else if (this->data.roundData.result == OutEnum::GAME_WON) {
                // switch to the winner
                this->data.nextActivePlayer();
                this->data.gameData.playerChips[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                this->data.gameData.gameWins[this->data.betRoundData.playerPos]++;
                this->data.gameData.chipWins[this->data.betRoundData.playerPos]++;
                this->data.gameData.chipWinsAmount[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                PLOG_INFO << "Game " << game << " ended in round " << round << "\nWINNER IS " << this->getPlayerInfo() << "\n\n";
                break;
            }

            // SHOWDOWN
            PLOG_DEBUG << "SHOWDOWN!!! Community cards: " << this->data.roundData.communityCards[0].toString() << " " << this->data.roundData.communityCards[1].toString() << " "
                       << this->data.roundData.communityCards[2].toString() << " " << this->data.roundData.communityCards[3].toString() << " " << this->data.roundData.communityCards[4].toString();
            // get hand strength for each player
            HandStrengths handStrengths[data.numPlayers];
            HandStrengths::getHandStrengths(this->players, this->data, handStrengths);
            // get winner

            HandStrengths strongestHand = HandStrengths(HandKinds::NO_HAND, 0);
            u_int8_t winners[this->data.numPlayers];
            u_int8_t numWinners = 0;

            for (u_int8_t i = 0; i < this->data.numPlayers; i++) {
                if (this->data.roundData.playerFolded[i] || this->data.gameData.playerOut[i]) continue;
                PLOG_DEBUG << this->getPlayerInfo(i) << " has hand " << this->players[i]->getHand().first.toString() << " " << this->players[i]->getHand().second.toString() << " and hand strength "
                           << EnumToString::enumToString(handStrengths[i].handkind) << " " << handStrengths[i].rankStrength;
                if (handStrengths[i] > strongestHand) {
                    strongestHand = handStrengths[i];
                    numWinners = 1;
                    winners[0] = i;
                } else if (handStrengths[i] == strongestHand) {
                    winners[numWinners++] = i;
                }
            }

            // distribute pot
            u_int64_t potPerWinner = this->data.roundData.pot / numWinners;
            winnerString[0] = '\0';
            for (u_int8_t i = 0; i < numWinners; i++) {
                // depending MAX_POT_DIST_STRING_LENGTH
                std::strncat(winnerString, this->getPlayerInfo(winners[i], potPerWinner), MAX_GET_PLAYER_INFO_LENGTH);
                this->data.gameData.playerChips[winners[i]] += potPerWinner;
                this->data.gameData.chipWins[winners[i]]++;
                this->data.gameData.chipWinsAmount[winners[i]] += potPerWinner;
                if (i != numWinners - 1) std::strcat(winnerString, ", ");
            }
            PLOG_DEBUG << "Pot of " << this->data.roundData.pot << " won by " << winnerString << ". Starting new round";
        }
    }
    PLOG_INFO << "Statistics: \n";
    // sort players by wins
    std::pair<u_int8_t, u_int32_t> winners[this->data.numPlayers];
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) winners[i] = std::make_pair(i, this->data.gameData.gameWins[i]);
    std::sort(&winners[0], &winners[this->config.numPlayers], [](const std::pair<u_int8_t, u_int32_t>& a, const std::pair<u_int8_t, u_int32_t>& b) { return a.second > b.second; });
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) PLOG_INFO << STR_PLAYER << this->players[winners[i].first]->getName() << " won " << winners[i].second << " games";
    PLOG_INFO << "\n";
}

const char* Game::getPlayerInfo(u_int8_t playerPos, const int64_t chipsDiff, const int64_t baseChipsDiff) const noexcept {
    // create player info string with MAX_GET_PLAYER_INFO_LENGTH
    static char playerInfo[MAX_GET_PLAYER_INFO_LENGTH];
    // if playerPos == MAX_PLAYERS, use the current player
    if (playerPos == MAX_PLAYERS) playerPos = this->data.betRoundData.playerPos;
    // if chipsDiff == 0, do not add chipsDiff
    if (chipsDiff == 0)
        std::snprintf(playerInfo, sizeof(playerInfo), "%s%s[%lu]", STR_PLAYER, this->players[playerPos]->getName(), this->data.gameData.playerChips[playerPos] + baseChipsDiff);
    else
        std::snprintf(playerInfo, sizeof(playerInfo), "%s%s[%lu%s%lu]", STR_PLAYER, this->players[playerPos]->getName(), this->data.gameData.playerChips[playerPos] + baseChipsDiff,
                      chipsDiff > 0 ? " + " : " - ", std::abs(chipsDiff));
    return playerInfo;
}

void Game::initPlayerOrder() noexcept {
    // shuffle player order
    if (this->config.shufflePlayers) std::random_shuffle(&this->players[0], &this->players[this->config.numPlayers]);
    PLOG_INFO << "Shuffled players, new order:";
    // rearrange the game stats according to the new player order
    u_int32_t gameWinsCopy[MAX_PLAYERS];
    u_int32_t chipWinsCopy[MAX_PLAYERS];
    u_int64_t chipWinsAmountCopy[MAX_PLAYERS];
    std::memcpy(gameWinsCopy, this->data.gameData.gameWins, sizeof(gameWinsCopy));
    std::memcpy(chipWinsCopy, this->data.gameData.chipWins, sizeof(chipWinsCopy));
    std::memcpy(chipWinsAmountCopy, this->data.gameData.chipWinsAmount, sizeof(chipWinsAmountCopy));
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) {
        // set playerPosNum for each player
        this->data.gameData.gameWins[i] = gameWinsCopy[this->players[i]->getPlayerPosNum()];
        this->data.gameData.chipWins[i] = chipWinsCopy[this->players[i]->getPlayerPosNum()];
        this->data.gameData.chipWinsAmount[i] = chipWinsAmountCopy[this->players[i]->getPlayerPosNum()];
        this->players[i]->setPlayerPosNum(i);
        PLOG_INFO << this->players[i]->getName();
    }
}

void Game::setBlinds() noexcept {
    // blinds
    if (this->data.gameData.numNonOutPlayers == 2) {
        // heads up rule (small blind is the dealer)
        // adjust button if the big blind would be the same player again
        this->data.roundData.dealerPos = this->data.roundData.bigBlindPos;
        this->data.betRoundData.playerPos = this->data.roundData.dealerPos;
    }
    // if the player can not bet the small blind, he is all-in
    this->data.roundData.smallBlindPos = this->data.betRoundData.playerPos;
    const u_int64_t smallBlindBet = this->betBlind(this->data.roundData.smallBlind);
    if (this->data.getChips() == 0)
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -smallBlindBet, smallBlindBet) << " is all-in with small blind " << this->data.roundData.smallBlind;
    else
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -smallBlindBet, smallBlindBet) << " bets small blind " << this->data.roundData.smallBlind;

    this->data.nextPlayer();

    // big blind analog to small blind
    this->data.roundData.bigBlindPos = this->data.betRoundData.playerPos;
    const u_int64_t bigBlindBet = this->betBlind(this->data.roundData.bigBlind);
    if (this->data.getChips() == 0)
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -bigBlindBet, bigBlindBet) << " is all-in with big blind " << this->data.roundData.bigBlind;
    else
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -bigBlindBet, bigBlindBet) << " bets big blind " << this->data.roundData.bigBlind;

    this->data.nextPlayer();
}

void Game::setupBetRound() noexcept {
    // resets the bet round data, first to act is the player after the dealer
    this->data.betRoundData.playerPos = this->data.roundData.dealerPos;
    this->data.nextPlayer();
    // reset player bets
    std::memset(this->data.betRoundData.playerBets, 0, sizeof(this->data.betRoundData.playerBets));
    this->data.betRoundData.currentBet = 0;
}

void Game::startRound(const bool firstRound) {
    // reset deck and round data
    if (this->config.shuffleDeck) this->deck.shuffle();
    // select new dealer
    u_int8_t lastDealerPos = this->data.roundData.dealerPos;
    this->data.selectDealer(firstRound);

    this->data.roundData.addBlind = this->config.addBlindPerDealer0;
    if (firstRound) this->data.roundData.smallBlind = this->config.smallBlind;
    // if the dealer is at position 0 again (or skipped 0), add the addBlind amount
    else if (this->data.roundData.dealerPos < lastDealerPos)
        this->data.roundData.smallBlind += this->config.addBlindPerDealer0;
    // big blind is always double the small blind
    this->data.roundData.bigBlind = this->data.roundData.smallBlind * 2;
    this->data.roundData.pot = 0;
    // reset player folded
    std::memset(this->data.roundData.playerFolded, 0, sizeof(this->data.roundData.playerFolded));
    this->setupBetRound();

    // deal cards
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) {
        if (this->data.gameData.playerOut[i]) continue;
        this->players[i]->setHand(this->deck.draw(), this->deck.draw());
    }
    // first action is setting the blinds
    if (firstRound) this->data.roundData.bigBlindPos = 0;  // fix first round heads up
    this->setBlinds();
}

OutEnum Game::betRound() {
    // position of the first player that checked or MAX_PLAYERS if no player checked yet
    u_int8_t firstChecker = MAX_PLAYERS;
    this->data.betRoundData.minimumRaise = this->data.roundData.bigBlind;
    // this loop will run until all players have either folded, checked or called
    // we can only exit if it is a players turn and he is in the game, has the same bet as the current bet and all players have checked if the bet is 0
    // we need to consider the case where every player folds except one, then the last player wins the pot
    while (this->betRoundContinue(firstChecker)) {
        if (!this->currentPlayerActive()) {
            // player is out of the game or folded, skip turn
            this->data.nextPlayer();
            continue;
        } else if (this->currentPlayerBlindOption()) {
            // player has the blind option
            OutEnum turnRes = this->playerTurnBlindOption();
            if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
        } else {
            OutEnum turnRes = this->playerTurn(firstChecker);
            if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
        }
    }

    PLOG_DEBUG << "Bet round finished with bet " << this->data.betRoundData.currentBet << " and pot " << this->data.roundData.pot;

    // TODO bet round result
    return OutEnum::ROUND_CONTINUE;
}

bool Game::betRoundContinue(const u_int8_t firstChecker) const noexcept {
    return !this->currentPlayerActive() ||                                                                                 // player is out of the game or folded, should be skipped
           this->data.betRoundData.currentBet != this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] ||  // current bet is not called by the player
           (this->data.betRoundData.currentBet == 0 && firstChecker != this->data.betRoundData.playerPos) ||               // current bet is 0 and the current player is not the first checker
           this->currentPlayerBlindOption();                                                                               // current player is the live big blind in the preflop round
}

bool Game::currentPlayerActive() const noexcept { return !this->data.gameData.playerOut[this->data.betRoundData.playerPos] && !this->data.roundData.playerFolded[this->data.betRoundData.playerPos]; }

bool Game::currentPlayerBlindOption() const noexcept {
    // current bet is the big blind and the current player is the big blind and it is the preflop round (the big blind can raise in the preflop round)
    return this->data.roundData.betRoundState == BetRoundState::PREFLOP && this->data.betRoundData.currentBet == this->data.roundData.bigBlind &&
           this->data.betRoundData.playerPos == this->data.roundData.bigBlindPos;
}

OutEnum Game::playerTurn(u_int8_t& firstChecker) {
    // get action from player
    Action action = this->players[this->data.betRoundData.playerPos]->turn(this->data);
    OutEnum res;
    // get player info string
    const char* playerInfo = this->getPlayerInfo();
    // store the error message if the action is illegal
    char str[MAX_ACTION_ERROR_LENGTH];
    switch (action.action) {
        case Actions::FOLD:
            // player folded
            PLOG_DEBUG << playerInfo << " folded";
            res = playerFolded();
            if (res != OutEnum::ROUND_CONTINUE) return res;
            break;

        case Actions::CHECK:
            // player checked
            if (this->data.betRoundData.currentBet != 0) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_CHECK_ERROR, this->data.betRoundData.currentBet);
                res = playerOut(str);
                if (res != OutEnum::ROUND_CONTINUE) return res;
            } else {
                PLOG_DEBUG << playerInfo << " checked";
                // if the player is the first checker, set firstChecker to the player position
                if (firstChecker == MAX_PLAYERS) firstChecker = this->data.betRoundData.playerPos;
                this->data.nextPlayer();
            }
            break;

        case Actions::CALL:
            // player called, set custom player info string with call data
            playerInfo = this->getPlayerInfo(MAX_PLAYERS, -this->data.getCallAdd());
            if (!this->bet(this->data.betRoundData.currentBet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_CALL_ERROR, this->data.betRoundData.currentBet);
                res = playerOut(str);
                if (res != OutEnum::ROUND_CONTINUE) return res;
                break;
            }
            PLOG_DEBUG << playerInfo << " called";
            break;

        case Actions::RAISE:
            // player raised, set custom player info string with raise data
            playerInfo = this->getPlayerInfo(MAX_PLAYERS, -this->data.getRaiseAdd(action.bet));
            if (this->data.betRoundData.currentBet == 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_RAISE_ERROR, action.bet);
                res = playerOut(str);
                if (res != OutEnum::ROUND_CONTINUE) return res;
                break;
            }
            PLOG_DEBUG << playerInfo << " raised to " << action.bet;
            break;

        case Actions::BET:
            // player bet, set custom player info string with bet data
            playerInfo = this->getPlayerInfo(MAX_PLAYERS, -action.bet);
            if (this->data.betRoundData.currentBet != 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_BET_ERROR, action.bet);
                res = playerOut(str);
                if (res != OutEnum::ROUND_CONTINUE) return res;
                break;
            }
            PLOG_DEBUG << playerInfo << " bet " << action.bet;
            break;

        default:
            // some action is not handled by the switch statement
            throw std::logic_error("Invalid action");
            PLOG_FATAL << "Invalid action: " << static_cast<int>(action.action);
    }
    return OutEnum::ROUND_CONTINUE;
}

OutEnum Game::playerTurnBlindOption() {
    // get action from player
    Action action = this->players[this->data.betRoundData.playerPos]->turn(this->data, true);
    OutEnum res;
    // get player info string
    const char* playerInfo = this->getPlayerInfo();
    // store the error message if the action is illegal
    char str[MAX_ACTION_ERROR_LENGTH_ONLY_RAISE];
    switch (action.action) {
        case Actions::CALL:
            // player called, does not add chips to the pot
            PLOG_DEBUG << playerInfo << " called";
            if (!this->bet(this->data.betRoundData.currentBet)) {
                // this move is not adding chips to the pot, so it can not be illegal
                PLOG_FATAL << "Player " << this->data.betRoundData.playerPos << " called but could not bet";
                throw std::logic_error("Player called but could not bet");
                break;
            }
            break;

        case Actions::RAISE:
            // player raised, set custom player info string with raise data
            playerInfo = this->getPlayerInfo(MAX_PLAYERS, -this->data.getRaiseAdd(action.bet));
            if (this->data.betRoundData.currentBet == 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_RAISE_ERROR, action.bet);
                res = playerOut(str);
                if (res != OutEnum::ROUND_CONTINUE) return res;
                break;
            }
            PLOG_DEBUG << playerInfo << " raised to " << action.bet;
            break;

        default:
            // illegal move leads to loss of the game
            // set up error message
            std::snprintf(str, sizeof(str), "%s%i", STR_ACTION_ERROR, static_cast<int>(action.action));
            res = playerOut(str);
            if (res != OutEnum::ROUND_CONTINUE) return res;
    }
    return OutEnum::ROUND_CONTINUE;
}

u_int64_t Game::betBlind(const u_int64_t blind) noexcept {
    // bet the blind, if the player can not bet the blind, he is all-in
    bool success = this->data.removeChips(blind);
    this->data.betRoundData.currentBet = blind;
    if (!success) {
        // TODO player is all-in
        u_int64_t allInAmount = this->data.getChips();
        this->data.removeChipsAllIn();
        this->data.roundData.pot += allInAmount;
        this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] = allInAmount;
        return allInAmount;
    }
    this->data.roundData.pot += blind;
    this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] = blind;
    return blind;
}

bool Game::bet(const u_int64_t amount) noexcept {
    // amount is the whole bet, not the amount that is added to the pot
    if (((amount < this->data.betRoundData.currentBet) ||                                                                                            // call condition
         ((amount > this->data.betRoundData.currentBet) && (amount < this->data.betRoundData.currentBet + this->data.betRoundData.minimumRaise)) ||  // raise condition
         (amount < this->data.roundData.bigBlind))                                                                                                   // bet condition
    )
        return false;
    u_int64_t addAmount = amount - this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];
    bool success = this->data.removeChips(addAmount);
    if (!success) return false;
    // minimum raise is the difference between the current bet and the new bet but at least the big blind
    if (amount > this->data.betRoundData.currentBet) this->data.betRoundData.minimumRaise = amount - this->data.betRoundData.currentBet;
    if (this->data.betRoundData.minimumRaise < this->data.roundData.bigBlind) this->data.betRoundData.minimumRaise = this->data.roundData.bigBlind;
    this->data.betRoundData.currentBet = amount;
    this->data.roundData.pot += addAmount;
    this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] = amount;
    this->data.nextPlayer();
    return true;
}

OutEnum Game::playerOut(const char* reason) noexcept {
    // player is out of the game, log the reason and set the player out
    PLOG_WARNING << this->getPlayerInfo() << " " << reason << " and is out";
    this->data.gameData.numNonOutPlayers--;
    this->data.roundData.numActivePlayers--;
    this->data.gameData.playerOut[this->data.betRoundData.playerPos] = true;
    this->data.gameData.playerChips[this->data.betRoundData.playerPos] = 0;
    this->data.nextPlayer();

    return this->getOutEnum();
}

OutEnum Game::playerFolded() noexcept {
    this->data.roundData.numActivePlayers--;
    this->data.roundData.playerFolded[this->data.betRoundData.playerPos] = true;
    this->data.nextPlayer();
    // if only one player is left, he wins the pot
    return this->getOutEnum();
}

OutEnum Game::getOutEnum() const noexcept {
    if (this->data.gameData.numNonOutPlayers == 1) {
        // only one player is left in the game, he wins the game
        return OutEnum::GAME_WON;
    } else if (this->data.roundData.numActivePlayers == 1) {
        // only one player is left in the round, he wins the pot
        return OutEnum::ROUND_WON;
    } else {
        return OutEnum::ROUND_CONTINUE;
    }
}

void Game::preflop() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::PREFLOP;
    PLOG_DEBUG << "Starting PREFLOP bet round";
    this->data.roundData.result = this->betRound();
}

void Game::flop() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::FLOP;
    this->setupBetRound();
    PLOG_DEBUG << "Starting FLOP bet round";
    for (u_int8_t i = 0; i < 3; i++) {
        this->data.roundData.communityCards[i] = this->deck.draw();  // draw flop cards
    }
    this->data.roundData.result = this->betRound();
}

void Game::turn() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::TURN;
    this->setupBetRound();
    PLOG_DEBUG << "Starting TURN bet round";
    this->data.roundData.communityCards[3] = this->deck.draw();  // draw turn card
    this->data.roundData.result = this->betRound();
}

void Game::river() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::RIVER;
    this->setupBetRound();
    PLOG_DEBUG << "Starting RIVER bet round";
    this->data.roundData.communityCards[4] = this->deck.draw();  // draw river card
    this->data.roundData.result = this->betRound();
}
