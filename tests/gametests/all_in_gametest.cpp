#include <gtest/gtest.h>

#include "game_test.h"
#include "test_player/test_player.h"

TEST(AllInTest, AllIn1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{13, 1}, Card{13, 2}, Card{14, 1}, Card{14, 2}, Card{12, 1}, Card{12, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 30, 50},
                              .playerHands = {{Card{13, 1}, Card{13, 2}}, {Card{14, 1}, Card{14, 2}}, {Card{12, 1}, Card{12, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, true},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {1, 0, 0},
                                  .pot = 90,
                                  .playerChips = {990, 90, 0},
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

TEST(AllInTest, AllIn2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 50},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, true, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1},
                                  .pot = 150,
                                  .playerChips = {950, 950, 150},
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

TEST(AllInTest, AllIn3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 50},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::RAISE, 100}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1},
                                  .pot = 140,
                                  .playerChips = {950, 0, 140},
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

TEST(AllInTest, AllIn4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 45, 50},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1},
                                  .pot = 135,
                                  .playerChips = {950, 0, 145},
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

TEST(AllInTest, AllIn5) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}, Card{13, 1}, Card{13, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 45, 50},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{14, 1}, Card{14, 2}}, {Card{13, 1}, Card{13, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .gameWins = {0, 1, 0},
                                  .pot = 135,
                                  .playerChips = {950, 135, 10},
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

TEST(AllInTest, AllIn6) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 45, 50, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 180,
                                  .playerChips = {950, 0, 195, 950},
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

TEST(AllInTest, AllIn7) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}, Card{13, 1}, Card{13, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 45, 50, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Card{13, 1}, Card{13, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 4,
                                  .numActivePlayers = 4,
                                  .gameWins = {0, 1, 0, 0},
                                  .pot = 180,
                                  .playerChips = {950, 180, 15, 950},
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

TEST(AllInTest, AllIn8) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}, Card{13, 1}, Card{13, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 45, 50, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 1}, Card{13, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, true, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .gameWins = {0, 1, 0, 0},
                                  .pot = 180,
                                  .playerChips = {950, 180, 0, 965},
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

TEST(AllInTest, AllIn9) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 45, 50, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::BET, 20}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 0, 1},
                                  .pot = 180,
                                  .playerChips = {950, 0, 0, 1145},
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

TEST(AllInTest, AllIn10) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 25, 26, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 46}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 26}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, false},
                                  .foldedPlayers = {false, false, false, true},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 100,
                                  .playerChips = {1117, 0, 0, 934},
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

TEST(AllInTest, AllIn11) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 25, 26, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::BET, 100}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 25}}},
                              .resultData{
                                  .outPlayers = {true, false, false, true},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 1, 0, 0},
                                  .pot = 90,
                                  .playerChips = {0, 90, 1, 0},
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

TEST(AllInTest, AllIn12) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 25, 26, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false, false},
                                  .foldedPlayers = {false, false, false, true},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 100,
                                  .playerChips = {974, 0, 103, 974},
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

TEST(AllInTest, AllIn13) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 25, 26, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 26}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false, false},
                                  .foldedPlayers = {false, false, false, true},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 100,
                                  .playerChips = {974, 0, 103, 974},
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

TEST(AllInTest, AllIn14) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 25, 26, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 25}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {true, true, false, false},
                                  .foldedPlayers = {false, false, false, true},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 100,
                                  .playerChips = {0, 0, 103, 974},
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

TEST(AllInTest, AllIn15) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 26, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 46}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 26}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false},
                                  .foldedPlayers = {false, false, true},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0},
                                  .pot = 78,
                                  .playerChips = {1092, 0, 934},
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

TEST(AllInTest, AllIn16) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{
            .numPlayers = 3,
            .smallBlind = 10,
            .playerChips = {1000, 26, 1000},
            .playerHands = {{Card{14, 1}, Card{14, 2}},
                            {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                            {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::BET, 100}}, {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}, {Action{Actions::CALL, 0}, Action{Actions::RAISE, 25}}},
            .resultData{
                .outPlayers = {true, false, true},
                .foldedPlayers = {false, false, false},
                .nonOutPlayers = 1,
                .numActivePlayers = 1,
                .gameWins = {0, 1, 0},
                .pot = 66,
                .playerChips = {0, 66, 0},
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

TEST(AllInTest, AllIn17) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 26, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false},
                                  .foldedPlayers = {false, false, true},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0},
                                  .pot = 78,
                                  .playerChips = {1052, 0, 974},
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

TEST(AllInTest, AllIn18) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 26, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 26}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, true},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 1, 0},
                                  .pot = 78,
                                  .playerChips = {974, 78, 974},
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

TEST(AllInTest, AllIn19) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 26, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::RAISE, 25}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {true, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 1, 0},
                                  .pot = 72,
                                  .playerChips = {0, 72, 974},
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

