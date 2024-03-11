//************************ ILLEGAL CHECK GAME TESTS ************************//
FILE_NAME: illegal_moves_gametest.cpp    // file name, should be unique, stored in gametests folder
CMAKE_TEST_NAME: illegal_moves           // cmake test name, should be unique, used in cmake add_test

TEST: IllegalMoves Check        // googletest class name and test name
// these can be in any order until SMALL_BLIND
PLAYER_NUM: 6                   // number of players
PLAYER_CHIPS: 1000 1000 1000 1000 1000 1000      // chips for each player separated by space
PLAYER_CARDS: RR RR RR RR RR RR     // cards for each player separated by space (H3SA: Heart 3, Spade Ace, etc., R for random card)
COMMUNITY_CARDS: RRRRR      // community cards (flop: first 3 cards, turn: 4th card, river: 5th card, R for random card)
SMALL_BLIND: 10                 // small blind (big blind is 2 * small blind)

// the following are the action taken by each player
// the player is identified with P0, P1, P2, etc. until Pn-1
// please note that you have to TODO

// actions are in the format of: P<n>: <action>[<amount>]
// actions are: c (call), r<bet> (raise), f (fold), chk (check), b<bet> (bet)
// small blind and big blind are set, first-to-act player is P0 (for 2 or 3 players) or P3 (for 4 or more players)

// preflop
P3: chk
P4: c
P5: c
P0: c
P1: c
P2: chk
// flop (first-to-act player is P1)
P1: chk
P4: b20
P5: c
P0: c
P1: chk
// turn (first-to-act player is P1)
P4: b40
P5: c
P0: c
// river (first-to-act player is P1)
P4: b80
P5: r160
P0: chk
P4: chk
END_MOVES                   // indicates the end of moves
// showdown
// player 5 should win

// result data
// how many chips are in the pot at the end of the game, in format of: POT: <pot>
POT: 520
// chips for each player after the game, in format of: PLAYER_CHIPS: <P0 chips> <P1 chips>...<Pn-1 chips>
PLAYER_CHIPS: 920 980 980 1000 840 1280
// which player is out, "t" and "f" for true and false, in format of: PLAYER_OUT: <P0 out?><P1 out?>...<Pn-1 out?>
PLAYER_OUT: tttttf
// which player folded, "t" and "f" for true and false, in format of: PLAYER_FOLD: <P0 fold?><P1 fold?>...<Pn-1 fold?>
PLAYER_FOLD: ffffff
// which players are the winner, "t" and "f" for true and false, in format of: WINNER: <P0 win?><P1 win?>...<Pn-1 win?>
// there can be a tie which is considered as win for all tie players
WINNER: ffffft
// on which stage did the game end (preflop/flop/turn/river), in format of: GAME_STAGE: <p|f|t|r>
GAME_STAGE: r

// ************************ ILLEGAL CALL GAME TESTS ************************//

TEST: IllegalMoves Call        // googletest class name and test name
// these can be in any order until SMALL_BLIND
PLAYER_NUM: 3                   // number of players
PLAYER_CHIPS: 1000 1000 100      // chips for each player separated by space
PLAYER_CARDS: RR RR RR     // cards for each player separated by space (H3SA: Heart 3, Spade Ace, etc., R for random card)
COMMUNITY_CARDS: RRRRR      // community cards (flop: first 3 cards, turn: 4th card, river: 5th card, R for random card)
SMALL_BLIND: 10                 // small blind (big blind is 2 * small blind)

// the following are the action taken by each player
// the player is identified with P0, P1, P2, etc. until Pn-1
// please note that you have to TODO

// actions are in the format of: P<n>: <action>[<amount>]
// actions are: c (call), r<bet> (raise), f (fold), chk (check), b<bet> (bet)
// small blind and big blind are set, first-to-act player is P0 (for 2 or 3 players) or P3 (for 4 or more players)

// preflop
P0: c
P1: c
P2: c
// flop (first-to-act player is P1)
P1: chk
P2: b20
P0: r91
P1: c
P2: c   // illegal
// turn (first-to-act player is P1)
P1: chk
P0: c   // illegal
// river (first-to-act player is P1)
END_MOVES                   // indicates the end of moves
// showdown

