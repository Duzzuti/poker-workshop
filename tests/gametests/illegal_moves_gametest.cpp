#include <gtest/gtest.h>

#include "game_test.h"
#include "test_player/test_player.h"

TEST(IllegalMoves, Check) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 6,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CHECK, 0}},
                                                {Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::BET, 40}, Action{Actions::BET, 80}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::RAISE, 160}}},
                              .resultData{
                                  .outPlayers = {true, true, true, true, true, false},
                                  .foldedPlayers = {false, false, false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .winners = {0, 0, 0, 0, 0, 1},
                                  .pot = 520,
                                  .playerChips = {920, 980, 980, 1000, 840, 1280},
                                  .betRoundState = BetRoundState::RIVER,
                              }};
        // game should only last one round and not shuffle players or deck
        Config config{1, testConfig.numPlayers, testConfig.playerChips, testConfig.smallBlind, 0, false, false, 1};
        GameTest game(config);
        // build the deck for the game
        game.buildDeck(testConfig.playerHands, testConfig.numPlayers, testConfig.communityCards);

        // generate players and their actions for the game
        for (int i = 0; i < testConfig.numPlayers; i++) {
            std::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);
            if (testConfig.playerActions[i].size() > 0) testPlayer->setActions(&testConfig.playerActions[i][0], testConfig.playerActions[i].size());
            game.getPlayers()[i] = std::move(testPlayer);
        }

        // run the game without setting new players
        game.run(false);

        // check if the game has run as expected
        Data data = game.getData();
        EXPECT_EQ(testConfig.numPlayers, data.numPlayers);

        EXPECT_EQ(testConfig.resultData.nonOutPlayers, data.gameData.numNonOutPlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.outPlayers[i], data.gameData.playerOut[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.playerChips[i], data.gameData.playerChips[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.winners[i], data.gameData.winners[i]);
        }

        EXPECT_EQ(0, data.roundData.addBlind);
        EXPECT_EQ(testConfig.resultData.betRoundState, data.roundData.betRoundState);
        EXPECT_EQ(testConfig.smallBlind * 2, data.roundData.bigBlind);
        EXPECT_EQ(testConfig.smallBlind, data.roundData.smallBlind);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 1 : 2, data.roundData.bigBlindPos);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 0 : 1, data.roundData.smallBlindPos);
        EXPECT_EQ(0, data.roundData.dealerPos);
        u_int8_t communityCardsCount = data.roundData.betRoundState == BetRoundState::PREFLOP ? 0
                                       : data.roundData.betRoundState == BetRoundState::FLOP  ? 3
                                       : data.roundData.betRoundState == BetRoundState::TURN  ? 4
                                                                                              : 5;
        for (int i = 0; i < communityCardsCount; i++) {
            EXPECT_EQ(testConfig.communityCards[i], data.roundData.communityCards[i]);
        }
        EXPECT_EQ(testConfig.resultData.numActivePlayers, data.roundData.numActivePlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.foldedPlayers[i], data.roundData.playerFolded[i]);
        }
        EXPECT_EQ(testConfig.resultData.pot, data.roundData.pot);
        EXPECT_EQ(OutEnum::GAME_WON, data.roundData.result);
    }
}

TEST(IllegalMoves, Call) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 100},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 91}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::CALL, 0}}},
                              .resultData{
                                  .outPlayers = {true, false, true},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .winners = {0, 1, 0},
                                  .pot = 262,
                                  .playerChips = {889, 1151, 60},
                                  .betRoundState = BetRoundState::TURN,
                              }};
        // game should only last one round and not shuffle players or deck
        Config config{1, testConfig.numPlayers, testConfig.playerChips, testConfig.smallBlind, 0, false, false, 1};
        GameTest game(config);
        // build the deck for the game
        game.buildDeck(testConfig.playerHands, testConfig.numPlayers, testConfig.communityCards);

        // generate players and their actions for the game
        for (int i = 0; i < testConfig.numPlayers; i++) {
            std::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);
            if (testConfig.playerActions[i].size() > 0) testPlayer->setActions(&testConfig.playerActions[i][0], testConfig.playerActions[i].size());
            game.getPlayers()[i] = std::move(testPlayer);
        }

        // run the game without setting new players
        game.run(false);

        // check if the game has run as expected
        Data data = game.getData();
        EXPECT_EQ(testConfig.numPlayers, data.numPlayers);

        EXPECT_EQ(testConfig.resultData.nonOutPlayers, data.gameData.numNonOutPlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.outPlayers[i], data.gameData.playerOut[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.playerChips[i], data.gameData.playerChips[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.winners[i], data.gameData.winners[i]);
        }

        EXPECT_EQ(0, data.roundData.addBlind);
        EXPECT_EQ(testConfig.resultData.betRoundState, data.roundData.betRoundState);
        EXPECT_EQ(testConfig.smallBlind * 2, data.roundData.bigBlind);
        EXPECT_EQ(testConfig.smallBlind, data.roundData.smallBlind);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 1 : 2, data.roundData.bigBlindPos);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 0 : 1, data.roundData.smallBlindPos);
        EXPECT_EQ(0, data.roundData.dealerPos);
        u_int8_t communityCardsCount = data.roundData.betRoundState == BetRoundState::PREFLOP ? 0
                                       : data.roundData.betRoundState == BetRoundState::FLOP  ? 3
                                       : data.roundData.betRoundState == BetRoundState::TURN  ? 4
                                                                                              : 5;
        for (int i = 0; i < communityCardsCount; i++) {
            EXPECT_EQ(testConfig.communityCards[i], data.roundData.communityCards[i]);
        }
        EXPECT_EQ(testConfig.resultData.numActivePlayers, data.roundData.numActivePlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.foldedPlayers[i], data.roundData.playerFolded[i]);
        }
        EXPECT_EQ(testConfig.resultData.pot, data.roundData.pot);
        EXPECT_EQ(OutEnum::GAME_WON, data.roundData.result);
    }
}

