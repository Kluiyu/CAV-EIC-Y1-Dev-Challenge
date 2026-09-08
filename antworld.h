#ifndef DEV_CHALLENGE_ANTWORLD_H
#define DEV_CHALLENGE_ANTWORLD_H

#include <vector>
#include  <random>
#include "utility_functions.h"
//
// Created by dusan on 9/4/26.
//
typedef std::vector<std::vector<int> > MapTemplate;

class WorldMap {
public:
    WorldMap(int mapSize_x, int mapSize_y);

    MapTemplate terrainMap;
    MapTemplate itemMap;
    MapTemplate pheromoneMap;

    MapTemplate generateWorldMap(int mapSize_x, int mapSize_y);

    MapTemplate spreadFood(int mapSize_x, int mapSize_y, int foodCount);

    bool hasFood();
};

class Ant {
public:
    Ant(int initEnergy, Coord initCoordinates);

    std::vector<Coord> search(MapTemplate &itemMap);

    Coord move(MapTemplate terrainMap, Coord dest);

    void dropPheromone(MapTemplate &itemMap);

    void erasePheromone(MapTemplate &pheromoneMap);

    Coord returnHome(MapTemplate &terrainMap);

    int viewRadius;
    Coord position;
    int energy;
    bool pheromoneDropped;
    Coord pheromonePosition;
    Coord homeCoord;
};

class AntColony {
public:
    AntColony(int antCount, int mapSize_x, int mapSize_y);

    void forage();

    bool isEmpty();

    std::vector<Ant> ants;
    Coord homeCoordinates;
    int score;

    WorldMap *world;
};

class AntWorld {
public:
    AntWorld(int mapSize_x = 15, int mapSize_y = 15, int antCount = 20);

    bool step();

    bool isGameOver();

    AntColony colony;
};


#endif //DEV_CHALLENGE_ANTWORLD_H
