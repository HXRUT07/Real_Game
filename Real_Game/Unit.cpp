#include "Unit.h"
#include <cmath>
#include <iostream>

const float HEX_SIZE_UNIT = 30.0f;
const float UNIT_RADIUS = 18.f;  // รัศมีวงกลมทหาร

sf::Texture Unit::s_texKnight;
sf::Texture Unit::s_texArmy;
bool Unit::s_texturesLoaded = false;

bool Unit::loadTextures() {
    if (s_texturesLoaded) return true;

    bool ok = true;

    if (!s_texKnight.loadFromFile("blueknight.png")) {
        std::cout << "ERROR: blueknight.png not found!\n";
        ok = false;
    }
    else {
        // ลบพื้นหลังสีมุมบนซ้าย
        sf::Image img = s_texKnight.copyToImage();
        sf::Color bg = img.getPixel(0, 0);
        img.createMaskFromColor(bg, 0);
        s_texKnight.loadFromImage(img);
        s_texKnight.setSmooth(true);
        std::cout << "blueknight.png loaded!\n";
    }

    if (!s_texArmy.loadFromFile("army.png")) {
        std::cout << "ERROR: army.png not found!\n";
        ok = false;
    }
    else {
        s_texArmy.setSmooth(true);
        std::cout << "army.png loaded!\n";
    }

    s_texturesLoaded = ok;
    return ok;
}

Unit::Unit(std::string name, int startR, int startC, int owner) {
    m_name = name;
    m_gridR = startR;
    m_gridC = startC;
    m_owner = owner;

    m_maxAP = 2;
    m_currentAP = m_maxAP;
    m_moveRange = 1;
    m_sightRange = 1;

    // เตรียม circle shape
    m_circle.setRadius(UNIT_RADIUS);
    m_circle.setOrigin(UNIT_RADIUS, UNIT_RADIUS);
    m_circle.setOutlineThickness(2.f);

    if (m_owner == 1) {
        loadTextures();
    }

    updateScreenPosition();
}

void Unit::moveTo(int r, int c) {
    m_gridR = r;
    m_gridC = c;
    updateScreenPosition();
}

void Unit::draw(sf::RenderWindow& window, int stackCount) {
    if (m_owner == 1) {
        if (!s_texturesLoaded) {
            // fallback วงกลมน้ำเงิน
            m_circle.setTexture(nullptr);
            m_circle.setFillColor(m_currentAP > 0
                ? sf::Color(50, 100, 255)
                : sf::Color(0, 0, 150));
            m_circle.setOutlineColor(sf::Color::White);
            window.draw(m_circle);
            return;
        }

        if (stackCount >= 2) {
            // กองทัพ - army.png ครอบในวงกลม
            m_circle.setTexture(&s_texArmy, true);
            sf::Vector2u texSize = s_texArmy.getSize();
            if (texSize.x > 0) {
                // ขยาย texture ให้เต็มวงกลม
                float diameter = UNIT_RADIUS * 2.f;
                // ใช้ด้านที่เล็กกว่าเพื่อ crop ให้พอดีวงกลม
                float scale = diameter / std::min((float)texSize.x, (float)texSize.y);
                // offset texture ให้อยู่ตรงกลาง
                int offsetX = (int)((texSize.x - diameter / scale) / 2.f);
                int offsetY = (int)((texSize.y - diameter / scale) / 2.f);
                int cropSize = (int)(diameter / scale);
                m_circle.setTextureRect(sf::IntRect(offsetX, offsetY, cropSize, cropSize));
            }
        }
        else {
            // ทหารคนเดียว - blueknight.png ครอบในวงกลม
            m_circle.setTexture(&s_texKnight, true);
            sf::Vector2u texSize = s_texKnight.getSize();
            if (texSize.x > 0) {
                float diameter = UNIT_RADIUS * 2.f;
                float scale = diameter / std::min((float)texSize.x, (float)texSize.y);
                int offsetX = (int)((texSize.x - diameter / scale) / 2.f);
                int offsetY = (int)((texSize.y - diameter / scale) / 2.f);
                int cropSize = (int)(diameter / scale);
                m_circle.setTextureRect(sf::IntRect(offsetX, offsetY, cropSize, cropSize));
            }
        }

        m_circle.setFillColor(m_currentAP > 0
            ? sf::Color(255, 255, 255, 255)
            : sf::Color(180, 180, 180, 180));
        m_circle.setOutlineColor(m_currentAP > 0
            ? sf::Color(255, 215, 0)   // ขอบทองตอนมี AP
            : sf::Color(100, 100, 100)); // ขอบเทาตอนหมด AP

        window.draw(m_circle);
    }
    else {
        // ศัตรู วงกลมแดง
        m_circle.setTexture(nullptr);
        m_circle.setFillColor(m_currentAP > 0
            ? sf::Color(200, 30, 30)
            : sf::Color(100, 0, 0));
        m_circle.setOutlineColor(sf::Color::White);
        window.draw(m_circle);
    }
}

bool Unit::isClicked(sf::Vector2f mousePos) {
    float dx = mousePos.x - m_pos.x;
    float dy = mousePos.y - m_pos.y;
    return (dx * dx + dy * dy) <= (UNIT_RADIUS * UNIT_RADIUS);
}

void Unit::updateScreenPosition() {
    float width = std::sqrt(3.0f) * HEX_SIZE_UNIT;
    float height = 2.0f * HEX_SIZE_UNIT;
    float horizDist = width;
    float vertDist = height * 0.75f;

    float x = m_gridC * horizDist + 50.0f;
    float y = m_gridR * vertDist + 50.0f;
    if (m_gridR % 2 != 0) x += width / 2.0f;

    m_pos = sf::Vector2f(x, y);
    m_circle.setPosition(x, y);
}