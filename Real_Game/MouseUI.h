#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class MouseUI {
private:
    sf::Font font;
    bool hasFont;
    bool isPanelVisible;

    // --- ส่วนที่เพิ่มสำหรับระบบคลิกขวา ---
    sf::RectangleShape infoPanel;
    sf::Text infoContent;

public:
    MouseUI(); // Constructor สำหรับโหลดฟอนต์

    void showResourcePanel(sf::Vector2f position, int gold, int wood, int food);
    void hideInfo();

    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window);
};