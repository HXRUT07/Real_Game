#include "MouseUI.h"
#include <random>
#include <array>
#include <string> // จำเป็นสำหรับ to_string

// MouseUI.cpp

MouseUI::MouseUI() {
    // โหลดฟอนต์เดิมของคุณ
    hasFont = font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf");

    // กำหนดค่าเริ่มต้น
    isPanelVisible = false;
    m_showSidePanel = false; //  เริ่มต้นปิดแถบขวาไว้ก่อน

    // ตั้งค่า Resource Panel 
    infoPanel.setSize(sf::Vector2f(220.f, 140.f));
    infoPanel.setFillColor(sf::Color(30, 30, 30, 220));
    infoPanel.setOutlineThickness(2.f);
    infoPanel.setOutlineColor(sf::Color(100, 200, 255));

    if (hasFont) {
        infoContent.setFont(font);
        infoContent.setCharacterSize(20);
        infoContent.setFillColor(sf::Color::White);

        // ---  ตั้งค่าปุ่ม End Turn ---
        endTurnBtn.setSize(sf::Vector2f(160.f, 50.f));
        endTurnBtn.setFillColor(sf::Color(150, 50, 50));
        endTurnBtn.setOutlineThickness(3.f);
        endTurnBtn.setOutlineColor(sf::Color(255, 200, 0));

        endTurnText.setFont(font);
        endTurnText.setString("END TURN");
        endTurnText.setCharacterSize(24);
        endTurnText.setFillColor(sf::Color::White);
        endTurnText.setStyle(sf::Text::Bold);

        // --- [ระบบใหม่] ตั้งค่าเลขเทิร์นมุมซ้ายบน ---
        turnCounterText.setFont(font);
        turnCounterText.setCharacterSize(45); // ตัวใหญ่ๆ
        turnCounterText.setFillColor(sf::Color::White);
        turnCounterText.setOutlineThickness(2.f);
        turnCounterText.setOutlineColor(sf::Color::Black);
        turnCounterText.setPosition(20.f, 20.f);
        turnCounterText.setString("Player 1 - Turn 1");
    }
}

// ---------------------------------------------------------
// ส่วนจัดการ Resource Panel 
// ---------------------------------------------------------
void MouseUI::showResourcePanel(float windowWidth, int gold, int wood, int food) {
    isPanelVisible = true;
    m_showSidePanel = false; // ปิดแถบทหารเผื่อเปิดค้างไว้

    float padding = 20.f;
    float posX = windowWidth - infoPanel.getSize().x - padding;
    float posY = padding;

    infoPanel.setPosition(posX, posY);
    infoContent.setPosition(posX + 14.f, posY + 12.f);

    // เอาค่าที่รับมาจาก main.cpp มาโชว์เลย ไม่ต้องสุ่มใหม่แล้ว
    infoContent.setString(
        "Resources in this area\n"
        "\n"
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food)
    );
}

// ---------------------------------------------------------
// โชว์หน้าต่างคลังหลวงของเมือง (เวลาคลิกขวาที่เมือง)
// ---------------------------------------------------------
void MouseUI::showCityResourcePanel(float windowWidth, int gold, int wood, int food) {
    isPanelVisible = true;
    m_showSidePanel = false; // ปิดแถบทหารเผื่อเปิดค้างไว้

    float padding = 20.f;
    float posX = windowWidth - infoPanel.getSize().x - padding;
    float posY = padding;

    infoPanel.setPosition(posX, posY);
    infoContent.setPosition(posX + 14.f, posY + 12.f);

    infoContent.setString(
        "--- CITY STOCKPILE ---\n"
        "\n"
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food)
    );
}

void MouseUI::hideInfo() {
    isPanelVisible = false;
}

// ---------------------------------------------------------
//  ส่วนจัดการ Side Panel (รายชื่อยูนิต)
// ---------------------------------------------------------
void MouseUI::setSelectionList(const std::vector<Unit*>& units) {
    m_selectedUnits = units;
    m_showSidePanel = !units.empty(); // ถ้ามีข้อมูลก็เปิดเลย

    if (m_showSidePanel) {
        isPanelVisible = false; // ปิด Resource panel เพื่อไม่ให้ตีกัน
    }
}

void MouseUI::clearSelection() {
    m_selectedUnits.clear();
    m_showSidePanel = false;
}

void MouseUI::update(sf::Vector2f mousePos) {
    // ยังไม่ต้องทำอะไร
}

