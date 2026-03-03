#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "ResourceManage.h"   

class City {
private:
    int gridR, gridC;
    sf::Vector2f center;
    sf::RectangleShape baseIcon;

    std::string name;
    ResourceYield baseResource;
    bool upgraded = false;

public:
    City(int r, int c, sf::Vector2f pos, ResourceYield res);

    int getR() const { return gridR; }
    int getC() const { return gridC; }

    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    ResourceYield getTotalResource() const;

    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    std::string getCityInfo() const;
};