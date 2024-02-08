FILE_NAME: gametest_test.cpp    // file name, stored in gametests folder

TEST: ClassName TestName        // googletest class name and test name
PLAYER_NUM: 3                   // number of players
PLAYER_CHIPS: 900 500 120       // chips for each player separated by space
PLAYER_CARDS: H3SA D7DJ RS2     // cards for each player separated by space (H3SA: Heart 3, Spade Ace, etc., R for random card)
COMMUNITY_CARDS: CAC3D3S5R      // community cards (flop: first 3 cards, turn: 4th card, river: 5th card, R for random card)
SMALL_BLIND: 10                 // small blind (big blind is 2 * small blind)

// the following are the action taken by each player
// the player is identified with P0, P1, P2, etc. until Pn-1

// actions are in the format of: P<n>: <action>[<amount>]
// actions are: c (call), r<bet> (raise), f (fold), chk (check), b<bet> (bet)
// small blind and big blind are set, first-to-act player is P0 (for 2 or 3 players) or P3 (for 4 or more players)

// preflop
P0: 

// flop (first-to-act player is P1)
P1: 

// turn (first-to-act player is P1)
P1: 

// river (first-to-act player is P1)
P1: 

// showdown

