FILE_NAME: normal_gametest.cpp
CMAKE_TEST_NAME: normal

TEST: NormalTest norm1
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: r40
P1: r60
P2: c
P0: c

P1: chk
P2: chk
P0: chk

P1: chk
P2: b30
P0: r60
P1: r90
P2: f
P0: c

P1: b70
P0: f

END_MOVES

POT: 460
PLAYER_CHIPS: 850 1240 910
PLAYER_OUT: fff
PLAYER_FOLD: tft
WINNER: ftf
GAME_STAGE: r

TEST: NormalTest norm2
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: f
P1: f
P2: f

END_MOVES

POT: 30
PLAYER_CHIPS: 1000 990 1010
PLAYER_OUT: fff
PLAYER_FOLD: ttf
WINNER: fft
GAME_STAGE: p

TEST: NormalTest norm3
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c
P2: r40
P0: f
P1: c

P1: chk
P2: f

END_MOVES

POT: 100
PLAYER_CHIPS: 980 1060 960
PLAYER_OUT: fff
PLAYER_FOLD: tft
WINNER: ftf
GAME_STAGE: f

TEST: NormalTest norm4
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 100 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: r120    // illegal move
P2: r40
P0: f

END_MOVES

POT: 70
PLAYER_CHIPS: 980 0 1030
PLAYER_OUT: ftf
PLAYER_FOLD: tff
WINNER: fft
GAME_STAGE: p

TEST: NormalTest norm5
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 100 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: r120    // illegal move
P2: r40
P0: c

P2: f

END_MOVES

POT: 90
PLAYER_CHIPS: 1050 0 960
PLAYER_OUT: ftf
PLAYER_FOLD: fft
WINNER: tff
GAME_STAGE: f

TEST: NormalTest norm6
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: r40
P0: c

P1: b20
P0: c

P1: b20
P0: c

P1: f

END_MOVES

POT: 160
PLAYER_CHIPS: 1080 920
PLAYER_OUT: ff
PLAYER_FOLD: ft
WINNER: tf
GAME_STAGE: r