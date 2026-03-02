#include "MouseUI.h"
#include <random>
#include <array>
#include <string>

MouseUI::MouseUI() {
    hasFont = font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf");

    isPanelVisible = false;
    m_showSidePanel = false;

    infoPanel.setSize(sf::Vector2f(220.f, 140.f));
    infoPanel.setFillColor(sf::Color(30, 30, 30, 220));
    infoPanel.setOutlineThickness(2.f);
    infoPanel.setOutlineColor(sf::Color(100, 200, 255));

    if (hasFont) {
        infoContent.setFont(font);
        infoContent.setCharacterSize(20);
        infoContent.setFillColor(sf::Color::White);

        endTurnBtn.setSize(sf::Vector2f(160.f, 50.f));
        endTurnBtn.setFillColor(sf::Color(150, 50, 50));
        endTurnBtn.setOutlineThickness(3.f);
        endTurnBtn.setOutlineColor(sf::Color(255, 200, 0));

        endTurnText.setFont(font);
        endTurnText.setString("END TURN");
        endTurnText.setCharacterSize(24);
        endTurnText.setFillColor(sf::Color::White);
        endTurnText.setStyle(sf::Text::Bold);

        turnCounterText.setFont(font);
        turnCounterText.setCharacterSize(45);
        turnCounterText.setFillColor(sf::Color::White);
        turnCounterText.setOutlineThickness(2.f);
        turnCounterText.setOutlineColor(sf::Color::Black);
        turnCounterText.setPosition(20.f, 20.f);
        turnCounterText.setString("Player 1 - Turn 1");

        // เปรมทำ - ตั้งค่า text แสดงจำนวนทรัพยากร
        m_woodText.setFont(font);
        m_woodText.setCharacterSize(22);
        m_woodText.setFillColor(sf::Color::White);

        m_goldText.setFont(font);
        m_goldText.setCharacterSize(22);
        m_goldText.setFillColor(sf::Color::Yellow);

        m_foodText.setFont(font);
        m_foodText.setCharacterSize(22);
        m_foodText.setFillColor(sf::Color::Green);
        // เปรมทำ - จบ
    }

    // เปรมทำ - โหลด icon รูปภาพทรัพยากร
    if (woodTex.loadFromFile("wood.png")) {
        woodIcon.setTexture(woodTex);
        woodIcon.setScale(40.f / woodTex.getSize().x, 40.f / woodTex.getSize().y);
    }
    if (goldTex.loadFromFile("gold.png")) {
        goldIcon.setTexture(goldTex);
        goldIcon.setScale(40.f / goldTex.getSize().x, 40.f / goldTex.getSize().y);
    }
    if (foodTex.loadFromFile("food.png")) {
        foodIcon.setTexture(foodTex);
        foodIcon.setScale(40.f / foodTex.getSize().x, 40.f / foodTex.getSize().y);
    }
    // เปรมทำ - จบ
}

// ---------------------------------------------------------
// ส่วนจัดการ Resource Panel 
// ---------------------------------------------------------
void MouseUI::showResourcePanel(float windowWidth, int gold, int wood, int food) {
    isPanelVisible = true;
    m_showSidePanel = false;

    float padding = 20.f;
    float posX = windowWidth - infoPanel.getSize().x - padding;
    float posY = padding;

    infoPanel.setPosition(posX, posY);
    infoContent.setPosition(posX + 14.f, posY + 12.f);

    infoContent.setString(
        "Resources in this area\n"
        "\n"
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food)
    );
}

