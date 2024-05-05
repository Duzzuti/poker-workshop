# Game Data
There are two types of data that is used in the game:
- player data
- simulation data

## Player Data
The player data is only available to the player and contains the player´s hand cards only. It is stored directly in the players object. All other information is available for all players and is stored in the simulation data.

## Simulation Data
The simulation data is available to all players and contains all information about the simulation. It is stored in the game object as `Data` and is passed to the player in the `turn` function. The player can use this information to make a decision.
`Data` is a struct of the following form:
- number of players (int)
- game data (struct)
- round data (struct)
- bet round data (struct)

### Game Data
The game data contains information about one poker game (until only one player is not out yet). It is stored in the `GameData` struct and has the following form:
- number of chips per player (int[])
- number of non out players (int)
- number of wins per player so far (in previous games) (int[], this array is updated after each player shuffle to correct the order of the players)
- number of chip wins per player so far (in previous games) (int[])
- amount of chips won per player so far (in previous games) (int[])
- bool array of players who are out (bool[])

### Round Data
The round data contains information about one round (until the pot is won). It is stored in the `RoundData` struct and has the following form:
- small blind (int)
- big blind (int)
- actual big blind bet (int, if big blind is all-in this is less than big blind)
- add blind (int, which is added to the small blind after the button has moved one time around the table)
- dealer position (int)
- small blind position (int)
- big blind position (int)
- pot (int)
- number of players that are still in the round (int)
- number of players who are all-in (int)
- bool array of players who folded (bool[])
- community cards (Card[])
- OutEnum which represents the state of the round (OutEnum)
- BetRoundState which represents which bet round is currently active (BetRoundState)
- total bets per player in this round (int[])

### Bet Round Data
The bet round data contains information about one bet round (until all players have bet the same amount). It is stored in the `BetRoundData` struct and has the following form:
- position of the player who has to act (int)
- current bet (int)
- minimum possible raise = last bet or raise (int)
- player bets (int[])