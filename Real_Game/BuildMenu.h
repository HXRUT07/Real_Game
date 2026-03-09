#pragma once
#include <SFML/Graphics.hpp>
#include "City.h"
#include "Unit.h"
#include <vector>

class BuildMenu {
public:
    BuildMenu(float winW, float winH);

    void setCity(City* c);
    void clear();
    bool isOpen() const { return city != nullptr; }

    void setUnits(std::vector<Unit>* u) { units = u; }

    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);

    // คืน index ที่เพิ่งสร้างสำเร็จ (-1 = ไม่มี)
    int getLastBuiltIndex() const { return lastBuiltIdx; }
    void clearLastBuilt() { lastBuiltIdx = -1; }

    bool didRecruit() const { return recruitedThisFrame; }
    void clearRecruit() { recruitedThisFrame = false; }

    bool isPendingTileSelect() const { return pendingBuildIdx != -1; }
    int  getPendingBuildIdx()  const { return pendingBuildIdx; }
    void confirmTileSelect(bool success); // เรียกจาก main หลังคลิก tile
    void cancelTileSelect();              // เรียกเมื่อ right-click หรือยกเลิก


private:
    City* city = nullptr;
    std::vector<Unit>* units = nullptr;
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
        sf::RectangleShape btnRecruit;
        sf::Text nameText;
        sf::Text descText;
        sf::Text countText;
        sf::Text costText;
        sf::Text btnText;
        sf::Text btnRecruitText;
    };
    Slot slots[4];

    // feedback
    sf::Text feedbackText;
    sf::Text placementHintText;
    int feedbackTimer = 0;
    int lastBuiltIdx = -1;
    bool recruitedThisFrame = false;

    int pendingBuildIdx = -1;

    void initSlots();
    void updateSlot(int i);
};