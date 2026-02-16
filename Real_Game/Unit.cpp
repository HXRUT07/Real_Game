#include "Unit.h"
#include <cmath>

// ต้องตรงกับ GameMap (ถ้าแยกไฟล์ config ได้จะดีมาก แต่ตอนนี้ Hardcode ไปก่อน)
const float HEX_SIZE_UNIT = 30.0f;

Unit::Unit(int startR, int startC) {
    m_gridR = startR;
    m_gridC = startC;
    m_maxAP = 2;
    m_currentAP = m_maxAP;
    m_moveRange = 3; // เดินได้ไกล 3 ช่อง

    // สร้างตัวทหาร (วงกลมสีแดง)
    m_shape.setRadius(15.0f);
    m_shape.setFillColor(sf::Color::Red);
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