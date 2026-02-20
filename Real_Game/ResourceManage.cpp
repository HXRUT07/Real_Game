#include "ResourceManage.h"
#include <cstdlib> // สำหรับ std::rand()

ResourceYield ResourceManage::generateResources(TerrainType type) {
    ResourceYield yield;

    switch (type) {
    case TerrainType::Mountain:
        yield.gold = randomRange(40, 80);
        yield.wood = randomRange(5, 15);
        yield.food = randomRange(10, 25);
        break;
    case TerrainType::Forest:
        yield.gold = randomRange(5, 15);
        yield.wood = randomRange(40, 80);
        yield.food = randomRange(20, 40);
        break;
    case TerrainType::Grass:
        yield.gold = randomRange(10, 25);
        yield.wood = randomRange(10, 25);
        yield.food = randomRange(40, 80);
        break;
    case TerrainType::Water:
        yield.gold = 0;
        yield.wood = 0;
        yield.food = randomRange(20, 50);
        break;
    }

    return yield;
}

int ResourceManage::randomRange(int min, int max) {
    if (min >= max) return min;
    return min + (std::rand() % (max - min + 1));
}