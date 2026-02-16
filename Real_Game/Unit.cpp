#include "Unit.h"
#include <cmath>

// ต้องตรงกับ GameMap
const float HEX_SIZE_UNIT = 30.0f;

// รับ name เข้ามาด้วย
Unit::Unit(std::string name, int startR, int startC) {
    m_name = name; // เก็บชื่อยูนิต
    m_gridR = startR;
    m_gridC = startC;

    // ค่าสถานะเริ่มต้น
    m_maxAP = 2;
    m_currentAP = m_maxAP;
    m_moveRange = 3;

    // สร้างตัวทหาร (วงกลม)
    m_shape.setRadius(15.0f);
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOrigin(15.0f, 15.0f); // จุดหมุนอยู่ตรงกลาง

    updateScreenPosition();
}

void Unit::moveTo(int r, int c) {
    m_gridR = r;
    m_gridC = c;
    updateScreenPosition();
}

void Unit::draw(sf::RenderWindow& window) {
    // Logic เปลี่ยนสีตาม AP
    // ถ้ายังมีแรงเดิน -> สีแดง
    if (m_currentAP > 0) {
        m_shape.setFillColor(sf::Color::Red);
    }
    // ถ้าหมดแรง -> สีฟ้า (Cyan)
    else {
        m_shape.setFillColor(sf::Color::Cyan);
    }

    window.draw(m_shape);
}

bool Unit::isClicked(sf::Vector2f mousePos) {
    return m_shape.getGlobalBounds().contains(mousePos);
}

void Unit::updateScreenPosition() {
    // สูตรเดียวกับ GameMap เพื่อแปลง Grid -> Pixel
    float width = sqrt(3.0f) * HEX_SIZE_UNIT;
    float height = 2.0f * HEX_SIZE_UNIT;
    float horizDist = width;
    float vertDist = height * 0.75f;

    float x = m_gridC * horizDist + 50.0f;
    float y = m_gridR * vertDist + 50.0f;
    if (m_gridR % 2 != 0) x += width / 2.0f;

    m_shape.setPosition(x, y);
}