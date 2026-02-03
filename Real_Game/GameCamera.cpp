#include "GameCamera.h"

GameCamera::GameCamera(float width, float height)
    : m_isPanning(false), m_currentZoom(1.0f)
{
    // กำหนดขนาดกล้องเริ่มต้นเท่ากับขนาดหน้าจอ
    m_view.setSize(width, height);
    m_view.setCenter(width / 2.0f, height / 2.0f);
}

void GameCamera::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    // 1. ตรวจจับการกดปุ่มเพื่อเริ่มลาก (ใช้คลิกขวา หรือ ปุ่มกลาง)
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Middle) { // เปลี่ยนเป็น Middle ได้ถ้าต้องการ
            m_isPanning = true;
            // จำตำแหน่งที่คลิก "บนโลกในเกม" (World Coordinates)
            m_dragOrigin = window.mapPixelToCoords(sf::Mouse::getPosition(window), m_view);
        }
    }

    // 2. ตรวจจับการปล่อยปุ่มเพื่อหยุดลาก
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Middle) {
            m_isPanning = false;
        }
    }

    // 3. ระบบ Zoom (แถมให้)
    else if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            m_view.zoom(0.9f); // Zoom In
            m_currentZoom *= 0.9f;
        }
        else {
            m_view.zoom(1.1f); // Zoom Out
            m_currentZoom *= 1.1f;
        }
    }
}

void GameCamera::update(const sf::RenderWindow& window) {
    if (m_isPanning) {
        // หาตำแหน่งเมาส์ปัจจุบันในโลกเกม (ณ View ปัจจุบัน)
        sf::Vector2f currentMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), m_view);

        // คำนวณระยะห่าง: จุดเดิม - จุดปัจจุบัน
        // เหตุผล: ถ้าเราลากเมาส์ไปทางขวา (MousePos เพิ่ม) เราต้องการให้กล้องขยับไปทางซ้ายเพื่อให้ภาพเลื่อนตามเมาส์
        sf::Vector2f delta = m_dragOrigin - currentMousePos;

        // ขยับกล้อง
        m_view.move(delta);
    }
}

const sf::View& GameCamera::getView() const {
    return m_view;
}