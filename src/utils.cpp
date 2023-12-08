#include <stdlib.h>
#include <time.h>

#include "player.h"

void shufflePlayers(Player* players[], u_int8_t numPlayer) noexcept{
    // shuffle deck
    srand(static_cast<unsigned int>(time(nullptr))); // Seed for randomization

    for (u_int8_t i = numPlayer - 1; i > 0; --i) {
        u_int8_t j = rand() % (i + 1); // Generate a random index within the unshuffled part of the array

        // Swap characters at indices i and j
        Player* temp = players[i];
        players[i] = players[j];
        players[j] = temp;
    }
}