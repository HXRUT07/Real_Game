#pragma once
#include <vector>
#include "Unit.h"
#include "City.h"

class UpkeepManager {
public:
    // หักอาหารผู้เล่น (ดึงจาก City)
    static void processPlayerUpkeep(std::vector<Unit>& units, City* playerCity);

    // หักอาหาร AI (ดึงจากตัวแปร aiFood ใน main)
    static void processAIUpkeep(std::vector<Unit>& units, int& aiFood);
};