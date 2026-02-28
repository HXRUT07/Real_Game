#include "City.h"
#include <sstream>

//  ลบค่าในวงเล็บให้ตรงกับ City.h
City::City(int r, int c, sf::Vector2f pos)
    : gridR(r), gridC(c), center(pos)
{
    name = "New City";

    baseIcon.setSize(sf::Vector2f(12.f, 12.f));
    baseIcon.setFillColor(sf::Color::Yellow);
    baseIcon.setOrigin(6.f, 6.f);
    baseIcon.setPosition(center);

    // ---ให้ทรัพยากรตั้งต้นนิดหน่อยตอนตั้งเมือง ---
    stockpile.gold = 100;
    stockpile.wood = 50;
    stockpile.food = 50;
}

void City::draw(sf::RenderWindow& window)
{
    window.draw(baseIcon);
}

sf::FloatRect City::getBounds() const
{
    return baseIcon.getGlobalBounds();
}

std::string City::getCityInfo() const
{
    std::stringstream ss;

    ss << "City Name: " << name << "\n\n";
    // เปลี่ยนมาโชว์ของในคลังหลวง (stockpile) 
    ss << "Wood: " << stockpile.wood << "\n";
    ss << "Gold: " << stockpile.gold << "\n";
    ss << "Food: " << stockpile.food << "\n\n";
    ss << "Upgraded: " << (upgraded ? "Yes" : "No");

    return ss.str();
}