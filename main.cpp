#include <iostream>
#include "antworld.h"

/** @brief The main function that will run the game. If you are not using a IDE gui, this is the executable you want to target when you build
 */
int main() {
    AntWorld gameInstance = AntWorld();

    bool gameOver = false;

    while (not gameOver) {
        gameOver = gameInstance.worldStep();
    }

    printf("GAME OVER!! Total score: %d\n", gameInstance.score);
}
