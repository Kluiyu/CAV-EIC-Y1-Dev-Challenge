//
// Created by dusan on 9/15/26.
//

#include "../include/antworld.h"
#include <iostream>


/** @brief this is where you as the applicant will make use of the above functions to develop your solution.
 * here are some existing examples of how calling these functions works to help get you started!
 */
void AntWorld::forage() {
    /*
    09/22/26
        basic wandering algo. score = 9
    */
    /* hmm how to spread out ants..
    all start at home location i think. maybe move in random direction, layinng pheromone
    if phermone is detected, avoid it.
    if have food, return home only when energy cost = all energy available wait no can only carry one food at a time. 
    can we have multiple pheromone types ?? can i do that
    probably not lol. maybe instead make pattern (like checkerboard, so violations of pattern can also be detected as smth else)
    if no food, keep moving until energy is depleted, mapping out as you go
    1. search for surrounding food in view radius

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
        
        // look for food in view radius
        if(!visibleFood.empty()) {
            // move to first visible food
            this->ants[i].move(this->terrainMap, visibleFood[0], this->foodMap);
        } else if(!visiblePheromones.empty()) {
            // move to first visible pheromone
            this->ants[i].move(this->terrainMap, visiblePheromones[0], this->foodMap);
        } else {
            // otherwise, move to random location
            std::uniform_int_distribution<int> rowDist(0, this->terrainMap.size() - 1);
            std::uniform_int_distribution<int> colDist(0, this->terrainMap[0].size() - 1);
            Coord randomDestination = {rowDist(rng), colDist(rng)};
            this->ants[i].move(this->terrainMap, randomDestination, this->foodMap);
        }
        
        // go home if carrying food (CHANGE: if too far from home, wander instead?)
        if (this->ants[i].carryingFood) {
            this->ants[i].returnHome(this->terrainMap, this->foodMap);
            //drops pheromone if about to die
            if (this->ants[i].energy == 0) {
                this->ants[i].dropPheromone(this->pheromoneMap);
            }
        } 
    }
}

/** You may insert any custom functions below **/
