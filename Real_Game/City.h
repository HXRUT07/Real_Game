#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "BuildingType.h"

class City {
private:
    int gridR, gridC;
    sf::Vector2f center;
    sf::RectangleShape baseIcon;   // สี่เหลี่ยมแทนฐาน

public:
    City(int r, int c, sf::Vector2f pos);
    void draw(sf::RenderWindow& window);
};
