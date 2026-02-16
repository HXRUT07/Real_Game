#include "ResourceManage.h"#include "ResourceSystem.h"
#include <cstdlib> // สำหรับ rand()

int ResourceSystem::randomRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

ResourceYield ResourceSystem::generateResources(TerrainType type) {
    ResourceYield loot;

    // --- กำหนด Logic ทรัพยากรตามโจทย์ ---
    switch (type) {
    case TerrainType::Forest:
        // ป่า: ไม้เยอะ (Wood High)
        loot.wood = randomRange(20, 40);
        loot.gold = randomRange(0, 5);
        loot.food = randomRange(5, 10);
        break;

    case TerrainType::Mountain:
        // ภูเขา: ทองเยอะ (Gold High)
        loot.wood = randomRange(0, 5);
        loot.gold = randomRange(15, 30);
        loot.food = randomRange(0, 5);
        break;

    case TerrainType::Water:
        // แม่น้ำ: อาหารเยอะ (Food High)
        loot.wood = randomRange(0, 5);
        loot.gold = randomRange(0, 5);
        loot.food = randomRange(20, 40);
        break;

    case TerrainType::Grass:
        // ทุ่งหญ้า: มาตรฐาน (Standard/Balanced)
        loot.wood = randomRange(5, 15);
        loot.gold = randomRange(5, 15);
        loot.food = randomRange(5, 15);
        break;

    default:
        // กันเหนียว
        loot.wood = 1; loot.gold = 1; loot.food = 1;
        break;
    }

    return loot;
}
#include <cstdlib> // สำหรับ rand()
                                                                                                                                                                                                        
int ResourceSystem::randomRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

ResourceYield ResourceSystem::generateResources(TerrainType type) {
    ResourceYield loot;

    // --- กำหนด Logic ทรัพยากรตามโจทย์ ---
    switch (type) {
    case TerrainType::Forest:
        // ป่า: ไม้เยอะ (Wood High)
        loot.wood = randomRange(20, 40);
        loot.gold = randomRange(0, 5);
        loot.food = randomRange(5, 10);
        break;

    case TerrainType::Mountain:
        // ภูเขา: ทองเยอะ (Gold High)
        loot.wood = randomRange(0, 5);
        loot.gold = randomRange(15, 30);
        loot.food = randomRange(0, 5);
        break;

    case TerrainType::Water:
        // แม่น้ำ: อาหารเยอะ (Food High)
        loot.wood = randomRange(0, 5);
        loot.gold = randomRange(0, 5);
        loot.food = randomRange(20, 40);
        break;

    case TerrainType::Grass:
        // ทุ่งหญ้า: มาตรฐาน (Standard/Balanced)
        loot.wood = randomRange(5, 15);
        loot.gold = randomRange(5, 15);
        loot.food = randomRange(5, 15);
        break;

    default:
        // กันเหนียว
        loot.wood = 1; loot.gold = 1; loot.food = 1;
        break;
    }

    return loot;
}