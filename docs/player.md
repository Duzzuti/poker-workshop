# Player
for all available player implementations see [players](players.md) 

A player has only access to its hand cards and is initialized with a number which represents the player's position on the table.

Action `turn`(const Data& data, const bool onlyRaise) is called when it is the player's turn. The player has to return a valid action. The action is represented by a struct of the form (`action_type`, `bet`). The `action_type` is an enum which represents the type of the action. If the `action_type` requires a bet, the `bet` is the second element of the struct. If the `action_type` does not require a bet, the bet is ignored.
If `onlyRaise` is true, the player is only allowed to raise or call. This is the case when the player is the big blind and no one has raised yet. The player can raise or just call the big blind (basically not adding any chips to the pot).

The player does also keep track of his own wins.

The player has access to all information from the [data](data.md) struct.