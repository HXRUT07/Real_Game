#include "City.h"

City::City(int r, int c, sf::Vector2f pos)
    : gridR(r), gridC(c), center(pos)
{
    baseIcon.setSize(sf::Vector2f(12.f, 12.f));
    baseIcon.setFillColor(sf::Color::Red);

    // ตั้ง origin ให้ตรงกลางสี่เหลี่ยม
    baseIcon.setOrigin(6.f, 6.f);

    // วางตรง center ของ hex tile ที่คลิก
    baseIcon.setPosition(center);
}

void City::draw(sf::RenderWindow& window)
{
    window.draw(baseIcon);
}