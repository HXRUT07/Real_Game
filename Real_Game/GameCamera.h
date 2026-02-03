#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include <SFML/Graphics.hpp>

class GameCamera {
public:
    // Constructor รับขนาดหน้าจอเริ่มต้น
    GameCamera(float width, float height);

    // ฟังก์ชันจัดการ Input (กดปุ่ม, เลื่อนลูกกลิ้ง)
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);

    // ฟังก์ชันอัปเดตสถานะกล้อง (เรียกทุกเฟรม)
    void update(const sf::RenderWindow& window);

    // คืนค่า View ปัจจุบันเพื่อนำไป set ให้ window
    const sf::View& getView() const;

private:
    sf::View m_view;

    // ตัวแปรสำหรับระบบ Panning
    bool m_isPanning;
    sf::Vector2f m_dragOrigin; // จุดเริ่มลาก (พิกัด World)

    // การตั้งค่า Zoom (แถมให้ เพราะเกม RTS ต้องมี)
    float m_currentZoom;
};

#endif