TEST(AllInTest, AllIn20) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 26},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, true},
                                  .foldedPlayers = {false, true, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0},
                                  .pot = 78,
                                  .playerChips = {1052, 974, 0},
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

TEST(AllInTest, AllIn21) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 26},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 100}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0},
                                  .pot = 72,
                                  .playerChips = {1046, 0, 0},
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

TEST(AllInTest, AllIn22) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 26, 45, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 50}}},
                              .resultData{
                                  .outPlayers = {false, true, false, false},
                                  .foldedPlayers = {true, false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 98,
                                  .playerChips = {980, 0, 136, 955},
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

TEST(AllInTest, AllIn23) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 26, 45, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 49}}},
                              .resultData{
                                  .outPlayers = {false, true, false, true},
                                  .foldedPlayers = {true, false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 92,
                                  .playerChips = {980, 0, 111, 0},
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

TEST(AllInTest, AllIn24) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 25, 45, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 45}}},
                              .resultData{
                                  .outPlayers = {false, true, false, false},
                                  .foldedPlayers = {true, false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 95,
                                  .playerChips = {980, 0, 135, 955},
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

TEST(AllInTest, AllIn25) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {1000, 25, 45, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::RAISE, 44}}},
                              .resultData{
                                  .outPlayers = {false, true, false, true},
                                  .foldedPlayers = {true, false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 0, 1, 0},
                                  .pot = 90,
                                  .playerChips = {980, 0, 100, 0},
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

TEST(AllInTest, AllInBlind1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 10,
                              .smallBlind = 4,
                              .playerChips = {1000, 1000, 7, 1000, 1000, 1000, 1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 16}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false, false, false, false, false, false, false},
                                  .foldedPlayers = {true, true, false, true, false, true, true, true, true, true},
                                  .nonOutPlayers = 10,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                                  .pot = 25,
                                  .playerChips = {1000, 996, 25, 992, 994, 1000, 1000, 1000, 1000, 1000},
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

TEST(AllInTest, AllInBlind2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 10,
                              .smallBlind = 4,
                              .playerChips = {1000, 3, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 16}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false, false, false, false, false, false, false},
                                  .foldedPlayers = {true, false, true, true, false, true, true, true, true, true},
                                  .nonOutPlayers = 10,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                  .pot = 12,
                                  .playerChips = {1000, 12, 992, 992, 1007, 1000, 1000, 1000, 1000, 1000},
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

TEST(AllInTest, AllInBlind3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}, Card{13, 1}, Card{13, 2}};
        TestConfig testConfig{.numPlayers = 10,
                              .smallBlind = 4,
                              .playerChips = {1000, 3, 7, 1000, 1000, 1000, 1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Card{13, 1}, Card{13, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 16}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false, false, false, false, false, false, false},
                                  .foldedPlayers = {true, false, false, true, false, true, true, true, true, true},
                                  .nonOutPlayers = 10,
                                  .numActivePlayers = 3,
                                  .gameWins = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                  .pot = 12,
                                  .playerChips = {1000, 12, 12, 992, 994, 1000, 1000, 1000, 1000, 1000},
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

TEST(AllInTest, AllInBlind4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{13, 1}, Card{13, 2}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 10,
                              .smallBlind = 4,
                              .playerChips = {1000, 3, 7, 1000, 1000, 1000, 1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 1}, Card{13, 2}},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 16}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false, false, false, false, false, false, false, false},
                                  .foldedPlayers = {true, false, false, true, false, true, true, true, true, true},
                                  .nonOutPlayers = 9,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                                  .pot = 12,
                                  .playerChips = {1000, 0, 24, 992, 994, 1000, 1000, 1000, 1000, 1000},
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

TEST(AllInTest, AllInBlind5) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 10,
                              .smallBlind = 4,
                              .playerChips = {1000, 3, 7, 1000, 1000, 1000, 1000, 1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}},
                                                {Action{Actions::RAISE, 16}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}},
                                                {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, false, false, false, false, false, false, false},
                                  .foldedPlayers = {true, false, false, true, false, true, true, true, true, true},
                                  .nonOutPlayers = 8,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                  .pot = 12,
                                  .playerChips = {1000, 0, 0, 992, 1018, 1000, 1000, 1000, 1000, 1000},
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

TEST(AllInTest, AllInEqBlind1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {10, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {1, 0},
                                  .pot = 20,
                                  .playerChips = {20, 990},
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

TEST(AllInTest, AllInEqBlind2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 20},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 1},
                                  .pot = 40,
                                  .playerChips = {980, 40},
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

TEST(AllInTest, AllInEqBlind3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 20},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {true, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 1},
                                  .pot = 30,
                                  .playerChips = {0, 30},
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

TEST(AllInTest, AllInEqBlind4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 20},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::RAISE, 40}, Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {true, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 1},
                                  .pot = 30,
                                  .playerChips = {0, 30},
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

