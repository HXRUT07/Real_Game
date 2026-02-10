#include "City.h"

City::City(int r, int c) : gridR(r), gridC(c) {}

void City::addBuilding(BuildingType type)
{
    Building b;
    b.type = type;

    if (type == BuildingType::Sawmill) {
        b.name = "Sawmill";
        b.woodCost = 2;
        b.stoneCost = 1;
        b.goldCost = 0;
        b.shape.setFillColor(sf::Color(139, 69, 19)); // น้ำตาล
    }
    else if (type == BuildingType::Workshop) {
        b.name = "Workshop";
        b.woodCost = 1;
        b.stoneCost = 2;
        b.goldCost = 1;
        b.shape.setFillColor(sf::Color::Red);
    }
    else if (type == BuildingType::ToolShop) {
        b.name = "Tool Shop";
        b.woodCost = 2;
        b.stoneCost = 2;
        b.goldCost = 1;
        b.shape.setFillColor(sf::Color::Yellow);
    }
    else if (type == BuildingType::Barracks) {
        b.name = "Barracks";
        b.woodCost = 3;
        b.stoneCost = 2;
        b.goldCost = 2;
        b.shape.setFillColor(sf::Color::Blue);
    }

    b.shape.setSize(sf::Vector2f(15, 15));
    buildings.push_back(b);
}

void City::draw(sf::RenderWindow& window)
{
    for (auto& b : buildings)
        window.draw(b.shape);
}

std::string City::getCityInfo()
{
    std::string info = "City Buildings:\n";
    for (auto& b : buildings)
        info += "- " + b.name + "\n";
    return info;
}
