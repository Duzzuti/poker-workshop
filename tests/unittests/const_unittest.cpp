#include <gtest/gtest.h>

#include "game_test.h"

TEST(CONST, IntLens) {
    u_int64_t i = -1;
    EXPECT_EQ(LEN_UINT64, std::to_string(i).length());
}

TEST(CONST, Chips) {
    for (u_int64_t i = 2; i < MAX_PLAYERS; i++) {
        EXPECT_NO_THROW(Config(1, i, MAX_CHIPS / i, 10, 1));
        EXPECT_THROW(Config(1, i, (MAX_CHIPS / i) + 1, 10, 1), std::invalid_argument);
    }
}

TEST(CONST, PlayerNameLength) {
    EXPECT_NO_THROW(CheckPlayer{""});
    EXPECT_NO_THROW(CheckPlayer{"Player"});

    for (u_int8_t i = 0; i <= MAX_PLAYER_NAME_LENGTH; i++) {
        EXPECT_NO_THROW(CheckPlayer{std::string(i, 'a').c_str()});
    }
    for (u_int8_t i = MAX_PLAYER_NAME_LENGTH + 1; i < MAX_PLAYER_NAME_LENGTH + 10; i++) {
        EXPECT_THROW(CheckPlayer{std::string(i, 'a').c_str()}, std::invalid_argument);
    }
}

TEST(CONST, PlayerGetNameLength) {
    CheckPlayer p{"Player"};
    EXPECT_GE(MAX_PLAYER_GET_NAME_LENGTH, std::strlen(p.getName()));
    for (u_int8_t i = 0; i <= MAX_PLAYER_NAME_LENGTH; i++) {
        CheckPlayer p{std::string(i, 'a').c_str()};
        p.setPlayerPosNum(0);
        // actual max is name255 but playerPosNum is limited by MAX_PLAYERS, test with 0 that is always valid (but we need to subtract 2 for the missing digits)
        EXPECT_GE(MAX_PLAYER_GET_NAME_LENGTH-2, std::strlen(p.getName()));
    }
}

TEST(CONST, PlayerInfoLength) {
    GameTest game{Config{1, 5, INT64_MAX, 10, 1}};
    game.setRealisticPlayers();
    for (u_int8_t i = 0; i < 5; i++) {
        EXPECT_GE(MAX_GET_PLAYER_INFO_LENGTH, std::strlen(game.getPlayerInfo(i, INT64_MAX)));
        EXPECT_GE(MAX_GET_PLAYER_INFO_LENGTH, std::strlen(game.getPlayerInfo(i, INT64_MIN)));
    }
}