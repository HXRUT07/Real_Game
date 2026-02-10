#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "BuildingType.h"

class City {
private:
    int gridR, gridC; // เมืองอยู่ที่ช่องไหน
    std::vector<Building> buildings;

public:
    City(int r, int c);

    void addBuilding(BuildingType type);
    void draw(sf::RenderWindow& window);

    std::string getCityInfo();
};
