#include "City.h"
#include <sstream>
#include <algorithm> // เปรมทำ - สำหรับ std::max

//  ลบค่าในวงเล็บให้ตรงกับ City.h
City::City(int r, int c, sf::Vector2f pos)
    : gridR(r), gridC(c), center(pos)
{
    name = "New City";

    // เปรมทำ - โหลดรูปปราสาทแทนสี่เหลี่ยมสีเหลือง
    if (castleTexture.loadFromFile("Blue Castle.png")) {
        baseIcon.setTexture(castleTexture);
        sf::Vector2u texSize = castleTexture.getSize();
        float scale = 28.f / std::max(texSize.x, texSize.y);
        baseIcon.setScale(scale, scale);
        baseIcon.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
    }
    baseIcon.setPosition(center);
    // เปรมทำ - จบ

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