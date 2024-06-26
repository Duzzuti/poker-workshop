#include <gtest/gtest.h>

#include "game_test.h"
#include "test_player/test_player.h"

TEST(EdgeCaseTest, TwoPlayers1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, false},
                .foldedPlayers = {true, false},
                .nonOutPlayers = 2,
                .numActivePlayers = 1,
                .gameWins = {0, 1},
                .pot = 30,
                .playerChips = {990, 1010},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, TwoPlayers2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, false},
                .foldedPlayers = {false, true},
                .nonOutPlayers = 2,
                .numActivePlayers = 1,
                .gameWins = {1, 0},
                .pot = 40,
                .playerChips = {1020, 980},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, TwoPlayers3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, false},
                .foldedPlayers = {false, true},
                .nonOutPlayers = 2,
                .numActivePlayers = 1,
                .gameWins = {1, 0},
                .pot = 40,
                .playerChips = {1020, 980},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, TwoPlayers4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::FOLD, 0}},
                              {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, false},
                .foldedPlayers = {false, true},
                .nonOutPlayers = 2,
                .numActivePlayers = 1,
                .gameWins = {1, 0},
                .pot = 40,
                .playerChips = {1020, 980},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, ThreePlayers1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 3,
            .smallBlind = 10,
            .playerChips = {1000, 1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                            {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                            {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}}, {Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}}, {Action{Actions::RAISE, 80}, Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, false, false},
                .foldedPlayers = {true, true, false},
                .nonOutPlayers = 3,
                .numActivePlayers = 1,
                .gameWins = {0, 0, 1},
                .pot = 180,
                .playerChips = {960, 940, 1100},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, ThreePlayers2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, true, true},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0},
                                  .pot = 180,
                                  .playerChips = {1100, 960, 940},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, ThreePlayers3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::BET, 20}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, true, true},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0},
                                  .pot = 180,
                                  .playerChips = {1100, 960, 940},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, ThreePlayers4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::BET, 20}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, true, true},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0},
                                  .pot = 180,
                                  .playerChips = {1100, 960, 940},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, FourPlayers1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 80}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 100}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false},
                                  .foldedPlayers = {true, true, false, true},
                                  .nonOutPlayers = 4,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 280,
                                  .playerChips = {940, 920, 1180, 960},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, FourPlayers2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 80}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false},
                                  .foldedPlayers = {false, true, true, true},
                                  .nonOutPlayers = 4,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 280,
                                  .playerChips = {1180, 960, 940, 920},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, FourPlayers3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 80}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::BET, 20}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false},
                                  .foldedPlayers = {false, true, true, true},
                                  .nonOutPlayers = 4,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 280,
                                  .playerChips = {1180, 960, 940, 920},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, FourPlayers4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                                                 Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 80}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::BET, 20}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 60}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false},
                                  .foldedPlayers = {false, true, true, true},
                                  .nonOutPlayers = 4,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 280,
                                  .playerChips = {1180, 960, 940, 920},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, RaiseLastChip) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::RAISE, 1000}}, {Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {true, false},
                .foldedPlayers = {false, false},
                .nonOutPlayers = 1,
                .numActivePlayers = 1,
                .gameWins = {0, 1},
                .pot = 30,
                .playerChips = {0, 1010},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, RaiseLastChipClose) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::RAISE, 999}}, {Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, false},
                .foldedPlayers = {false, true},
                .nonOutPlayers = 2,
                .numActivePlayers = 1,
                .gameWins = {1, 0},
                .pot = 1019,
                .playerChips = {1020, 980},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, BetLastChip) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::BET, 980}}},
            .resultData{
                .outPlayers = {false, true},
                .foldedPlayers = {false, false},
                .nonOutPlayers = 1,
                .numActivePlayers = 1,
                .gameWins = {1, 0},
                .pot = 40,
                .playerChips = {1020, 0},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, BetLastChipClose) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::BET, 979}}},
            .resultData{
                .outPlayers = {false, false},
                .foldedPlayers = {true, false},
                .nonOutPlayers = 2,
                .numActivePlayers = 1,
                .gameWins = {0, 1},
                .pot = 1019,
                .playerChips = {980, 1020},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, CallLastChip) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {10000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::BET, 980}, Action{Actions::FOLD, 0}},
                              {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, true},
                .foldedPlayers = {false, false},
                .nonOutPlayers = 1,
                .numActivePlayers = 1,
                .gameWins = {1, 0},
                .pot = 1020,
                .playerChips = {10020, 0},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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

TEST(EdgeCaseTest, CallLastChipClose) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {};
        TestConfig testConfig{
            .numPlayers = 2,
            .smallBlind = 10,
            .playerChips = {10000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards),
                               Deck::getRandomCardExceptAdd(drawnCards)},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::BET, 979}, Action{Actions::FOLD, 0}},
                              {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::FOLD, 0}}},
            .resultData{
                .outPlayers = {false, false},
                .foldedPlayers = {true, false},
                .nonOutPlayers = 2,
                .numActivePlayers = 1,
                .gameWins = {1, 0},
                .pot = 1998,
                .playerChips = {9001, 1999},
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
            EXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);
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
