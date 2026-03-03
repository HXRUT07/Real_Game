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
    sf::RectangleShape infoPanel;    // <--- [คืนชีพ] เอากลับมาแล้วครับ!
    sf::Text infoContent;            // <--- [คืนชีพ] เอากลับมาแล้วครับ!
    std::string m_targetResourceStr; // เก็บข้อความของช่อง/เมืองที่คลิกขวา

    // --- ตัวแปรเก็บคลังสมบัติของผู้เล่น ---
    int m_playerGold = 0;
    int m_playerWood = 0;
    int m_playerFood = 0;

    // --- ส่วนแถบขวา (Side Panel) ---
    bool m_showSidePanel = false;
    std::vector<Unit*> m_selectedUnits;

    // ---  ตัวแปรสำหรับปุ่มจบเทิร์น & เลขเทิร์น ---
    sf::RectangleShape endTurnBtn;
    sf::Text endTurnText;
    sf::Text turnCounterText;

public:
    MouseUI(); // Constructor

    void showResourcePanel(float windowWidth, int gold, int wood, int food);
    void hideInfo();

    void showCityResourcePanel(float windowWidth, int gold, int wood, int food);

    // --- ฟังก์ชันอัปเดตยอดคลังสมบัติของผู้เล่น ---
    void updatePlayerTreasury(int gold, int wood, int food);

    void setSelectionList(const std::vector<Unit*>& units);
    void clearSelection();

    void update(sf::Vector2f mousePos);

    // --- เพิ่ม isGameStarted เพื่อซ่อน UI ก่อนเริ่มเกม ---
    void draw(sf::RenderWindow& window, bool isGameStarted);

    void updateTurnInfo(int playerTurn, int turnNumber);
    bool isEndTurnButtonClicked(sf::Vector2f mousePos);
};