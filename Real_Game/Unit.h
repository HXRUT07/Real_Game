#pragma once
#include <SFML/Graphics.hpp>
#include <string> // <--- [สำคัญ] อย่าลืม include string

class Unit {
public:
    // Constructor: รับชื่อเพิ่มเข้ามา
    Unit(std::string name, int startR, int startC);

    // วาดตัวละคร
    void draw(sf::RenderWindow& window);

    // การเคลื่อนที่
    void moveTo(int r, int c);

    // ระบบ Action Point
    bool hasAP() const { return m_currentAP > 0; }
    void consumeAP(int cost) { m_currentAP -= cost; }
    void resetAP() { m_currentAP = m_maxAP; }
    int getMoveRange() const { return m_moveRange; }

    // Getter พื้นฐาน
    int getR() const { return m_gridR; }
    int getC() const { return m_gridC; }
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }

    // Getter สำหรับ UI (เอาไว้โชว์ชื่อและ AP ในแถบขวา)
    std::string getName() const { return m_name; }
    int getCurrentAP() const { return m_currentAP; }
    int getMaxAP() const { return m_maxAP; }

    // เช็คว่าเมาส์คลิกโดนตัวไหม
    bool isClicked(sf::Vector2f mousePos);

private:
    sf::CircleShape m_shape;

    std::string m_name; // ตัวแปรเก็บชื่อยูนิต
    int m_gridR, m_gridC;

    // Stats
    int m_maxAP = 2;       // เดินได้ 2 ครั้งต่อเทิร์น
    int m_currentAP = 2;
    int m_moveRange = 3;   // รัศมีเดิน 3 ช่อง

    void updateScreenPosition();
};