// ---------------------------------------------------------
// โชว์หน้าต่างคลังหลวงของเมือง
// ---------------------------------------------------------
void MouseUI::showCityResourcePanel(float windowWidth, int gold, int wood, int food) {
    isPanelVisible = true;
    m_showSidePanel = false;

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
// ส่วนจัดการ Side Panel
// ---------------------------------------------------------
void MouseUI::setSelectionList(const std::vector<Unit*>& units) {
    m_selectedUnits = units;
    m_showSidePanel = !units.empty();

    if (m_showSidePanel) {
        isPanelVisible = false;
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
// อัปเดตเลขเทิร์น
// ---------------------------------------------------------
void MouseUI::updateTurnInfo(int playerTurn, int turnNumber) {
    if (!hasFont) return;

    std::string pName = (playerTurn == 1) ? "Player 1" : "AI (Player 2)";
    turnCounterText.setString(pName + "   |   Turn: " + std::to_string(turnNumber));

    if (playerTurn == 1) {
        turnCounterText.setFillColor(sf::Color(100, 255, 100));
        endTurnBtn.setFillColor(sf::Color(150, 50, 50));
    }
    else {
        turnCounterText.setFillColor(sf::Color(255, 100, 100));
        endTurnBtn.setFillColor(sf::Color(80, 80, 80));
    }
}

bool MouseUI::isEndTurnButtonClicked(sf::Vector2f mousePos) {
    return endTurnBtn.getGlobalBounds().contains(mousePos);
}

// ---------------------------------------------------------
// เปรมทำ - อัปเดตค่าทรัพยากรมุมขวาบน
// ---------------------------------------------------------
void MouseUI::updateResourceBar(int wood, int gold, int food) {
    m_wood = wood;
    m_gold = gold;
    m_food = food;
    m_woodText.setString(std::to_string(wood));
    m_goldText.setString(std::to_string(gold));
    m_foodText.setString(std::to_string(food));
}
// เปรมทำ - จบ

// ---------------------------------------------------------
// Draw Function
// ---------------------------------------------------------
void MouseUI::draw(sf::RenderWindow& window) {
    if (!hasFont) return;

    window.draw(turnCounterText);

    float screenW = (float)window.getSize().x;
    float screenH = (float)window.getSize().y;

    endTurnBtn.setPosition(screenW - 190.f, screenH - 80.f);
    endTurnText.setPosition(screenW - 165.f, screenH - 70.f);

    window.draw(endTurnBtn);
    window.draw(endTurnText);

    // เปรมทำ - วาด resource bar มุมขวาบน
    float iconSize = 40.f;
    float gap = 90.f;
    float startX = screenW - (gap * 3) - 20.f;
    float iconY = 15.f;
    float textY = iconY + iconSize + 2.f;

    woodIcon.setPosition(startX, iconY);
    m_woodText.setPosition(startX + 5.f, textY);

    goldIcon.setPosition(startX + gap, iconY);
    m_goldText.setPosition(startX + gap + 5.f, textY);

    foodIcon.setPosition(startX + gap * 2, iconY);
    m_foodText.setPosition(startX + gap * 2 + 5.f, textY);

    window.draw(woodIcon);
    window.draw(m_woodText);
    window.draw(goldIcon);
    window.draw(m_goldText);
    window.draw(foodIcon);
    window.draw(m_foodText);
    // เปรมทำ - จบ

    if (isPanelVisible) {
        window.draw(infoPanel);
        window.draw(infoContent);
    }

    if (m_showSidePanel) {
        float panelWidth = 220.0f;

        sf::RectangleShape bg(sf::Vector2f(panelWidth, screenH));
        bg.setPosition(screenW - panelWidth, 0);
        bg.setFillColor(sf::Color(20, 20, 20, 200));
        bg.setOutlineColor(sf::Color(100, 100, 100));
        bg.setOutlineThickness(-2.0f);

        window.draw(bg);

        sf::Text title("Selected Group", font, 22);
        title.setPosition(screenW - panelWidth + 15, 20);
        title.setFillColor(sf::Color::Yellow);
        title.setStyle(sf::Text::Bold);
        window.draw(title);

        sf::RectangleShape line(sf::Vector2f(panelWidth - 30, 2));
        line.setPosition(screenW - panelWidth + 15, 55);
        line.setFillColor(sf::Color(100, 100, 100));
        window.draw(line);

        float startY = 70.0f;
        for (const auto* u : m_selectedUnits) {
            std::string status = u->getName();
            std::string apText = "AP: " + std::to_string(u->getCurrentAP()) + " / " + std::to_string(u->getMaxAP());

            sf::Text nameText(status, font, 18);
            nameText.setPosition(screenW - panelWidth + 20, startY);

            sf::Text subText(apText, font, 14);
            subText.setPosition(screenW - panelWidth + 20, startY + 22);

            if (u->hasAP()) {
                nameText.setFillColor(sf::Color::White);
                subText.setFillColor(sf::Color::Green);
            }
            else {
                nameText.setFillColor(sf::Color(150, 150, 150));
                subText.setFillColor(sf::Color::Cyan);
            }

            window.draw(nameText);
            window.draw(subText);

            startY += 60.0f;
        }
    }
}