#include "City.h"
#include <sstream>
#include <cmath> // เปรมทำ - สำหรับ cos, sin
#include <algorithm> // เปรมทำ - สำหรับ std::max

//  ลบค่าในวงเล็บให้ตรงกับ City.h
City::City(int r, int c, sf::Vector2f pos)
    : gridR(r), gridC(c), center(pos)
{
    name = "New City";

    // เปรมทำ - โหลดรูปปราสาทและ clip เป็น hex shape
    if (castleTexture.loadFromFile("Blue Castle.png")) {
        float size = 30.0f * 2.0f;

        // เปรมทำ - สร้าง RenderTexture ขนาดเท่า hex
        hexMaskTexture.create((unsigned int)size, (unsigned int)size);
        hexMaskTexture.clear(sf::Color::Transparent);

        // เปรมทำ - วาด hex shape ก่อนเป็น mask
        sf::ConvexShape hexMask;
        hexMask.setPointCount(6);
        for (int i = 0; i < 6; ++i) {
            float angle = 60.f * i - 30.f;
            float rad = angle * 3.14159f / 180.f;
            hexMask.setPoint(i, sf::Vector2f(
                size / 2.f + (size / 2.f) * std::cos(rad),
                size / 2.f + (size / 2.f) * std::sin(rad)
            ));
        }
        hexMask.setFillColor(sf::Color::White);
        hexMaskTexture.draw(hexMask);

        // เปรมทำ - วาดรูปปราสาททับ โดยใช้ BlendMode ให้โชว์เฉพาะในพื้นที่ hex
        sf::Sprite castleSprite(castleTexture);
        sf::Vector2u texSize = castleTexture.getSize();
        castleSprite.setScale(size / texSize.x, size / texSize.y);

        sf::BlendMode blendDestIn(
            sf::BlendMode::DstAlpha,
            sf::BlendMode::Zero,
            sf::BlendMode::Add,
            sf::BlendMode::Zero,
            sf::BlendMode::SrcAlpha,
            sf::BlendMode::Add
        );
        hexMaskTexture.draw(castleSprite, sf::RenderStates(blendDestIn));

        hexMaskTexture.display();

        // เปรมทำ - ใช้ texture ที่ clip แล้ว
        hexMaskedSprite.setTexture(hexMaskTexture.getTexture());
        hexMaskedSprite.setOrigin(size / 2.f, size / 2.f);
    }
    hexMaskedSprite.setPosition(center);
    // เปรมทำ - จบ

    // ---ให้ทรัพยากรตั้งต้นนิดหน่อยตอนตั้งเมือง ---
    stockpile.gold = 100;
    stockpile.wood = 50;
    stockpile.food = 50;
}

void City::draw(sf::RenderWindow& window)
{
    // เปรมทำ - วาด hexMaskedSprite แทน baseIcon
    window.draw(hexMaskedSprite);
    // เปรมทำ - จบ
}

sf::FloatRect City::getBounds() const
{
    // เปรมทำ
    return hexMaskedSprite.getGlobalBounds();
    // เปรมทำ - จบ
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
ResourceYield City::getTotalResource() const
{
    return stockpile;
}