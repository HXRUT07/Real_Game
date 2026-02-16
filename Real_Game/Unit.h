#pragma once
#include <SFML/Graphics.hpp>

class Unit {
public:
    // Constructor
    Unit(int startR, int startC);

    // วาดตัวละคร
    void draw(sf::RenderWindow& window);

    // การเคลื่อนที่
    void moveTo(int r, int c);

    // ระบบ Action Point (เตรียมไว้)
    bool hasAP() const { return m_currentAP > 0; }
    void consumeAP(int cost) { m_currentAP -= cost; }
    void resetAP() { m_currentAP = m_maxAP; }
    int getMoveRange() const { return m_moveRange; }

    // Getter
    int getR() const { return m_gridR; }
    int getC() const { return m_gridC; }
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }

    // เช็คว่าเมาส์คลิกโดนตัวไหม
    bool isClicked(sf::Vector2f mousePos);

private:
    sf::CircleShape m_shape; // ใช้วงกลมแทนทหารไปก่อน
    int m_gridR, m_gridC;

    // Stats
    int m_maxAP = 2;       // เดินได้ 2 ครั้งต่อเทิร์น (สมมติ)
    int m_currentAP = 2;
    int m_moveRange = 3;   // รัศมีเดิน 3 ช่อง

    void updateScreenPosition();
};