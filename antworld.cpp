#include "antworld.h"
#include <vector>
#include <random>
#include <algorithm>

//
// Created by dusan on 9/4/26.
//

Ant::Ant(int initEnergy, Coord initCoordinates) {
    this->energy;
    this->position = initCoordinates;
    this->homeCoord = initCoordinates;
}

std::vector<Coord> Ant::search(MapTemplate &itemMap) {
    std::vector<Coord> foodLocations = {};
    for (int i = this->position.first - this->viewRadius; i <= this->position.first + this->viewRadius; ++i) {
        for (int j = this->position.second - this->viewRadius; j <= this->position.second + this->viewRadius; ++j) {
            if (i < 0 || i > itemMap.size() || j < 0 || j > itemMap[0].size()) {
                continue;
            } else {
                if (itemMap[i][j] == 1) {
                    foodLocations.emplace_back(i, j);
                }
            }
        }
    }
    return foodLocations;
}


Coord Ant::move(MapTemplate terrainMap, Coord dest) {
    std::vector<Coord> path = shortestPath(terrainMap, this->position, dest);
    calculatePathCost(terrainMap, path);

    for (int i = 1; i < path.size(); ++i) {
        auto [r1, c1] = path[i - 1];
        auto [r2, c2] = path[i];

        int cost = std::abs(terrainMap[r1][c1] - terrainMap[r2][c2]);
        if (cost > energy) {
            this->position = path[i - 1];
            break;
        }
    }

    return this->position;
}

void Ant::dropPheromone(MapTemplate &pheromoneMap) {
    if (this->pheromoneDropped) {
        erasePheromone(pheromoneMap);
    }

    pheromoneMap[this->position.first][this->position.second] = 1;
    this->pheromoneDropped = true;
}

void Ant::erasePheromone(MapTemplate &pheromoneMap) {
    if (this->pheromoneDropped) {
        pheromoneMap[this->pheromonePosition.first][this->pheromonePosition.second] = 0;
        pheromoneDropped = false;
    }
}

Coord Ant::returnHome(MapTemplate &terrainMap) {
    return this->move(terrainMap, this->homeCoord);
}

AntColony::AntColony(int antCount, int mapSize_x, int mapSize_y) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<int> rowDist(0, mapSize_x - 1);
    std::uniform_int_distribution<int> colDist(0, mapSize_y - 1);

    this->homeCoordinates = {rowDist(gen), colDist(gen)};


    for (int i = 0; i < antCount; ++i) {
        int initialEnergy = std::uniform_int_distribution<int>(int(mapSize_x * mapSize_y * 0.3),
                                                               int(mapSize_x * mapSize_y * 0.6))(gen);
        this->ants.emplace_back(initialEnergy, this->homeCoordinates);
    }

    this->score = 0;

    this->world = new WorldMap(mapSize_x, mapSize_y);
}

void AntColony::forage() {
    // this is where students will write their code

    //useable functions;

    // TIP will return a submap of the ant's visible area

    std::vector<Coord> visibleFood = this->ants[0].search(this->world->itemMap);

    Coord desiredDestination;
    Coord finalPos = this->ants[0].move(this->world->terrainMap, desiredDestination);

    this->ants[0].dropPheromone(this->world->pheromoneMap);

    this->ants[0].erasePheromone(this->world->pheromoneMap);

    this->ants[0].returnHome(this->world->terrainMap);
}

bool AntColony::isEmpty() {
    return this->ants.empty();
}

WorldMap::WorldMap(int mapSize_x, int mapSize_y) {
    this->terrainMap = generateWorldMap(mapSize_x, mapSize_y);
    // come back to this
    int foodCount = int(mapSize_x * mapSize_y / 9.0 * 0.6);
    this->itemMap = spreadFood(mapSize_x, mapSize_y, foodCount);
    this->pheromoneMap = MapTemplate(mapSize_x, std::vector<int>(mapSize_y, 0));
}

MapTemplate WorldMap::generateWorldMap(int mapSize_x, int mapSize_y) {
    std::random_device rd;
    std::mt19937 gen(rd());

    MapTemplate map(mapSize_x, std::vector<int>(mapSize_y));

    for (int row = 0; row < mapSize_x; ++row) {
        for (int col = 0; col < mapSize_y; ++col) {
            int minValue = 0;
            int maxValue = 2;

            // Left
            if (col > 0) {
                minValue = std::max(minValue, map[row][col - 1] - 1);
                maxValue = std::min(maxValue, map[row][col - 1] + 1);
            }

            // Top-left
            if (row > 0 && col > 0) {
                minValue = std::max(minValue, map[row - 1][col - 1] - 1);
                maxValue = std::min(maxValue, map[row - 1][col - 1] + 1);
            }

            // Top
            if (row > 0) {
                minValue = std::max(minValue, map[row - 1][col] - 1);
                maxValue = std::min(maxValue, map[row - 1][col] + 1);
            }

            // Top-right
            if (row > 0 && col < mapSize_y - 1) {
                minValue = std::max(minValue, map[row - 1][col + 1] - 1);
                maxValue = std::min(maxValue, map[row - 1][col + 1] + 1);
            }

            std::uniform_int_distribution<int> dist(minValue, maxValue);
            map[row][col] = dist(gen);
        }
    }

    return map;
}

MapTemplate WorldMap::spreadFood(int mapSize_x, int mapSize_y, int foodCount) {
    {
        std::vector<std::vector<int> > map(mapSize_x, std::vector<int>(mapSize_y, 0));

        // Generate all possible cell indices
        std::vector<int> indices(mapSize_x * mapSize_y);
        std::iota(indices.begin(), indices.end(), 0);

        // Randomize their order
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(indices.begin(), indices.end(), gen);

        // Set the first x randomly selected cells to 1
        for (int i = 0; i < foodCount; ++i) {
            int row = indices[i] / mapSize_y;
            int col = indices[i] % mapSize_y;

            map[row][col] = 1;
        }

        return map;
    }
}

bool WorldMap::hasFood() {
    for (const auto &row: this->itemMap) {
        if (std::find(row.begin(), row.end(), 1) != row.end()) {
            return true;
        }
    }

    return false;
}

AntWorld::AntWorld(int mapSize_x, int mapSize_y, int antCount) : colony(antCount, mapSize_x, mapSize_y) {
}

bool AntWorld::step() {
    this->colony.forage();


    return this->isGameOver();
}

bool AntWorld::isGameOver() {
    if (this->colony.isEmpty()) {
        return true;
    }

    if (not this->colony.world->hasFood()) {
        return true;
    }

    return false;
}