// result data
// how many chips are in the pot at the end of the game, in format of: POT: <pot>
POT: 262
// chips for each player after the game, in format of: PLAYER_CHIPS: <P0 chips> <P1 chips>...<Pn-1 chips>
PLAYER_CHIPS: 889 1151 60
// which player is out, "t" and "f" for true and false, in format of: PLAYER_OUT: <P0 out?><P1 out?>...<Pn-1 out?>
PLAYER_OUT: tft
// which player folded, "t" and "f" for true and false, in format of: PLAYER_FOLD: <P0 fold?><P1 fold?>...<Pn-1 fold?>
PLAYER_FOLD: fff
// which players are the winner, "t" and "f" for true and false, in format of: WINNER: <P0 win?><P1 win?>...<Pn-1 win?>
// there can be a tie which is considered as win for all tie players
WINNER: ftf
// on which stage did the game end (preflop/flop/turn/river), in format of: GAME_STAGE: <p|f|t|r>
GAME_STAGE: t

// ************************ ILLEGAL RAISE GAME TESTS ************************//

TEST: IllegalMoves Raise        // googletest class name and test name
// these can be in any order until SMALL_BLIND
PLAYER_NUM: 6                   // number of players
PLAYER_CHIPS: 100 1000 1000 1000 1000 1000      // chips for each player separated by space
PLAYER_CARDS: RR RR RR RR RR RR    // cards for each player separated by space (H3SA: Heart 3, Spade Ace, etc., R for random card)
COMMUNITY_CARDS: RRRRR      // community cards (flop: first 3 cards, turn: 4th card, river: 5th card, R for random card)
SMALL_BLIND: 10                 // small blind (big blind is 2 * small blind)

// the following are the action taken by each player
// the player is identified with P0, P1, P2, etc. until Pn-1
// please note that you have to TODO

// actions are in the format of: P<n>: <action>[<amount>]
// actions are: c (call), r<bet> (raise), f (fold), chk (check), b<bet> (bet)
// small blind and big blind are set, first-to-act player is P0 (for 2 or 3 players) or P3 (for 4 or more players)

// preflop
P3: r30 // illegal
P4: c
P5: c
P0: c
P1: c
P2: r40
P3: c
P4: c
P5: c
P0: c
P1: c
// flop (first-to-act player is P1)
P1: chk
P2: chk
P4: r40 // illegal
P5: b20 
P0: r70 // illegal
P1: r60
P2: r100
P5: r130 // illegal
P1: c
// turn (first-to-act player is P1)
P1: chk
P2: b40
P1: r60 // illegal
// river (first-to-act player is P1)
END_MOVES                   // indicates the end of moves
// showdown

// result data
// how many chips are in the pot at the end of the game, in format of: POT: <pot>
POT: 460
// chips for each player after the game, in format of: PLAYER_CHIPS: <P0 chips> <P1 chips>...<Pn-1 chips>
PLAYER_CHIPS: 60 860 1280 1000 960 940
// which player is out, "t" and "f" for true and false, in format of: PLAYER_OUT: <P0 out?><P1 out?>...<Pn-1 out?>
PLAYER_OUT: ttfttt
// which player folded, "t" and "f" for true and false, in format of: PLAYER_FOLD: <P0 fold?><P1 fold?>...<Pn-1 fold?>
PLAYER_FOLD: ffffff
// which players are the winner, "t" and "f" for true and false, in format of: WINNER: <P0 win?><P1 win?>...<Pn-1 win?>
// there can be a tie which is considered as win for all tie players
WINNER: fftfff
// on which stage did the game end (preflop/flop/turn/river), in format of: GAME_STAGE: <p|f|t|r>
GAME_STAGE: t

// ************************ ILLEGAL BET GAME TESTS ************************//

TEST: IllegalMoves Bet        // googletest class name and test name
// these can be in any order until SMALL_BLIND
PLAYER_NUM: 4                   // number of players
PLAYER_CHIPS: 100 1000 1000 1000      // chips for each player separated by space
PLAYER_CARDS: RR RR RR RR    // cards for each player separated by space (H3SA: Heart 3, Spade Ace, etc., R for random card)
COMMUNITY_CARDS: RRRRR      // community cards (flop: first 3 cards, turn: 4th card, river: 5th card, R for random card)
SMALL_BLIND: 10                 // small blind (big blind is 2 * small blind)

