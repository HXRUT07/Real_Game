#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Unit.h"

class MouseUI {
private:
    sf::Font font;
    bool hasFont;

    // --- ส่วนเดิม (Tooltip/Resource) ---
    bool isPanelVisible;
    sf::RectangleShape infoPanel;
    sf::Text infoContent;
    std::string m_targetResourceStr;

    // -------------------------------
    // Resource Icon (เปรมทำ)
    // -------------------------------
    sf::Texture foodTex;
    sf::Texture woodTex;
    sf::Texture goldTex;
    sf::Sprite foodIcon;
    sf::Sprite woodIcon;
    sf::Sprite goldIcon;
    int m_wood = 0;
    int m_gold = 0;
    int m_food = 0;
    sf::Text m_woodText;
    sf::Text m_goldText;
    sf::Text m_foodText;

    // --- ส่วนแถบขวา (Side Panel) ---
    bool m_showSidePanel = false;
    std::vector<Unit*> m_selectedUnits;

    // ---  ตัวแปรสำหรับปุ่มจบเทิร์น & เลขเทิร์น ---
    sf::RectangleShape endTurnBtn;
    sf::Text endTurnText;
    sf::Text turnCounterText;

    // Building City Button
    sf::RectangleShape buildingCityBtn;
    sf::Text buildingCityText;
    bool m_buildingCityMode = false; 

    // ==========================================
    // [ระบบกองทัพและการแยกทัพ]
    // ==========================================
    bool m_isArmyMode = true;
    int m_selectedIndex = 0;
    sf::RectangleShape modeBtn;
    sf::Text modeText;
    std::vector<sf::FloatRect> itemRects;

public:
    MouseUI();
    void showResourcePanel(float windowWidth, int gold, int wood, int food);
    void hideInfo();
    void showCityResourcePanel(float windowWidth, int gold, int wood, int food);
    void setSelectionList(const std::vector<Unit*>& units);
    void clearSelection();
    void update(sf::Vector2f mousePos);

    // ฟังก์ชันวาด UI
    void draw(sf::RenderWindow& window);
    void updateTurnInfo(int playerTurn, int turnNumber);
    bool isEndTurnButtonClicked(sf::Vector2f mousePos);

    // เปรมทำ - ฟังก์ชันอัปเดตทรัพยากรมุมขวาบน
    void updateResourceBar(int wood, int gold, int food);

    bool isBuildingCityButtonClicked(sf::Vector2f mousePos);
    bool isBuildingCityMode() const { return m_buildingCityMode; }
    void setBuildingCityMode(bool val) { m_buildingCityMode = val; }

    // --- ฟังก์ชันคุมปุ่มแยกกองทัพ ---
    bool isSidePanelVisible() const { return m_showSidePanel; }
    void toggleArmyMode() { m_isArmyMode = !m_isArmyMode; }
    bool isArmyMode() const { return m_isArmyMode; }
    void setArmyMode(bool mode) { m_isArmyMode = mode; }
    void setSelectedIndex(int idx) { m_selectedIndex = idx; m_isArmyMode = false; }
    int getSelectedIndex() const { return m_selectedIndex; }
    bool isModeButtonClicked(sf::Vector2f mousePos);
    int getClickedItemIndex(sf::Vector2f mousePos);
};