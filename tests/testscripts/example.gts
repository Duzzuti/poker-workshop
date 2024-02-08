// comment, empty lines are not considered
FILE_NAME: test_gametest.cpp    // file name, should be unique, stored in gametests folder
CMAKE_TEST_NAME: test           // cmake test name, should be unique, used in cmake add_test

TEST: ClassName TestName        // googletest class name and test name
// these can be in any order until SMALL_BLIND
PLAYER_NUM: 3                   // number of players
PLAYER_CHIPS: 900 500 120       // chips for each player separated by space
PLAYER_CARDS: H3SA D7DJ RS2     // cards for each player separated by space (H3SA: Heart 3, Spade Ace, etc., R for random card)
COMMUNITY_CARDS: CAC3D3S5R      // community cards (flop: first 3 cards, turn: 4th card, river: 5th card, R for random card)
SMALL_BLIND: 10                 // small blind (big blind is 2 * small blind)

// the following are the action taken by each player
// the player is identified with P0, P1, P2, etc. until Pn-1
// please note that you have to TODO

// actions are in the format of: P<n>: <action>[<amount>]
// actions are: c (call), r<bet> (raise), f (fold), chk (check), b<bet> (bet)
// small blind and big blind are set, first-to-act player is P0 (for 2 or 3 players) or P3 (for 4 or more players)

// preflop
P0: c
P1: r40
P2: f
P0: c
// flop (first-to-act player is P1)
P1: b30
P0: c
// turn (first-to-act player is P1)
P1: chk
P0: chk
// river (first-to-act player is P1)
P1: b50
P0: r100
P1: c
// showdown
// player 0 should win with full house

