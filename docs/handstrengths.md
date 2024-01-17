# Handstrengths

There are two parameter that determine the strength of a hand: 
- `hand kind` 
- `rank strength`

## Hand kind
In poker, there are 10 different hands. In order of strength, they are:
1. Royal flush
2. Straight flush
3. Four of a kind
4. Full house
5. Flush
6. Straight
7. Three of a kind
8. Two pair
9. One pair
10. High card

## Rank strength
The `rank strength` is a way to determine a winner when two players have the same `hand kind`. The `rank strength` is determined by the ranks of the variable cards (not included to the `hand kind`) in the hand. The higher the `rank strength`, the stronger the hand.

`hc` = hand card; A card that is included in the `hand kind` \
`nhc` = non-hand card; A card that is not included in the `hand kind`

| Hand kind | Rank strength computation |
| --- | --- |
| Royal flush | 0 |
| Straight flush | highest hc |
| Four of a kind | hc * 16 + highest nhc |
| Full house | triple hc * 16 + double hc |
| Flush | highest hc * 16^4 + second highest hc * 16^3 + third highest hc * 16^2 + fourth highest hc * 16 + lowest hc |
| Straight | highest hc |
| Three of a kind | hc * 16^2 + highest nhc * 16 + second highest nhc |
| Two pair | highest hc * 16^2 + second highest hc * 16 + highest nhc |
| One pair | hc * 16^3 + highest nhc * 16^2 + second highest nhc * 16 + third highest nhc |
| High card | highest hc * 16^4 + second highest hc * 16^3 + third highest hc * 16^2 + fourth highest hc * 16 + lowest hc |

## Hand comparison
In order to compare two hands, we first compare the `hand kinds`. If the `hand kinds` are different, the hand with the higher `hand kind` is the winner. If the `hand kinds` are the same, we compare the `rank strengths`. If the `rank strengths` are the same, the hands are equal.