TEST(IllegalMoves, Raise) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 6,
                              .smallBlind = 10,
                              .playerChips = {100, 1000, 1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::RAISE, 70}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 60}, Action{Actions::CALL, 0},
                                                 Action{Actions::CHECK, 0}, Action{Actions::RAISE, 60}},
                                                {Action{Actions::RAISE, 40}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 100}, Action{Actions::BET, 40}},
                                                {Action{Actions::RAISE, 30}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::RAISE, 40}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::RAISE, 130}}},
                              .resultData{
                                  .outPlayers = {true, true, false, true, true, true},
                                  .foldedPlayers = {false, false, false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .winners = {0, 0, 1, 0, 0, 0},
                                  .pot = 460,
                                  .playerChips = {60, 860, 1280, 1000, 960, 940},
                                  .betRoundState = BetRoundState::TURN,
                              }};
        // game should only last one round and not shuffle players or deck
        Config config{1, testConfig.numPlayers, testConfig.playerChips, testConfig.smallBlind, 0, false, false, 1};
        GameTest game(config);
        // build the deck for the game
        game.buildDeck(testConfig.playerHands, testConfig.numPlayers, testConfig.communityCards);

        // generate players and their actions for the game
        for (int i = 0; i < testConfig.numPlayers; i++) {
            std::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);
            if (testConfig.playerActions[i].size() > 0) testPlayer->setActions(&testConfig.playerActions[i][0], testConfig.playerActions[i].size());
            game.getPlayers()[i] = std::move(testPlayer);
        }

        // run the game without setting new players
        game.run(false);

        // check if the game has run as expected
        Data data = game.getData();
        EXPECT_EQ(testConfig.numPlayers, data.numPlayers);

        EXPECT_EQ(testConfig.resultData.nonOutPlayers, data.gameData.numNonOutPlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.outPlayers[i], data.gameData.playerOut[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.playerChips[i], data.gameData.playerChips[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.winners[i], data.gameData.winners[i]);
        }

        EXPECT_EQ(0, data.roundData.addBlind);
        EXPECT_EQ(testConfig.resultData.betRoundState, data.roundData.betRoundState);
        EXPECT_EQ(testConfig.smallBlind * 2, data.roundData.bigBlind);
        EXPECT_EQ(testConfig.smallBlind, data.roundData.smallBlind);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 1 : 2, data.roundData.bigBlindPos);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 0 : 1, data.roundData.smallBlindPos);
        EXPECT_EQ(0, data.roundData.dealerPos);
        u_int8_t communityCardsCount = data.roundData.betRoundState == BetRoundState::PREFLOP ? 0
                                       : data.roundData.betRoundState == BetRoundState::FLOP  ? 3
                                       : data.roundData.betRoundState == BetRoundState::TURN  ? 4
                                                                                              : 5;
        for (int i = 0; i < communityCardsCount; i++) {
            EXPECT_EQ(testConfig.communityCards[i], data.roundData.communityCards[i]);
        }
        EXPECT_EQ(testConfig.resultData.numActivePlayers, data.roundData.numActivePlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.foldedPlayers[i], data.roundData.playerFolded[i]);
        }
        EXPECT_EQ(testConfig.resultData.pot, data.roundData.pot);
        EXPECT_EQ(OutEnum::GAME_WON, data.roundData.result);
    }
}

