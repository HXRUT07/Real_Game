#pragma once
#include <SFML/Graphics.hpp>
#include "City.h"

class BuildMenu {
public:
    BuildMenu(float winW, float winH);

    void setCity(City* c);
    void clear();
    bool isOpen() const { return city != nullptr; }

    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);

    // คืน index ที่เพิ่งสร้างสำเร็จ (-1 = ไม่มี)
    int getLastBuiltIndex() const { return lastBuiltIdx; }
    void clearLastBuilt() { lastBuiltIdx = -1; }

private:
    City* city = nullptr;
    float panelX, panelY, panelW, panelH;
    sf::Font font;

    // พื้นหลัง
    sf::RectangleShape background;
    sf::RectangleShape topBar;
    sf::Text labelTitle;

    // Close
    sf::RectangleShape btnClose;
    sf::Text btnCloseText;

    // 4 building slots
    struct Slot {
        sf::RectangleShape card;
        sf::RectangleShape btnBuild;
        sf::Text nameText;
        sf::Text descText;
        sf::Text countText;
        sf::Text costText;
        sf::Text btnText;
    };
    Slot slots[4];

    // feedback
    sf::Text feedbackText;
    int feedbackTimer = 0;
    int lastBuiltIdx = -1;

    void initSlots();
    void updateSlot(int i);
};