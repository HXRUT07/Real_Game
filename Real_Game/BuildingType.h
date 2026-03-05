#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class BuildingType {
    Village,       // หมู่บ้าน -> ผลิต Gold
    Barracks,      // ค่ายทหาร -> เทรนทหาร
    Restaurant,    // ร้านอาหาร -> ผลิต Food
    Lumbermill     // โรงไม้ -> ผลิต Wood
};

struct BuildingCost {
    int gold, wood, food;
};

struct Building {
    BuildingType type;
    int count = 0; // จำนวนที่สร้างแล้ว (ใช้คำนวณราคาถัดไป)

    std::string getName() const {
        switch (type) {
        case BuildingType::Village:    return "Village";
        case BuildingType::Barracks:   return "Barracks";
        case BuildingType::Restaurant: return "Restaurant";
        case BuildingType::Lumbermill: return "Lumbermill";
        }
        return "";
    }

    std::string getDescription() const {
        switch (type) {
        case BuildingType::Village:    return "+10 Gold / turn";
        case BuildingType::Barracks:   return "Train soldiers";
        case BuildingType::Restaurant: return "+10 Food / turn";
        case BuildingType::Lumbermill: return "+10 Wood / turn";
        }
        return "";
    }

    // ค่าสร้างพื้นฐาน x (1 + จำนวนที่มีอยู่แล้ว)
    BuildingCost getCost() const {
        int n = count + 1; // ราคาของอันถัดไป
        switch (type) {
        case BuildingType::Village:
            return { 50 * n, 30 * n, 20 * n };
        case BuildingType::Barracks:
            return { 80 * n, 60 * n, 40 * n };
        case BuildingType::Restaurant:
            return { 40 * n, 20 * n, 30 * n };
        case BuildingType::Lumbermill:
            return { 40 * n, 10 * n, 20 * n };
        }
        return { 0, 0, 0 };
    }

    // ผลผลิตต่อเทิร์น (Barracks ไม่ผลิตอะไร)
    int goldPerTurn() const { return type == BuildingType::Village ? 10 * count : 0; }
    int foodPerTurn() const { return type == BuildingType::Restaurant ? 10 * count : 0; }
    int woodPerTurn() const { return type == BuildingType::Lumbermill ? 10 * count : 0; }
};
