//
// Created by dusan on 9/15/26.
//

#include "../include/antworld.h"
#include <iostream>

//function prototypes
bool isCheckerboard(Coord pos);
void flipOffChecker(std::vector<std::vector<int> > &pheromoneMap, Ant &ant);
void flipOnChecker(std::vector<std::vector<int> > &pheromoneMap, Ant &ant);
Coord offPatternCheckerboard(std::vector<std::vector<int> > &pheromoneMap, std::vector<Coord> positions);
std::vector<Coord> scanSurroundings(Ant &ant, MapTemplate &map, Coord position, int radius);

/** @brief this is where you as the applicant will make use of the above functions to develop your solution.
 * here are some existing examples of how calling these functions works to help get you started!
 */
void AntWorld::forage() {
    /*
    09/22/26
        ALGOS AND SCORES WITH BASE SEED 12345
        basic wandering algo. score = 9
        added search w/pheromones; score = 28

    09/24/26
        tried implementing the checkerboard pheromone strategy; score = 22
    */
    /* hmm how to spread out ants..
    all start at home location i think. maybe move in random direction, layinng pheromone
    if phermone is detected, avoid it.
    if have food, return home only when energy cost = all energy available wait no can only carry one food at a time. 
    can we have multiple pheromone types ?? can i do that
    probably not lol. maybe instead make pattern (like checkerboard, so violations of pattern can also be detected as smth else)
    if no food, keep moving until energy is depleted, mapping out as you go
    1. search for surrounding food in view radius
    2. if food found, move to it and pick it up, move home

    */
    
    //
    // Coord desiredDestination = Coord(5, 5);
    // Coord finalPos = this->ants[0].move(this->terrainMap, desiredDestination, this->foodMap);
    // bool destCheck = (desiredDestination == finalPos);
    // //
    // this->ants[0].dropPheromone(this->pheromoneMap);
    //
    // this->ants[0].erasePheromone(this->pheromoneMap);
    //
    // this->ants[0].returnHome(this->terrainMap, this->foodMap);
    
    //all ants perform same actions
    for (int i = 0; i < this->ants.size(); ++i) {
        //scan for food and pheromones in view radius
        std::vector<Coord> visibleFood = this->ants[i].foodScan(this->foodMap);
        std::vector<Coord> visiblePheromones = this->ants[i].pheromoneScan(this->pheromoneMap);

        //drop pheromone marking to create checkerboard
        flipOnChecker(this->pheromoneMap, this->ants[i]);
        
        //prioritize returning home with food, then looking for food, then exploring.
        // go home if carrying food (CHANGE maybe: if too far from home, wander instead?)
        if (this->ants[i].carryingFood) {
            //marks food seen nearby before heading home
            if(!visibleFood.empty()) {
                this->ants[i].move(this->terrainMap, visibleFood[0], this->foodMap);
                flipOffChecker(this->pheromoneMap, this->ants[i]);
            }
            this->ants[i].returnHome(this->terrainMap, this->foodMap);

            //drops pheromone if about to die
            if (this->ants[i].energy == 0) {
                this->ants[i].dropPheromone(this->pheromoneMap);
            }

        } else if(!visibleFood.empty()) {
            // look for food in view radius (if not carrying food), move there
            this->ants[i].move(this->terrainMap, visibleFood[0], this->foodMap);
            flipOnChecker(this->pheromoneMap, this->ants[i]);
        } 
        // look for off-pattern positions, move to those
        else {
            Coord offPatternPos = offPatternCheckerboard(this->pheromoneMap, visiblePheromones);
            //checking if any off-pattern positions were found, if so move to first one
            if (offPatternPos.first != -1 && offPatternPos.second != -1) {
                this->ants[i].move(this->terrainMap, offPatternPos, this->foodMap);
            } else {
                // otherwise, move to random location on edge of radius
                // UHH IMPLEMENT THIS LATER LOL
                std::uniform_int_distribution<int> rowDist(0, this->terrainMap.size() - 1);
                std::uniform_int_distribution<int> colDist(0, this->terrainMap[0].size() - 1);
                Coord randomDestination = {rowDist(rng), colDist(rng)};
                this->ants[i].move(this->terrainMap, randomDestination, this->foodMap);
            }
            // this->ants[i].move(this->terrainMap, offPatternPos, this->foodMap);
        }
        
    }
}

/** You may insert any custom functions below **/
//returns all coordinates an ant can see
std::vector<Coord> scanSurroundings(Ant &ant, MapTemplate &map, Coord position, int radius) {
    std::vector<Coord> visibleCoords;
    for (int i = ant.position.first - ant.foodRadius; i <= ant.position.first + ant.foodRadius; ++i) {
        for (int j = ant.position.second - ant.foodRadius; j <= ant.position.second + ant.foodRadius; ++j) {
            if (i < 0 || i >= map.size() || j < 0 || j >= map[0].size()) {
                continue;
            } else {
                visibleCoords.push_back({i, j});
            }
        }
    }
    return visibleCoords;
}

//sees if position coordinate is on a 'checkerboard' pattern (ex. (1,1), (1,3), (2,2).. etc)
bool isCheckerboard(Coord pos) {
    return (pos.first ^ pos.second) & 1;
}

void flipOffChecker(std::vector<std::vector<int> > &pheromoneMap, Ant &ant) {
    if (isCheckerboard(ant.position)) {
        ant.erasePheromone(pheromoneMap);
    } else {
        ant.dropPheromone(pheromoneMap);
    }
}
void flipOnChecker(std::vector<std::vector<int> > &pheromoneMap, Ant &ant) {
    if (!isCheckerboard(ant.position)) {
        ant.erasePheromone(pheromoneMap);
    } else {
        ant.dropPheromone(pheromoneMap);
    }
}

//if the position is on the checkerboard, should have pheromone on it, if not should be empty.
//if above is false, move to first of that position found!
Coord offPatternCheckerboard(
    std::vector<std::vector<int> > &pheromoneMap, 
    std::vector<Coord> positions) {
    std::vector<Coord> offPatternPositions;
    for (const auto& pos : positions) {
        if (isCheckerboard(pos)) {
            if (pheromoneMap[pos.first][pos.second] == 0) {
                return pos;
                // offPatternPositions.push_back(pos);
            }
        } else {
            offPatternPositions.push_back(pos);
            if (pheromoneMap[pos.first][pos.second] == 1) {
                return pos;
                // offPatternPositions.push_back(pos);
            }
        }
    }
    //nothing found
    return {-1, -1};
    
    // return offPatternPositions;

}