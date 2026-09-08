#include <iostream>
#include "antworld.h"

#include <iostream>
#include <vector>

void printMap(const std::vector<std::vector<int> > &map) {
    for (const auto &row: map) {
        for (int cell: row) {
            std::cout << cell << " ";
        }
        std::cout << '\n';
    }
}

// TIP test test test


int main() {
    AntWorld gameInstance = AntWorld();

    printMap(gameInstance.colony.world->terrainMap);
    printMap(gameInstance.colony.world->itemMap);

    bool gameOver = false;

    while (not gameOver) {
        gameOver = gameInstance.step();
    }

    printf("GAME OVER!! Total score: %d\n", gameInstance.colony.score);
}
