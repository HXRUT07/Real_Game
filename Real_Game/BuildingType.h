#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class BuildingType {
    None,
    Sawmill,     // โรงไม้
    Workshop,   // ค้อน (ตีเหล็ก)
    ToolShop,   // เลื่อย
    Barracks    // เต็นท์ทหาร
};

struct Building {
    BuildingType type = BuildingType::None;
    sf::RectangleShape shape;

    int woodCost;
    int stoneCost;
    int goldCost;

    std::string name;

    Building() {}
};
