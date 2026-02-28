#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector> // <---  จำเป็นสำหรับ std::vector
#include "Unit.h" // <---  ต้องรู้จัก Unit เพื่อเก็บ Pointer

class MouseUI {
private:
    sf::Font font;
    bool hasFont;

    // --- ส่วนเดิม (Tooltip/Resource) ---
    bool isPanelVisible;
    sf::RectangleShape infoPanel;
    sf::Text infoContent;

    // --- ส่วนแถบขวา (Side Panel) ---
    bool m_showSidePanel = false;        // สถานะโชว์แถบขวา
    std::vector<Unit*> m_selectedUnits;  // รายการยูนิตที่จะโชว์

public:
    MouseUI(); // Constructor

    // ฟังก์ชันเดิม (ปรับลำดับเป็น wood, gold, food ให้ตรงกับ main.cpp)
    void showResourcePanel(float windowWidth, int wood, int gold, int food);
    void hideInfo();

    // --- ฟังก์ชันโชว์คลังหลวงของเมือง ---
    void showCityResourcePanel(float windowWidth, int gold, int wood, int food);

    // ฟังก์ชันจัดการแถบขวา
    void setSelectionList(const std::vector<Unit*>& units); // รับยูนิตมาโชว์
    void clearSelection();                                  // ปิดแถบ

    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window);
};