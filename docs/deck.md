# Deck and Card
Card class hold the information of a card
- unsigned char suit: the suit of the card
- unsigned char rank: the rank of the card

Deck class hold the information of a deck of cards
- Card cards[52]: the cards in the deck
- unsigned char len: the number of cards currently in the deck 
- you can shuffle the deck or draw a card from the deck
- for a new deck, you can use the constructor to initialize it