// the following are the action taken by each player
// the player is identified with P0, P1, P2, etc. until Pn-1
// please note that you have to TODO

// actions are in the format of: P<n>: <action>[<amount>]
// actions are: c (call), r<bet> (raise), f (fold), chk (check), b<bet> (bet)
// small blind and big blind are set, first-to-act player is P0 (for 2 or 3 players) or P3 (for 4 or more players)

// preflop
P3: b100 // illegal
P0: c
P1: c
P2: b20 // illegal
// flop (first-to-act player is P1)
P1: chk
P0: b100 // illegal
// turn (first-to-act player is P1)
// river (first-to-act player is P1)
END_MOVES                   // indicates the end of moves
// showdown

// result data
// how many chips are in the pot at the end of the game, in format of: POT: <pot>
POT: 60
// chips for each player after the game, in format of: PLAYER_CHIPS: <P0 chips> <P1 chips>...<Pn-1 chips>
PLAYER_CHIPS: 80 1040 980 1000
// which player is out, "t" and "f" for true and false, in format of: PLAYER_OUT: <P0 out?><P1 out?>...<Pn-1 out?>
PLAYER_OUT: tftt
// which player folded, "t" and "f" for true and false, in format of: PLAYER_FOLD: <P0 fold?><P1 fold?>...<Pn-1 fold?>
PLAYER_FOLD: ffff
// which players are the winner, "t" and "f" for true and false, in format of: WINNER: <P0 win?><P1 win?>...<Pn-1 win?>
// there can be a tie which is considered as win for all tie players
WINNER: ftff
// on which stage did the game end (preflop/flop/turn/river), in format of: GAME_STAGE: <p|f|t|r>
GAME_STAGE: f

// ************************ ILLEGAL FOLD GAME TESTS ************************//

TEST: IllegalMoves Fold         // googletest class name and test name
// these can be in any order until SMALL_BLIND
PLAYER_NUM: 2                   // number of players
PLAYER_CHIPS: 100 1000          // chips for each player separated by space
PLAYER_CARDS: RR RR             // cards for each player separated by space (H3SA: Heart 3, Spade Ace, etc., R for random card)
COMMUNITY_CARDS: RRRRR          // community cards (flop: first 3 cards, turn: 4th card, river: 5th card, R for random card)
SMALL_BLIND: 10                 // small blind (big blind is 2 * small blind)

// the following are the action taken by each player
// the player is identified with P0, P1, P2, etc. until Pn-1
// please note that you have to TODO

// actions are in the format of: P<n>: <action>[<amount>]
// actions are: c (call), r<bet> (raise), f (fold), chk (check), b<bet> (bet)
// small blind and big blind are set, first-to-act player is P0 (for 2 or 3 players) or P3 (for 4 or more players)

// preflop
P0: c
P1: f // illegal
// flop (first-to-act player is P1)
// turn (first-to-act player is P1)
// river (first-to-act player is P1)
END_MOVES                   // indicates the end of moves
// showdown

// result data
// how many chips are in the pot at the end of the game, in format of: POT: <pot>
POT: 40
// chips for each player after the game, in format of: PLAYER_CHIPS: <P0 chips> <P1 chips>...<Pn-1 chips>
PLAYER_CHIPS: 120 980
// which player is out, "t" and "f" for true and false, in format of: PLAYER_OUT: <P0 out?><P1 out?>...<Pn-1 out?>
PLAYER_OUT: ft
// which player folded, "t" and "f" for true and false, in format of: PLAYER_FOLD: <P0 fold?><P1 fold?>...<Pn-1 fold?>
PLAYER_FOLD: ff
// which players are the winner, "t" and "f" for true and false, in format of: WINNER: <P0 win?><P1 win?>...<Pn-1 win?>
// there can be a tie which is considered as win for all tie players
WINNER: tf
// on which stage did the game end (preflop/flop/turn/river), in format of: GAME_STAGE: <p|f|t|r>
GAME_STAGE: p