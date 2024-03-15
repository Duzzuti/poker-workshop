#include <gtest/gtest.h>

#include "game_test.h"
#include "test_player/test_player.h"

TEST(ClassName, TestName) {
    for (int iters = 0; iters < TEST_ITERS; iters++) {
        std::vector<Card> drawnCards = {Card{14, 3}, Card{3, 3}, Card{3, 0}, Card{5, 2}, Card{3, 1}, Card{14, 2}, Card{7, 0}, Card{11, 0}, Card{2, 2}};
        TestConfig testConfig{.numPlayers = 3,
                              .smallBlind = 10,
                              .playerChips = {900, 500, 120},
                              .playerHands = {{Card{3, 1}, Card{14, 2}}, {Card{7, 0}, Card{11, 0}}, {Deck::getRandomCardExceptAdd(drawnCards), Card{2, 2}}},
                              .drawnCards = {},
                              .communityCards = {Card{14, 3}, Card{3, 3}, Card{3, 0}, Card{5, 2}, Deck::getRandomCardExceptAdd(drawnCards)},
                              .playerActions = {{Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::CALL, 0}, Action{Actions::CHECK, 0}, Action{Actions::RAISE, 100}},
                                                {Action{Actions::RAISE, 40}, Action{Actions::BET, 30}, Action{Actions::CHECK, 0}, Action{Actions::BET, 50}, Action{Actions::CALL, 0}},
                                                {Action{Actions::FOLD, 0}}},
                              .resultData{
                                  .outPlayers = {false, false, false},
                                  .foldedPlayers = {false, false, true},
                                  .nonOutPlayers = 3,
                                  .numActivePlayers = 2,
                                  .gameWins = {1, 0, 0},
                                  .pot = 360,
                                  .playerChips = {1090, 330, 100},
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
