#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class BuildingType {
    None,
    Sawmill,
    Workshop,
    ToolShop,
    Barracks
};

struct Building {
    BuildingType type = BuildingType::None;
    sf::RectangleShape shape;

    int woodCost = 0;
    int stoneCost = 0;
    int goldCost = 0;

    std::string name = "";

    Building() {}
};
