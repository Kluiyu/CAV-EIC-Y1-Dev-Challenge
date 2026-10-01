//
// Created by dusan on 9/15/26.
//

#include "../include/antworld.h"
#include <iostream>
#include <functional>
#include <algorithm>
#include <math.h>
#include <random>
#include <numeric>

//function prototypes
bool isCheckerboard(Coord, int);
bool isPolka(Coord, int);
void flipOffPattern(std::function<bool(Coord, int)>, std::vector<std::vector<int>>&, Ant&);
void flipOnPattern(std::function<bool(Coord, int)>, std::vector<std::vector<int>>&, Ant&);
Coord offPatternPositions(std::function<bool(Coord, int)>, std::vector<std::vector<int>>&, Ant&);
std::vector<Coord> scanSurroundings(Ant&, MapTemplate&);
Coord searchPos(Ant&, std::vector<std::vector<int>>&);
Coord closest(Ant&, std::vector<Coord>);
bool canTakeStep(Ant&, std::vector<std::vector<int>>&, Coord);

/** @brief this is where you as the applicant will make use of the above functions to develop your solution.
 * here are some existing examples of how calling these functions works to help get you started!
 */
void AntWorld::forage() {
    /*
    09/22/26
        ALGOS AND SCORES WITH BASE SEED 12345
        basic wandering algo. score = 9
            -ants move randomly, pick up food and return home when food is spotted
        added search w/pheromones; score = 28
            - ants move randomly initially, if they die with food on them, drop a pheromone marker
            - ants prioritize moving to pheromones and moving home, erasing pheromone if food is picked up

    09/24/26
        checkerboard pheromone strategy; score = 21
            - using pheromones to form a checkerboard pattern, if any tiles spotted that are 'off-pattern', move there
            - if food is found when ant is already carrying, flip tile 'off pattern'
            - if food is picked up from an 'off pattern' tile, flip it 'on pattern'
            - does worse, i assume its cause the ants waste all their energy moving to form the pattern, unecessary

    09/28/26
        sparse pheromone strategy; score = 20
            - similar to previous strategy, switch out checkerboard pattern for having one every so often (based on radius of view)
            - think. probably the pattern doesnt stretch out far enough, so ants are still wandering around aimlessly.
    09/30/26
        add distance calc to finding food. tried reverting back to base pheromone strategy without pattern
        issue with hitting step limit likely due to ants getting stuck, implemented apoptosis to fix lol
        score = 18. still not great ...

    */
   
    //all ants perform same actions
    for (int i = 0; i < this->ants.size(); ++i) {
        //scan for food and pheromones in view radius
        std::vector<Coord> visibleFood = this->ants[i].foodScan(this->foodMap);
        std::vector<Coord> visiblePheromones = this->ants[i].pheromoneScan(this->pheromoneMap);
        std::vector<Coord> visibleSpaces = scanSurroundings(this->ants[i], this->pheromoneMap);
        Coord currentPos = {this->ants[i].position.first, this->ants[i].position.second};
        //drop pheromone marking to create pattern
        flipOnPattern(isPolka, this->pheromoneMap, this->ants[i]);

        //skip loop if ant has no energy left
        if (this->ants[i].energy <= 0) {
            continue;
        }

        //prioritize returning home with food, then looking for food, then exploring.
        // go home if carrying food 
        if (this->ants[i].carryingFood) {
            //marks food seen nearby before heading home (if not already on food)
            if(!visibleFood.empty() && this->foodMap[currentPos.first][currentPos.second] != 1) {
                Coord closestFoodPos = closest(this->ants[i], visibleFood);
                if (canTakeStep(this->ants[i], this->terrainMap, closestFoodPos)) {
                    this->ants[i].move(this->terrainMap, closestFoodPos, this->foodMap);
                    flipOffPattern(isPolka, this->pheromoneMap, this->ants[i]);
                    // this->ants[i].dropPheromone(this->pheromoneMap);
                } else { 
                    // this->ants[i].dropPheromone(this->pheromoneMap);
                    this->ants[i].energy = 0;
                    flipOffPattern(isPolka, this->pheromoneMap, this->ants[i]);
                }
            }

            //if ant can reach home, go home, otherwise drop pheromone and die
            if (canTakeStep(this->ants[i], this->terrainMap, this->ants[i].homeCoord)) {
                this->ants[i].returnHome(this->terrainMap, this->foodMap);
            } else {
                    // this->ants[i].dropPheromone(this->pheromoneMap);
                    flipOffPattern(isPolka, this->pheromoneMap, this->ants[i]);
                    this->ants[i].energy = 0;
            }

        } else if(!visibleFood.empty()) {
            // look for food in view radius (if not carrying food), move there, mark it
            Coord closestFoodPos = closest(this->ants[i], visibleFood);
            if (canTakeStep(this->ants[i], this->terrainMap, closestFoodPos)) {
                    this->ants[i].move(this->terrainMap, closestFoodPos, this->foodMap);
            } else {
                this->ants[i].energy = 0;
            }

        } else if (!visiblePheromones.empty()) { // look for pheromones in view radius, move there
            Coord offPatternPos = offPatternPositions(isPolka, this->pheromoneMap, this->ants[i]);
            // Coord closestPheromonePos = closest(this->ants[i], visiblePheromones);
            if (canTakeStep(this->ants[i], this->terrainMap, offPatternPos)) {
                this->ants[i].move(this->terrainMap, offPatternPos, this->foodMap);
            } else {
                this->ants[i].energy = 0;
            }

            // this->ants[i].move(this->terrainMap, visiblePheromones[0], this->foodMap);
            // flipOffPattern(isPolka, this->pheromoneMap, this->ants[i]);
            
        } else { // otherwise, move to random location on edge of radius
            Coord moveTo = searchPos(this->ants[i], this->pheromoneMap);
            if (canTakeStep(this->ants[i], this->terrainMap, moveTo)) {
                this->ants[i].move(this->terrainMap, moveTo, this->foodMap);
            } else {
                this->ants[i].energy = 0;
            }
            // Coord offPatternPos = offPatternPositions(isPolka, this->pheromoneMap, this->ants[i]);
            // //checking if any off-pattern positions were found, if so move to first one
            // if (offPatternPos.first != -1 && offPatternPos.second != -1) {
            //     this->ants[i].move(this->terrainMap, offPatternPos, this->foodMap);
                
                // yeah random mvmt isnt helping lol okay better movement solution
            
        }
    }
}

