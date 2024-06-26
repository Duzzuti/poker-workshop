FILE_NAME: edge_cases_gametest.cpp
CMAKE_TEST_NAME: edge_cases

TEST: EdgeCaseTest TwoPlayers1
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: f
P1: f

END_MOVES

POT: 30
PLAYER_CHIPS: 990 1010
PLAYER_OUT: ff
PLAYER_FOLD: tf
WINNER: ft
GAME_STAGE: p

TEST: EdgeCaseTest TwoPlayers2
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c

P1: f
P0: f

END_MOVES

POT: 40
PLAYER_CHIPS: 1020 980
PLAYER_OUT: ff
PLAYER_FOLD: ft
WINNER: tf
GAME_STAGE: f

TEST: EdgeCaseTest TwoPlayers3
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c

P1: chk
P0: chk

P1: f
P0: f

END_MOVES

POT: 40
PLAYER_CHIPS: 1020 980
PLAYER_OUT: ff
PLAYER_FOLD: ft
WINNER: tf
GAME_STAGE: t

TEST: EdgeCaseTest TwoPlayers4
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c

P1: chk
P0: chk

P1: chk
P0: chk

P1: f
P0: f

END_MOVES

POT: 40
PLAYER_CHIPS: 1020 980
PLAYER_OUT: ff
PLAYER_FOLD: ft
WINNER: tf
GAME_STAGE: r

TEST: EdgeCaseTest ThreePlayers1
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: r40
P1: r60
P2: r80
P0: f
P1: f
P2: f

END_MOVES

POT: 180
PLAYER_CHIPS: 960 940 1100
PLAYER_OUT: fff
PLAYER_FOLD: ttf
WINNER: fft
GAME_STAGE: p

TEST: EdgeCaseTest ThreePlayers2
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P1: b20
P2: r40
P0: r60
P1: f
P2: f
P0: f

END_MOVES

POT: 180
PLAYER_CHIPS: 1100 960 940
PLAYER_OUT: fff
PLAYER_FOLD: ftt
WINNER: tff
GAME_STAGE: f

TEST: EdgeCaseTest ThreePlayers3
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P1: chk
P2: chk
P0: chk

P1: b20
P2: r40
P0: r60
P1: f
P2: f
P0: f

END_MOVES

POT: 180
PLAYER_CHIPS: 1100 960 940
PLAYER_OUT: fff
PLAYER_FOLD: ftt
WINNER: tff
GAME_STAGE: t

TEST: EdgeCaseTest ThreePlayers4
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P1: chk
P2: chk
P0: chk

P1: chk
P2: chk
P0: chk

P1: b20
P2: r40
P0: r60
P1: f
P2: f
P0: f

END_MOVES

POT: 180
PLAYER_CHIPS: 1100 960 940
PLAYER_OUT: fff
PLAYER_FOLD: ftt
WINNER: tff
GAME_STAGE: r

TEST: EdgeCaseTest FourPlayers1
PLAYER_NUM: 4
PLAYER_CHIPS: 1000 1000 1000 1000
PLAYER_CARDS: RR RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P3: r40
P0: r60
P1: r80
P2: r100
P3: f
P0: f
P1: f
P2: f

END_MOVES

POT: 280
PLAYER_CHIPS: 940 920 1180 960
PLAYER_OUT: ffff
PLAYER_FOLD: ttft
WINNER: fftf
GAME_STAGE: p

TEST: EdgeCaseTest FourPlayers2
PLAYER_NUM: 4
PLAYER_CHIPS: 1000 1000 1000 1000
PLAYER_CARDS: RR RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P3: c
P0: c
P1: c
P2: c

P1: b20
P2: r40
P3: r60
P0: r80
P1: f
P2: f
P3: f
P0: f

END_MOVES

POT: 280
PLAYER_CHIPS: 1180 960 940 920
PLAYER_OUT: ffff
PLAYER_FOLD: fttt
WINNER: tfff
GAME_STAGE: f

TEST: EdgeCaseTest FourPlayers3
PLAYER_NUM: 4
PLAYER_CHIPS: 1000 1000 1000 1000
PLAYER_CARDS: RR RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P3: c
P0: c
P1: c
P2: c

P1: chk
P2: chk
P3: chk
P0: chk

P1: b20
P2: r40
P3: r60
P0: r80
P1: f
P2: f
P3: f
P0: f

END_MOVES

POT: 280
PLAYER_CHIPS: 1180 960 940 920
PLAYER_OUT: ffff
PLAYER_FOLD: fttt
WINNER: tfff
GAME_STAGE: t

TEST: EdgeCaseTest FourPlayers4
PLAYER_NUM: 4
PLAYER_CHIPS: 1000 1000 1000 1000
PLAYER_CARDS: RR RR RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P3: c
P0: c
P1: c
P2: c

P1: chk
P2: chk
P3: chk
P0: chk

P1: chk
P2: chk
P3: chk
P0: chk

P1: b20
P2: r40
P3: r60
P0: r80
P1: f
P2: f
P3: f
P0: f

END_MOVES

POT: 280
PLAYER_CHIPS: 1180 960 940 920
PLAYER_OUT: ffff
PLAYER_FOLD: fttt
WINNER: tfff
GAME_STAGE: r


TEST: EdgeCaseTest RaiseLastChip
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: r1000       // illegal move (all in has an extra action)
P1: f

END_MOVES

POT: 30
PLAYER_CHIPS: 0 1010
PLAYER_OUT: tf
PLAYER_FOLD: ff
WINNER: ft
GAME_STAGE: p

TEST: EdgeCaseTest RaiseLastChipClose
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: r999
P1: f

END_MOVES

POT: 1019
PLAYER_CHIPS: 1020 980
PLAYER_OUT: ff
PLAYER_FOLD: ft
WINNER: tf
GAME_STAGE: p

TEST: EdgeCaseTest BetLastChip
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c

P1: b980       // illegal move (all in has an extra action)
P0: f

END_MOVES

POT: 40
PLAYER_CHIPS: 1020 0
PLAYER_OUT: ft
PLAYER_FOLD: ff
WINNER: tf
GAME_STAGE: f

TEST: EdgeCaseTest BetLastChipClose
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c

P1: b979
P0: f

END_MOVES

POT: 1019
PLAYER_CHIPS: 980 1020
PLAYER_OUT: ff
PLAYER_FOLD: tf
WINNER: ft
GAME_STAGE: f

TEST: EdgeCaseTest CallLastChip
PLAYER_NUM: 2
PLAYER_CHIPS: 10000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c

P1: chk
P0: b980 
P1: c               // illegal move (all in has an extra action)

P1: chk
P0: f
P1: f

END_MOVES

POT: 1020
PLAYER_CHIPS: 10020 0
PLAYER_OUT: ft
PLAYER_FOLD: ff
WINNER: tf
GAME_STAGE: f

TEST: EdgeCaseTest CallLastChipClose
PLAYER_NUM: 2
PLAYER_CHIPS: 10000 1000
PLAYER_CARDS: RR RR
COMMUNITY_CARDS: RRRRR
SMALL_BLIND: 10

P0: c
P1: c

P1: chk
P0: b979
P1: c

P1: chk
P0: f
P1: f

END_MOVES

POT: 1998
PLAYER_CHIPS: 9001 1999
PLAYER_OUT: ff
PLAYER_FOLD: tf
WINNER: tf
GAME_STAGE: t