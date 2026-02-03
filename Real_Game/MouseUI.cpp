#include "MouseUI.h"

MouseUI::MouseUI() {
    // โหลดฟอนต์จากชื่อไฟล์ที่มีในเครื่องคุณ
    hasFont = font.loadFromFile("font");
    isPanelVisible = false;

    infoPanel.setSize(sf::Vector2f(200.f, 100.f));
    infoPanel.setFillColor(sf::Color(30, 30, 30, 220)); // สีเทาเข้มโปร่งใส
    infoPanel.setOutlineThickness(-2.0f); // วาดขอบเข้าด้านในเพื่อความเป๊ะ
    infoPanel.setOutlineColor(sf::Color::White);

    if (hasFont) {
        infoText.setFont(font);
        infoText.setCharacterSize(18);
        infoText.setFillColor(sf::Color::White);

        infoContent.setFont(font);
        infoContent.setCharacterSize(16);
        infoContent.setFillColor(sf::Color::White);
    }
}

void MouseUI::showInfo(sf::Vector2f position, std::string title, std::string detail) {
    isPanelVisible = true;
    infoPanel.setPosition(position);
    infoContent.setPosition(position.x + 10.f, position.y + 10.f);
    infoContent.setString(title + "\n\n" + detail);
}

void MouseUI::hideInfo() { isPanelVisible = false; }

void MouseUI::update(sf::Vector2f mousePos, int gold, int wood) {
    if (hasFont) {
        // อัปเดตข้อความให้ติดตามเมาส์
        infoText.setString("Gold: " + std::to_string(gold) + " Wood: " + std::to_string(wood));
        infoText.setPosition(mousePos.x + 15.f, mousePos.y + 15.f);
    }
}

void MouseUI::draw(sf::RenderWindow& window) {
    if (hasFont) {
        window.draw(infoText);
        if (isPanelVisible) {
            window.draw(infoPanel);
            window.draw(infoContent);
        }
    }
}