/** You may insert any custom functions below **/
//returns all coordinates an ant can see using pheromones
std::vector<Coord> scanSurroundings(Ant &ant, MapTemplate &map) {
    std::vector<Coord> visibleCoords;
    for (int i = ant.position.first - ant.pheromoneRadius; i <= ant.position.first + ant.pheromoneRadius; ++i) {
        for (int j = ant.position.second - ant.pheromoneRadius; j <= ant.position.second + ant.pheromoneRadius; ++j) {
            if (i < 0 || i >= map.size() || j < 0 || j >= map[0].size()) {
                continue;
            } else {
                visibleCoords.push_back({i, j});
            }
        }
    }
    return visibleCoords;
}

//pattern functions
//sees if position coordinate is on a 'checkerboard' pattern (ex. (1,1), (1,3), (2,2).. etc)
bool isCheckerboard(Coord pos, int unused = 0) {
    return (pos.first ^ pos.second) & 1;
}

//checks if position is on big grid of dots (ex. (0,0), (0,5), (5,0), (5,5).. etc)
bool isPolka(Coord pos, int radius) {
    return pos.first % radius == 0 && pos.second % radius == 0;
}

//breaks pattern
void flipOffPattern(std::function<bool(Coord, int)> isPattern, std::vector<std::vector<int>> &pheromoneMap, Ant &ant) {
    if (isPattern(ant.position, ant.pheromoneRadius)) {
        ant.erasePheromone(pheromoneMap);
    } else {
        ant.dropPheromone(pheromoneMap);
    }
}

//makes pattern
void flipOnPattern(std::function<bool(Coord, int)> isPattern, std::vector<std::vector<int>> &pheromoneMap, Ant &ant) {
    if (!isPattern(ant.position, ant.pheromoneRadius)) {
        ant.erasePheromone(pheromoneMap);
    } else {
        ant.dropPheromone(pheromoneMap);
    }
}

//if the position is on the checkerboard, should have pheromone on it, if not should be empty.
//if above is false, move to first of that position found!
Coord offPatternPositions(
    std::function<bool(Coord, int)> isPattern,
    std::vector<std::vector<int> > &pheromoneMap, 
    Ant &ant) {
    std::vector<Coord> positions = scanSurroundings(ant, pheromoneMap);
    for (const auto& pos : positions) {
        if (!isPattern(pos, ant.pheromoneRadius)) {
            positions.push_back(pos);
        } 
    }
    if (!positions.empty()) {
        return closest(ant, positions);
    }
    //nothing found
    return {-1, -1};
}

int distFromHome(Coord pos, Coord home){
    return sqrt(pow(pos.first-home.first, 2) + pow(pos.second-home.second, 2));
}

//search movement
Coord searchPos(Ant &ant, std::vector<std::vector<int> > &pheromoneMap){
    Coord pos = ant.position; //initial position
    Coord home = ant.homeCoord; //home position

    //prefer locations further from home coordinates
    std::vector<Coord> candidates = scanSurroundings(ant, pheromoneMap);
    std::sort(candidates.begin(), candidates.end(), [home](Coord a, Coord b){
        return (distFromHome(a, home) > distFromHome(b, home)); //sorted descending
    });

    //remove current location
    candidates.erase(std::remove(candidates.begin(), candidates.end(), pos), candidates.end());

    //keep top 1/4
    int popSize = candidates.size()/4;
    int initialSize = candidates.size();
    for (int i=0; i<(initialSize-popSize); i++){
        candidates.pop_back();
    }
    
    //add some randomness so ants behave differently 
    std::random_device rand;
    std::mt19937 rng(rand());
    std::uniform_int_distribution<int> dist(0, candidates.size() - 1);

    return candidates[dist(rng)];
}

//returns index of closest food/pheromon in coords vector, -1 if none found
Coord closest(Ant &ant, std::vector<Coord> coords){
    Coord closestPos = {-1, -1};
    int minDist = INT_MAX;
    for (int i=0; i<coords.size(); i++){
        Coord food = coords.at(i);
        int dist = sqrt(pow(food.first - ant.position.first, 2) + pow(food.second - ant.position.second, 2));
        if (dist < minDist) {
            minDist = dist;
            closestPos = food;
        }
    }
    return closestPos;
}

//returns true if ant can take a step towards destination with current energy, false otherwise
bool canTakeStep(Ant& ant, std::vector<std::vector<int>>& terrainMap, Coord dest) {
    std::vector<Coord> path = shortestPath(terrainMap, ant.position, dest);

    if (path.size() <= 1) {
        return false;
    }

    auto [r1, c1] = path[0];
    auto [r2, c2] = path[1];

    int cost = 1 + std::abs(terrainMap[r1][c1] - terrainMap[r2][c2]);
    return ant.energy >= cost;
}
