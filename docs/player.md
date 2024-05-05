# Player
for all available player implementations see [players](players.md) 

A player has only access to its hand cards and is initialized with a number which represents the player's position on the table.

Action `turn`(const Data& data, const bool blindOption, const bool equalize) is called when it is the player's turn. The player has to return a valid action. The action is represented by a struct of the form (`action_type`, `bet`). The `action_type` is an enum which represents the type of the action. If the `action_type` requires a bet, the `bet` is the second element of the struct. If the `action_type` does not require a bet, the bet is ignored.
If `blindOption` is true, the player is only allowed to raise, call or all-in. This is the case when the player is the big blind and no one has raised yet. The player can raise or just call the big blind (basically not adding any chips to the pot).
If `equalize` is true, the player is only allowed to fold, call or all-in (but only if the all-in amount is not greater than the current bet). This is the case when all other players are all-in or out and the player has to equalize the bets of the other players.

The player has access to all information from the [data](data.md) struct.