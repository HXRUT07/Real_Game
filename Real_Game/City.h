#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "BuildingType.h"

class City {
private:
    int gridR, gridC;
    sf::Vector2f worldPos;
    std::vector<Building> buildings;

public:
    City(int r, int c, sf::Vector2f pos);

    void addBuilding(BuildingType type);
    void draw(sf::RenderWindow& window);
    std::string getCityInfo();
};
