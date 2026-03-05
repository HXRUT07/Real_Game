#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "City.h"
#include <string>

class CityPanel {
public:
    CityPanel(float winW, float winH);
    void setCity(City* c);
    void clear();
    bool isOpen() const { return city != nullptr; }
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);

private:
    City* city = nullptr;
    float panelX, panelY, panelW, panelH;
    float winWidth, winHeight;
    sf::Font font;

    sf::RectangleShape background;
    sf::RectangleShape topBar;
    sf::Text labelTopBar;

    // ชื่อเมือง
    sf::Text labelCityName;
    sf::Text textCityName;

    // Rename
    sf::Text labelRename;
    sf::RectangleShape renameBox;
    sf::Text renameInput;
    sf::RectangleShape btnRename;
    sf::Text btnRenameText;
    bool isRenaming = false;
    std::string renameBuffer;

    sf::RectangleShape divider1;

    // เลเวล
    sf::Text labelLevel;
    sf::Text textLevel;
    sf::RectangleShape levelBarBg;
    sf::RectangleShape levelBarFill;

    sf::RectangleShape divider2;

    // Stockpile
    sf::Text labelResources;
    sf::RectangleShape goldIcon, woodIcon, foodIcon;
    sf::Text textGold, textWood, textFood;

    sf::RectangleShape divider3;

    // Upgrade cost display
    sf::Text labelUpgradeCost;
    sf::RectangleShape costGoldIcon, costWoodIcon, costFoodIcon;
    sf::Text textCostGold, textCostWood, textCostFood;

    // Upgrade button
    sf::RectangleShape btnUpgrade;
    sf::Text btnUpgradeText;

    // "ไม่พอ" warning
    sf::Text textNotEnough;
    bool showNotEnough = false;

    // Close
    sf::RectangleShape btnClose;
    sf::Text btnCloseText;
};