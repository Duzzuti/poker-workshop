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
END_MOVES                   // indicates the end of moves
// showdown
// player 0 should win with full house

// result data
// how many chips are in the pot at the end of the game, in format of: POT: <pot>
POT: 360
// chips for each player after the game, in format of: PLAYER_CHIPS: <P0 chips> <P1 chips>...<Pn-1 chips>
PLAYER_CHIPS: 1090 330 100
// which player is out, "t" and "f" for true and false, in format of: PLAYER_OUT: <P0 out?><P1 out?>...<Pn-1 out?>
PLAYER_OUT: fff
// which player folded, "t" and "f" for true and false, in format of: PLAYER_FOLD: <P0 fold?><P1 fold?>...<Pn-1 fold?>
PLAYER_FOLD: fft
// which players are the winner, "t" and "f" for true and false, in format of: WINNER: <P0 win?><P1 win?>...<Pn-1 win?>
// there can be a tie which is considered as win for all tie players
WINNER: tff
// on which stage did the game end (preflop/flop/turn/river), in format of: GAME_STAGE: <p|f|t|r>
GAME_STAGE: r