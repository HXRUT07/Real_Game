#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class MouseUI {
private:
    sf::Font font;
    sf::Text infoText;

    // --- ส่วนที่เพิ่มสำหรับระบบคลิกขวา ---
    sf::RectangleShape infoPanel;
    sf::Text infoContent;
    bool isPanelVisible;
    bool hasFont;

public:
    MouseUI(); // Constructor สำหรับโหลดฟอนต์
    void update(sf::Vector2f mousePos, int gold, int wood); // อัปเดตข้อความตามเมาส์
    void showInfo(sf::Vector2f position, std::string title, std::string detail); // เปิดแถบข้อมูล
    void hideInfo(); // ปิดแถบข้อมูล
    void draw(sf::RenderWindow& window); // วาด UI ลงหน้าจอ
};