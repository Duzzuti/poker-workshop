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
- bool array of players who are out (bool[])

### Round Data
The round data contains information about one round (until the pot is won). It is stored in the `RoundData` struct and has the following form:
- small blind (int)
- big blind (int)
- add blind (int, which is added to the small blind after the button has moved one time around the table)
- dealer position (int)
- small blind position (int)
- big blind position (int)
- pot (int)
- bool array of players who folded (bool[])
- community cards (Card[])
- OutEnum which represents the state of the round (OutEnum)
- BetRoundState which represents which bet round is currently active (BetRoundState)

### Bet Round Data
The bet round data contains information about one bet round (until all players have bet the same amount). It is stored in the `BetRoundData` struct and has the following form:
- position of the player who has to act (int)
- current bet (int)
- player bets (int[])