# Player
A player has only access to its hand cards and is initialized with a number which represents the player's position on the table.

Action `turn`(const Data& data) is called when it is the player's turn. The player has to return a valid action. The action is represented by a struct of the form (`action_type`, `bet`). The `action_type` is an enum which represents the type of the action. If the `action_type` requires a bet, the `bet` is the second element of the struct. If the `action_type` does not require a bet, the bet is ignored.

The player has access to all information from the [data](data.md) struct.