// ---------------------------------------------------------
// [ระบบใหม่] อัปเดตข้อมูลเลขเทิร์นและปุ่ม
// ---------------------------------------------------------
void MouseUI::updateTurnInfo(int playerTurn, int turnNumber) {
    if (!hasFont) return;

    std::string pName = (playerTurn == 1) ? "Player 1" : "AI (Player 2)";
    turnCounterText.setString(pName + "   |   Turn: " + std::to_string(turnNumber));

    // เปลี่ยนสีข้อความตามตาของใคร
    if (playerTurn == 1) {
        turnCounterText.setFillColor(sf::Color(100, 255, 100)); // ตาเราสีเขียว
        endTurnBtn.setFillColor(sf::Color(150, 50, 50));        // ปุ่มกดได้สีแดงสด
    }
    else {
        turnCounterText.setFillColor(sf::Color(255, 100, 100)); // ตา AI สีแดง
        endTurnBtn.setFillColor(sf::Color(80, 80, 80));         // ปุ่มเทา (กดไม่ได้)
    }
}

bool MouseUI::isEndTurnButtonClicked(sf::Vector2f mousePos) {
    return endTurnBtn.getGlobalBounds().contains(mousePos);
}

// ---------------------------------------------------------
// Draw Function (วาดทั้งสองอย่าง)
// ---------------------------------------------------------
void MouseUI::draw(sf::RenderWindow& window) {
    if (!hasFont) return;

    // ---  วาดเลขเทิร์นมุมซ้ายบนเสมอ ---
    window.draw(turnCounterText);

    // จัดตำแหน่งปุ่ม End Turn ไว้มุมขวาล่าง
    float screenW = (float)window.getSize().x;
    float screenH = (float)window.getSize().y;

    endTurnBtn.setPosition(screenW - 190.f, screenH - 80.f);
    endTurnText.setPosition(screenW - 165.f, screenH - 70.f); // จัดตัวหนังสือให้อยู่ตรงกลางปุ่ม

    window.draw(endTurnBtn);
    window.draw(endTurnText);

    // 1. วาด Resource Panel (ถ้าเปิดอยู่)
    if (isPanelVisible) {
        window.draw(infoPanel);
        window.draw(infoContent);
    }

    // 2. วาด Side Panel (ถ้ามีการเลือกยูนิต)
    if (m_showSidePanel) {
        float panelWidth = 220.0f;

        // พื้นหลังแถบขวา (ยาวเต็มจอ)
        sf::RectangleShape bg(sf::Vector2f(panelWidth, screenH));
        bg.setPosition(screenW - panelWidth, 0);
        bg.setFillColor(sf::Color(20, 20, 20, 200)); // สีดำจางๆ
        bg.setOutlineColor(sf::Color(100, 100, 100));
        bg.setOutlineThickness(-2.0f); // ขอบใน

        window.draw(bg);

        // หัวข้อ "Unit Group"
        sf::Text title("Selected Group", font, 22);
        title.setPosition(screenW - panelWidth + 15, 20);
        title.setFillColor(sf::Color::Yellow);
        title.setStyle(sf::Text::Bold);
        window.draw(title);

        // เส้นคั่นหัวข้อ
        sf::RectangleShape line(sf::Vector2f(panelWidth - 30, 2));
        line.setPosition(screenW - panelWidth + 15, 55);
        line.setFillColor(sf::Color(100, 100, 100));
        window.draw(line);

        // วนลูปวาดรายชื่อยูนิต
        float startY = 70.0f;
        for (const auto* u : m_selectedUnits) {
            // สร้างข้อความ
            std::string status = u->getName();
            std::string apText = "AP: " + std::to_string(u->getCurrentAP()) + " / " + std::to_string(u->getMaxAP());

            // ชื่อยูนิต
            sf::Text nameText(status, font, 18);
            nameText.setPosition(screenW - panelWidth + 20, startY);

            // AP (อยู่บรรทัดล่าง)
            sf::Text subText(apText, font, 14);
            subText.setPosition(screenW - panelWidth + 20, startY + 22);

            // เปลี่ยนสีตามสถานะ AP
            if (u->hasAP()) {
                nameText.setFillColor(sf::Color::White);      // มีแรง = ขาว
                subText.setFillColor(sf::Color::Green);       // AP = เขียว
            }
            else {
                nameText.setFillColor(sf::Color(150, 150, 150)); // หมดแรง = เทา
                subText.setFillColor(sf::Color::Cyan);           // AP หมด = ฟ้า
            }

            window.draw(nameText);
            window.draw(subText);

            // ขยับตำแหน่งลงมาสำหรับตัวถัดไป
            startY += 60.0f;
        }
    }
}