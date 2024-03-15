FILE_NAME: showdowns_gametest.cpp
CMAKE_TEST_NAME: showdowns

TEST: ShowdownTest RoyalFlushVsRoyalFlush
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: SASK H9H8 RR
COMMUNITY_CARDS: HAHKHQHJHT
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 1000 1000 1000
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: ttt
GAME_STAGE: r

TEST: ShowdownTest RoyalFlushVsStraightFlush
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: HAC8 H9C5 RR
COMMUNITY_CARDS: H2HKHQHJHT
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 1040 980 980
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: tff
GAME_STAGE: r

TEST: ShowdownTest StraightFlushVsStraightFlushClose
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: HQR HAH7 HKR
COMMUNITY_CARDS: H2HJHTH9H8
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 1040 980 980
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: tff
GAME_STAGE: r

TEST: ShowdownTest StraightFlushVsStraightFlushSplit
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: HAC8 HKCA H6H5
COMMUNITY_CARDS: H7HJHTH9H8
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 1000 1000 1000
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: ttt
GAME_STAGE: r

TEST: ShowdownTest FullHouseVsFourOfAKind
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR H3C5 H8C8
COMMUNITY_CARDS: S8D8D3C3H4
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 980 980 1040
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: fft
GAME_STAGE: r

TEST: ShowdownTest FourOfAKindVsFourOfAKind
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR H3S3 H8C8
COMMUNITY_CARDS: S8D8D3C3H4
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 980 980 1040
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: fft
GAME_STAGE: r

TEST: ShowdownTest FourOfAKindVsFourOfAKindClose
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: H2H3 HKH7 CAC2
COMMUNITY_CARDS: S8D8C8H8H4
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 980 980 1040
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: fft
GAME_STAGE: r

TEST: ShowdownTest FourOfAKindVsFourOfAKindSplit
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: H2H3 HKHA CAC2
COMMUNITY_CARDS: S8D8C8H8H4
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 980 1010 1010
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: ftt
GAME_STAGE: r

TEST: ShowdownTest FlushVsStraight
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: RR SKSA H7R
COMMUNITY_CARDS: S3S4STD5D6
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 980 1040 980
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: ftf
GAME_STAGE: r

TEST: ShowdownTest StraightVsStraightClose
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: H2H3 S2CK H7DA
COMMUNITY_CARDS: S3S4STD5D6
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 980 980 1040
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: fft
GAME_STAGE: r

TEST: ShowdownTest StraightVsStraightSplit
PLAYER_NUM: 3
PLAYER_CHIPS: 1000 1000 1000
PLAYER_CARDS: C7D8 S7H8 H7DA
COMMUNITY_CARDS: S3S4STD5D6
SMALL_BLIND: 10

P0: c
P1: c
P2: c

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

P0: chk
P1: chk
P2: chk

END_MOVES

POT: 60
PLAYER_CHIPS: 1010 1010 980
PLAYER_OUT: fff
PLAYER_FOLD: fff
WINNER: ttf
GAME_STAGE: r

TEST: ShowdownTest PairVsHighCard
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: HJH8 H7R
COMMUNITY_CARDS: S2S7D3DAHK
SMALL_BLIND: 10

P0: c
P1: c

P0: chk
P1: chk

P0: chk
P1: chk

P0: chk
P1: chk

END_MOVES

POT: 40
PLAYER_CHIPS: 980 1020
PLAYER_OUT: ff
PLAYER_FOLD: ff
WINNER: ft
GAME_STAGE: r

TEST: ShowdownTest PairVsPair
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: HJH3 H7C5
COMMUNITY_CARDS: S2S7D3DAHK
SMALL_BLIND: 10

P0: c
P1: c

P0: chk
P1: chk

P0: chk
P1: chk

P0: chk
P1: chk

END_MOVES

POT: 40
PLAYER_CHIPS: 980 1020
PLAYER_OUT: ff
PLAYER_FOLD: ff
WINNER: ft
GAME_STAGE: r

TEST: ShowdownTest PairVsPairClose
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: HJC7 H7C5
COMMUNITY_CARDS: S2S7D3DAHK
SMALL_BLIND: 10

P0: c
P1: c

P0: chk
P1: chk

P0: chk
P1: chk

P0: chk
P1: chk

END_MOVES

POT: 40
PLAYER_CHIPS: 1020 980
PLAYER_OUT: ff
PLAYER_FOLD: ff
WINNER: tf
GAME_STAGE: r

TEST: ShowdownTest PairVsPairSplit
PLAYER_NUM: 2
PLAYER_CHIPS: 1000 1000
PLAYER_CARDS: HJC7 H7C5
COMMUNITY_CARDS: SQS7D3DAHK
SMALL_BLIND: 10

P0: c
P1: c

P0: chk
P1: chk

P0: chk
P1: chk

P0: chk
P1: chk

END_MOVES

POT: 40
PLAYER_CHIPS: 1000 1000
PLAYER_OUT: ff
PLAYER_FOLD: ff
WINNER: tt
GAME_STAGE: r