#include <gtest/gtest.h>

#include "game_test.h"
#include "test_player/test_player.h"

TEST(ShowdownTest, RoyalFlushVsRoyalFlush) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 1}, Card{13, 1}, Card{12, 1}, Card{11, 1}, Card{10, 1}, Card{14, 2}, Card{13, 2}, Card{9, 1}, Card{8, 1}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{14, 2}, Card{13, 2}}, {Card{9, 1}, Card{8, 1}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 1}, Card{13, 1}, Card{12, 1}, Card{11, 1}, Card{10, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {1, 1, 1},
                                  .pot = 60,
                                  .playerChips = {1000, 1000, 1000},
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

TEST(ShowdownTest, RoyalFlushVsStraightFlush) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{2, 1}, Card{13, 1}, Card{12, 1}, Card{11, 1}, Card{10, 1}, Card{14, 1}, Card{8, 3}, Card{9, 1}, Card{5, 3}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{14, 1}, Card{8, 3}}, {Card{9, 1}, Card{5, 3}}, {Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{2, 1}, Card{13, 1}, Card{12, 1}, Card{11, 1}, Card{10, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {1, 0, 0},
                                  .pot = 60,
                                  .playerChips = {1040, 980, 980},
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

TEST(ShowdownTest, StraightFlushVsStraightFlushClose) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{2, 1}, Card{11, 1}, Card{10, 1}, Card{9, 1}, Card{8, 1}, Card{12, 1}, Card{14, 1}, Card{7, 1}, Card{13, 1}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{12, 1}, Deck::getRandomCardExceptAdd(drawnCards)}, {Card{14, 1}, Card{7, 1}}, {Card{13, 1}, Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{2, 1}, Card{11, 1}, Card{10, 1}, Card{9, 1}, Card{8, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {1, 0, 0},
                                  .pot = 60,
                                  .playerChips = {1040, 980, 980},
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

TEST(ShowdownTest, StraightFlushVsStraightFlushSplit) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{7, 1}, Card{11, 1}, Card{10, 1}, Card{9, 1}, Card{8, 1}, Card{14, 1}, Card{8, 3}, Card{13, 1}, Card{14, 3}, Card{6, 1}, Card{5, 1}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{14, 1}, Card{8, 3}}, {Card{13, 1}, Card{14, 3}}, {Card{6, 1}, Card{5, 1}}},
                              .drawnCards = {},
                              .communityCards = {Card{7, 1}, Card{11, 1}, Card{10, 1}, Card{9, 1}, Card{8, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {1, 1, 1},
                                  .pot = 60,
                                  .playerChips = {1000, 1000, 1000},
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

TEST(ShowdownTest, FullHouseVsFourOfAKind) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{8, 2}, Card{8, 0}, Card{3, 0}, Card{3, 3}, Card{4, 1}, Card{3, 1}, Card{5, 3}, Card{8, 1}, Card{8, 3}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{3, 1}, Card{5, 3}}, {Card{8, 1}, Card{8, 3}}},
                              .drawnCards = {},
                              .communityCards = {Card{8, 2}, Card{8, 0}, Card{3, 0}, Card{3, 3}, Card{4, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {0, 0, 1},
                                  .pot = 60,
                                  .playerChips = {980, 980, 1040},
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

TEST(ShowdownTest, FourOfAKindVsFourOfAKind) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{8, 2}, Card{8, 0}, Card{3, 0}, Card{3, 3}, Card{4, 1}, Card{3, 1}, Card{3, 2}, Card{8, 1}, Card{8, 3}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{3, 1}, Card{3, 2}}, {Card{8, 1}, Card{8, 3}}},
                              .drawnCards = {},
                              .communityCards = {Card{8, 2}, Card{8, 0}, Card{3, 0}, Card{3, 3}, Card{4, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {0, 0, 1},
                                  .pot = 60,
                                  .playerChips = {980, 980, 1040},
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

TEST(ShowdownTest, FourOfAKindVsFourOfAKindClose) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{8, 2}, Card{8, 0}, Card{8, 3}, Card{8, 1}, Card{4, 1}, Card{2, 1}, Card{3, 1}, Card{13, 1}, Card{7, 1}, Card{14, 3}, Card{2, 3}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{2, 1}, Card{3, 1}}, {Card{13, 1}, Card{7, 1}}, {Card{14, 3}, Card{2, 3}}},
                              .drawnCards = {},
                              .communityCards = {Card{8, 2}, Card{8, 0}, Card{8, 3}, Card{8, 1}, Card{4, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {0, 0, 1},
                                  .pot = 60,
                                  .playerChips = {980, 980, 1040},
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

TEST(ShowdownTest, FourOfAKindVsFourOfAKindSplit) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{8, 2}, Card{8, 0}, Card{8, 3}, Card{8, 1}, Card{4, 1}, Card{2, 1}, Card{3, 1}, Card{13, 1}, Card{14, 1}, Card{14, 3}, Card{2, 3}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{2, 1}, Card{3, 1}}, {Card{13, 1}, Card{14, 1}}, {Card{14, 3}, Card{2, 3}}},
                              .drawnCards = {},
                              .communityCards = {Card{8, 2}, Card{8, 0}, Card{8, 3}, Card{8, 1}, Card{4, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {0, 1, 1},
                                  .pot = 60,
                                  .playerChips = {980, 1010, 1010},
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

TEST(ShowdownTest, FlushVsStraight) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{3, 2}, Card{4, 2}, Card{10, 2}, Card{5, 0}, Card{6, 0}, Card{13, 2}, Card{14, 2}, Card{7, 1}};
        TestConfig testConfig{
            .numPlayers = 3,
            .smallBlind = 10,
            .playerChips = {1000, 1000, 1000},
            .playerHands = {{Deck::getRandomCardExceptAdd(drawnCards), Deck::getRandomCardExceptAdd(drawnCards)}, {Card{13, 2}, Card{14, 2}}, {Card{7, 1}, Deck::getRandomCardExceptAdd(drawnCards)}},
            .drawnCards = {},
            .communityCards = {Card{3, 2}, Card{4, 2}, Card{10, 2}, Card{5, 0}, Card{6, 0}},
            .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                              {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                              {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
            .resultData{
                .outPlayers = {false, false, false},
                .foldedPlayers = {false, false, false},
                .nonOutPlayers = 3,
                .numActivePlayers = 3,
                .winners = {0, 1, 0},
                .pot = 60,
                .playerChips = {980, 1040, 980},
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

TEST(ShowdownTest, StraightVsStraightClose) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{3, 2}, Card{4, 2}, Card{10, 2}, Card{5, 0}, Card{6, 0}, Card{2, 1}, Card{3, 1}, Card{2, 2}, Card{13, 3}, Card{7, 1}, Card{14, 0}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{2, 1}, Card{3, 1}}, {Card{2, 2}, Card{13, 3}}, {Card{7, 1}, Card{14, 0}}},
                              .drawnCards = {},
                              .communityCards = {Card{3, 2}, Card{4, 2}, Card{10, 2}, Card{5, 0}, Card{6, 0}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {0, 0, 1},
                                  .pot = 60,
                                  .playerChips = {980, 980, 1040},
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

TEST(ShowdownTest, StraightVsStraightSplit) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{3, 2}, Card{4, 2}, Card{10, 2}, Card{5, 0}, Card{6, 0}, Card{7, 3}, Card{8, 0}, Card{7, 2}, Card{8, 1}, Card{7, 1}, Card{14, 0}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000, 1000},
                              .playerHands = {{Card{7, 3}, Card{8, 0}}, {Card{7, 2}, Card{8, 1}}, {Card{7, 1}, Card{14, 0}}},
                              .drawnCards = {},
                              .communityCards = {Card{3, 2}, Card{4, 2}, Card{10, 2}, Card{5, 0}, Card{6, 0}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, false},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 3,
                                  .winners = {1, 1, 0},
                                  .pot = 60,
                                  .playerChips = {1010, 1010, 980},
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

TEST(ShowdownTest, PairVsHighCard) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{2, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}, Card{11, 1}, Card{8, 1}, Card{7, 1}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000},
                              .playerHands = {{Card{11, 1}, Card{8, 1}}, {Card{7, 1}, Deck::getRandomCardExceptAdd(drawnCards)}},
                              .drawnCards = {},
                              .communityCards = {Card{2, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .winners = {0, 1},
                                  .pot = 40,
                                  .playerChips = {980, 1020},
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

TEST(ShowdownTest, PairVsPair) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{2, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}, Card{11, 1}, Card{3, 1}, Card{7, 1}, Card{5, 3}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000},
                              .playerHands = {{Card{11, 1}, Card{3, 1}}, {Card{7, 1}, Card{5, 3}}},
                              .drawnCards = {},
                              .communityCards = {Card{2, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .winners = {0, 1},
                                  .pot = 40,
                                  .playerChips = {980, 1020},
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

TEST(ShowdownTest, PairVsPairClose) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{2, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}, Card{11, 1}, Card{7, 3}, Card{7, 1}, Card{5, 3}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000},
                              .playerHands = {{Card{11, 1}, Card{7, 3}}, {Card{7, 1}, Card{5, 3}}},
                              .drawnCards = {},
                              .communityCards = {Card{2, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .winners = {1, 0},
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

TEST(ShowdownTest, PairVsPairSplit) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{12, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}, Card{11, 1}, Card{7, 3}, Card{7, 1}, Card{5, 3}};
        TestConfig testConfig{.numPlayers = 2,
                              .smallBlind = 10,
                              .playerChips = {1000, 1000},
                              .playerHands = {{Card{11, 1}, Card{7, 3}}, {Card{7, 1}, Card{5, 3}}},
                              .drawnCards = {},
                              .communityCards = {Card{12, 2}, Card{7, 2}, Card{3, 0}, Card{14, 0}, Card{13, 1}},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}},
                                                {Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}, Action{Actions::CHECK, 0}}},
                              .resultData{
                                  .outPlayers = {false, false},
                                  .foldedPlayers = {false, false},
                                  .nonOutPlayers = 2,
                                  .numActivePlayers = 2,
                                  .winners = {1, 1},
                                  .pot = 40,
                                  .playerChips = {1000, 1000},
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
