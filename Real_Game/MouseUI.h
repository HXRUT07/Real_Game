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
    // -------------------------------
    // Resource Icon (เปรมทำ)
    // -------------------------------
    sf::Texture foodTex;
    sf::Texture woodTex;
    sf::Texture goldTex;
    sf::Sprite foodIcon;
    sf::Sprite woodIcon;
    sf::Sprite goldIcon;
    // เปรมทำ - ตัวแปรเก็บค่าทรัพยากรที่จะแสดงมุมขวาบน
    int m_wood = 0;
    int m_gold = 0;
    int m_food = 0;
    sf::Text m_woodText;
    sf::Text m_goldText;
    sf::Text m_foodText;
    // เปรมทำ - จบ
    // --- ส่วนแถบขวา (Side Panel) ---
    bool m_showSidePanel = false;
    std::vector<Unit*> m_selectedUnits;
    // ---  ตัวแปรสำหรับปุ่มจบเทิร์น & เลขเทิร์น ---
    sf::RectangleShape endTurnBtn;
    sf::Text endTurnText;
    sf::Text turnCounterText;
public:
    MouseUI();
    void showResourcePanel(float windowWidth, int wood, int gold, int food);
    void hideInfo();
    void showCityResourcePanel(float windowWidth, int gold, int wood, int food);
    void setSelectionList(const std::vector<Unit*>& units);
    void clearSelection();
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window);
    void updateTurnInfo(int playerTurn, int turnNumber);
    bool isEndTurnButtonClicked(sf::Vector2f mousePos);
    // เปรมทำ - ฟังก์ชันอัปเดตทรัพยากรมุมขวาบน
    void updateResourceBar(int wood, int gold, int food);
    // เปรมทำ - จบ
};