TEST(IllegalMoves, Bet) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 4,
            .smallBlind = 10,
            .playerChips = {100, 1000, 1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                            {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                            {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                            {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::BET, 100}}, {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}}, {Action{Actions::BET, 20}}, {Action{Actions::BET, 100}}},
            .resultData{
                .outPlayers = {true, false, true, true},
                .foldedPlayers = {false, false, false, false},
                .nonOutPlayers = 1,
                .numActivePlayers = 1,
                .winners = {0, 1, 0, 0},
                .pot = 60,
                .playerChips = {80, 1040, 980, 1000},
                .betRoundState = BetRoundState::FLOP,
            }};
        // game should only last one round and not shuffle players or deck
        Config config{1, testConfig.numPlayers, testConfig.playerChips, testConfig.smallBlind, 0, false, false, 1};
        GameTest game(config);
        // build the deck for the game
        game.buildDeck(testConfig.playerHands, testConfig.numPlayers, testConfig.communityCards);

        // generate players and their actions for the game
        for (int i = 0; i < testConfig.numPlayers; i++) {
            std::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);
            if (testConfig.playerActions[i].size() > 0) testPlayer->setActions(&testConfig.playerActions[i][0], testConfig.playerActions[i].size());
            game.getPlayers()[i] = std::move(testPlayer);
        }

        // run the game without setting new players
        game.run(false);

        // check if the game has run as expected
        Data data = game.getData();
        EXPECT_EQ(testConfig.numPlayers, data.numPlayers);

        EXPECT_EQ(testConfig.resultData.nonOutPlayers, data.gameData.numNonOutPlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.outPlayers[i], data.gameData.playerOut[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.playerChips[i], data.gameData.playerChips[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.winners[i], data.gameData.winners[i]);
        }

        EXPECT_EQ(0, data.roundData.addBlind);
        EXPECT_EQ(testConfig.resultData.betRoundState, data.roundData.betRoundState);
        EXPECT_EQ(testConfig.smallBlind * 2, data.roundData.bigBlind);
        EXPECT_EQ(testConfig.smallBlind, data.roundData.smallBlind);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 1 : 2, data.roundData.bigBlindPos);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 0 : 1, data.roundData.smallBlindPos);
        EXPECT_EQ(0, data.roundData.dealerPos);
        u_int8_t communityCardsCount = data.roundData.betRoundState == BetRoundState::PREFLOP ? 0
                                       : data.roundData.betRoundState == BetRoundState::FLOP  ? 3
                                       : data.roundData.betRoundState == BetRoundState::TURN  ? 4
                                                                                              : 5;
        for (int i = 0; i < communityCardsCount; i++) {
            EXPECT_EQ(testConfig.communityCards[i], data.roundData.communityCards[i]);
        }
        EXPECT_EQ(testConfig.resultData.numActivePlayers, data.roundData.numActivePlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.foldedPlayers[i], data.roundData.playerFolded[i]);
        }
        EXPECT_EQ(testConfig.resultData.pot, data.roundData.pot);
        EXPECT_EQ(OutEnum::GAME_WON, data.roundData.result);
    }
}

TEST(IllegalMoves, Fold) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {100, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}}, {Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, true},
                .foldedPlayers = {false, false},
                .nonOutPlayers = 1,
                .numActivePlayers = 1,
                .winners = {1, 0},
                .pot = 40,
                .playerChips = {120, 980},
                .betRoundState = BetRoundState::PREFLOP,
            }};
        // game should only last one round and not shuffle players or deck
        Config config{1, testConfig.numPlayers, testConfig.playerChips, testConfig.smallBlind, 0, false, false, 1};
        GameTest game(config);
        // build the deck for the game
        game.buildDeck(testConfig.playerHands, testConfig.numPlayers, testConfig.communityCards);

        // generate players and their actions for the game
        for (int i = 0; i < testConfig.numPlayers; i++) {
            std::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);
            if (testConfig.playerActions[i].size() > 0) testPlayer->setActions(&testConfig.playerActions[i][0], testConfig.playerActions[i].size());
            game.getPlayers()[i] = std::move(testPlayer);
        }

        // run the game without setting new players
        game.run(false);

        // check if the game has run as expected
        Data data = game.getData();
        EXPECT_EQ(testConfig.numPlayers, data.numPlayers);

        EXPECT_EQ(testConfig.resultData.nonOutPlayers, data.gameData.numNonOutPlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.outPlayers[i], data.gameData.playerOut[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.playerChips[i], data.gameData.playerChips[i]);
        }
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.winners[i], data.gameData.winners[i]);
        }

        EXPECT_EQ(0, data.roundData.addBlind);
        EXPECT_EQ(testConfig.resultData.betRoundState, data.roundData.betRoundState);
        EXPECT_EQ(testConfig.smallBlind * 2, data.roundData.bigBlind);
        EXPECT_EQ(testConfig.smallBlind, data.roundData.smallBlind);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 1 : 2, data.roundData.bigBlindPos);
        EXPECT_EQ(testConfig.numPlayers == 2 ? 0 : 1, data.roundData.smallBlindPos);
        EXPECT_EQ(0, data.roundData.dealerPos);
        u_int8_t communityCardsCount = data.roundData.betRoundState == BetRoundState::PREFLOP ? 0
                                       : data.roundData.betRoundState == BetRoundState::FLOP  ? 3
                                       : data.roundData.betRoundState == BetRoundState::TURN  ? 4
                                                                                              : 5;
        for (int i = 0; i < communityCardsCount; i++) {
            EXPECT_EQ(testConfig.communityCards[i], data.roundData.communityCards[i]);
        }
        EXPECT_EQ(testConfig.resultData.numActivePlayers, data.roundData.numActivePlayers);
        for (int i = 0; i < testConfig.numPlayers; i++) {
            EXPECT_EQ(testConfig.resultData.foldedPlayers[i], data.roundData.playerFolded[i]);
        }
        EXPECT_EQ(testConfig.resultData.pot, data.roundData.pot);
        EXPECT_EQ(OutEnum::GAME_WON, data.roundData.result);
    }
}