TEST(AllInTest, AllInEqBlind5) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 20},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {true, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 1},
                                  .pot = 30,
                                  .playerChips = {990, 30},
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

TEST(AllInTest, AllInEqBlind6) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {10, 20, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}}},
                              .resultData{
                                  .outPlayers = {true, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {0, 1, 0},
                                  .pot = 30,
                                  .playerChips = {0, 50, 980},
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

TEST(AllInTest, AllInEq1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {20, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {1, 0},
                                  .pot = 40,
                                  .playerChips = {40, 980},
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

TEST(AllInTest, AllInEq2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {40, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {1, 0},
                                  .pot = 80,
                                  .playerChips = {80, 960},
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

TEST(AllInTest, AllInEq3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {40, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, true},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0},
                                  .pot = 60,
                                  .playerChips = {60, 980},
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

TEST(AllInTest, AllInEq4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {40, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::ALL_IN, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0},
                                  .pot = 60,
                                  .playerChips = {60, 980},
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

TEST(AllInTest, AllInEq5) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {40, 1000},
                              .playerHands = {{Card{14, 1}, Card{14, 2}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}, Action{Actions::FOLD, 0}}, {Action{Actions::RAISE, 100}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0},
                                  .pot = 60,
                                  .playerChips = {60, 980},
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

TEST(AllInTest, AllInEq6) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {50, 10, 20, 40},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, true},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 40,
                                  .playerChips = {120, 0, 0, 0},
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

TEST(AllInTest, AllInEq7) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {40, 10, 20, 40},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, true},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 40,
                                  .playerChips = {110, 0, 0, 0},
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

TEST(AllInTest, AllInEq8) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {50, 10, 20, 40},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {true, true, true, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 0, 0, 1},
                                  .pot = 30,
                                  .playerChips = {0, 0, 0, 70},
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

TEST(AllInTest, AllInEq9) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {30, 10, 20, 40},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{14, 1}, Card{14, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {true, true, true, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {0, 0, 0, 1},
                                  .pot = 40,
                                  .playerChips = {0, 0, 0, 100},
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

TEST(AllInTest, AllInEq10) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {30, 10, 20, 40},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 40,
                                  .playerChips = {90, 0, 0, 10},
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

TEST(AllInTest, AllInEq11) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {30, 10, 20, 30},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::ALL_IN, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, true},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 40,
                                  .playerChips = {90, 0, 0, 0},
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

TEST(AllInTest, AllInEq12) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}, Card{14, 1}, Card{14, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {30, 10, 20, 30},
                              .playerHands = {{Card{14, 1}, Card{14, 2}},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 0}, Card{13, 3}, Card{13, 0}, Card{2, 1}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::CALL, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, true, true},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 1,
                                  .numActivePlayers = 1,
                                  .gameWins = {1, 0, 0, 0},
                                  .pot = 40,
                                  .playerChips = {80, 0, 0, 0},
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

TEST(AllInTest, AllInSplit1) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}, Card{13, 3}, Card{13, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {20, 10, 20, 30},
                              .playerHands = {{Card{13, 3}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 2}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, true, false, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .gameWins = {1, 0, 1, 0},
                                  .pot = 40,
                                  .playerChips = {35, 0, 35, 10},
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

TEST(AllInTest, AllInSplit2) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}, Card{13, 3}, Card{13, 2}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {20, 10, 20, 30},
                              .playerHands = {{Card{13, 3}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 2}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, true, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .gameWins = {1, 1, 0, 0},
                                  .pot = 40,
                                  .playerChips = {50, 20, 0, 10},
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

TEST(AllInTest, AllInSplit3) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}, Card{13, 3}, Card{13, 2}, Card{13, 1}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {20, 10, 20, 30},
                              .playerHands = {{Card{13, 3}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 2}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 1}, Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, true, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .gameWins = {1, 1, 0, 1},
                                  .pot = 40,
                                  .playerChips = {28, 13, 0, 38},
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

TEST(AllInTest, AllInSplit4) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}, Card{13, 3}, Card{13, 2}, Card{13, 0}, Card{13, 1}};
        TestConfig testConfig{.numPlayers = 4,
                              .smallBlind = 10,
                              .playerChips = {20, 10, 20, 30},
                              .playerHands = {{Card{13, 3}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 2}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 0}, Deck::getRandomCardExceptAdd(drawnCards)},
                                              {Card{13, 1}, Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{14, 2}, Card{14, 1}, Card{14, 0}, Card{12, 3}},
                              .playerActions = {{Action{Actions::ALL_IN, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::FOLD, 0}}, {Action{Actions::CALL, 0}, Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false, false},
                                  .foldedPlayers = {false, false, false, false},
                                  .nonOutPlayers = 4,
                                  .numActivePlayers = 4,
                                  .gameWins = {1, 1, 1, 1},
                                  .pot = 40,
                                  .playerChips = {20, 10, 20, 30},
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
