#include <iostream>
#include "antworld.h"


/** @brief this is where you as the applicant will make use of the above functions to develop your solution.
 * here are some existing examples of how calling these functions works to help get you started!
 */
void AntWorld::forage() {
    std::vector<Coord> visibleFood = this->ants[0].foodScan(this->foodMap);

    Coord desiredDestination = Coord(5, 5);
    Coord finalPos = this->ants[0].move(this->terrainMap, desiredDestination, this->foodMap);
    bool destCheck = (desiredDestination == finalPos);

    this->ants[0].dropPheromone(this->pheromoneMap);

    this->ants[0].erasePheromone(this->pheromoneMap);

    this->ants[0].returnHome(this->terrainMap, this->foodMap);
}

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
