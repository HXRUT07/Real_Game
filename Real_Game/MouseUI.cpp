#include "MouseUI.h"

MouseUI::MouseUI() {
    hasFont = font.loadFromFile("font");

    infoPanel.setSize(sf::Vector2f(220.f, 110.f));
    infoPanel.setFillColor(sf::Color(30, 30, 30, 220));
    infoPanel.setOutlineThickness(-2.f);
    infoPanel.setOutlineColor(sf::Color::White);

    if (hasFont) {
        infoContent.setFont(font);
        infoContent.setCharacterSize(16);
        infoContent.setFillColor(sf::Color::White);
    }
}

void MouseUI::showResourcePanel(sf::Vector2f position, int gold, int wood, int food) {
    isPanelVisible = true;

    infoPanel.setPosition(position);
    infoContent.setPosition(position.x + 10.f, position.y + 10.f);

    infoContent.setString(
        "Resources\n"
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food)
    );
}

void MouseUI::hideInfo() {
    isPanelVisible = false;
}

void MouseUI::update(sf::Vector2f mousePos) {
    // ตอนนี้ยังไม่ต้องทำอะไร
}

void MouseUI::draw(sf::RenderWindow& window) {
    if (!hasFont) return;

    if (isPanelVisible) {
        window.draw(infoPanel);
        window.draw(infoContent);
    }
}
