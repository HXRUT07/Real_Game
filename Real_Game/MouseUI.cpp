#include "MouseUI.h"
#include <random>
#include <array>
#include <string> // จำเป็นสำหรับ to_string

// MouseUI.cpp

MouseUI::MouseUI() {
    hasFont = font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf");

    isPanelVisible = false;
    m_showSidePanel = false;

    infoPanel.setSize(sf::Vector2f(220.f, 140.f));
    infoPanel.setFillColor(sf::Color(30, 30, 30, 220));
    infoPanel.setOutlineThickness(2.f);
    infoPanel.setOutlineColor(sf::Color(100, 200, 255));

    if (hasFont) {
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
    }
}

void MouseUI::showResourcePanel(float windowWidth, int gold, int wood, int food) {
    isPanelVisible = true;
    m_showSidePanel = false;

    m_targetResourceStr =
        "--- TILE RESOURCES ---\n"
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food);
}

void MouseUI::showCityResourcePanel(float windowWidth, int gold, int wood, int food) {
    isPanelVisible = true;
    m_showSidePanel = false;

    m_targetResourceStr =
        "--- CITY STOCKPILE ---\n"
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food);
}

void MouseUI::hideInfo() {
    isPanelVisible = false;
}

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
}

void MouseUI::updatePlayerTreasury(int gold, int wood, int food) {
    m_playerGold = gold;
    m_playerWood = wood;
    m_playerFood = food;
}

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

void MouseUI::draw(sf::RenderWindow& window, bool isGameStarted) {
    if (!hasFont) return;

    // ซ่อน UI ทุกอย่าง ถ้าเกมยังไม่เริ่ม 
    if (!isGameStarted) return;

    float screenW = (float)window.getSize().x;
    float screenH = (float)window.getSize().y;

    window.draw(turnCounterText);

    endTurnBtn.setPosition(screenW - 190.f, screenH - 80.f);
    endTurnText.setPosition(screenW - 165.f, screenH - 70.f);
    window.draw(endTurnBtn);
    window.draw(endTurnText);

    // หน้าต่าง Resource มุมขวาบน
    float padding = 20.f;
    float panelWidth = 240.f;
    float baseHeight = 110.f;

    if (isPanelVisible) {
        baseHeight += 110.f;
    }

    sf::RectangleShape unifiedPanel(sf::Vector2f(panelWidth, baseHeight));
    unifiedPanel.setPosition(screenW - panelWidth - padding, padding);
    unifiedPanel.setFillColor(sf::Color(30, 30, 30, 230));
    unifiedPanel.setOutlineThickness(2.f);
    unifiedPanel.setOutlineColor(sf::Color(255, 215, 0));

    window.draw(unifiedPanel);

    sf::Text treasuryText;
    treasuryText.setFont(font);
    treasuryText.setCharacterSize(18);
    treasuryText.setFillColor(sf::Color::White);
    treasuryText.setString(
        "=== PLAYER TREASURY ===\n"
        "Gold : " + std::to_string(m_playerGold) + "\n" +
        "Wood : " + std::to_string(m_playerWood) + "\n" +
        "Food : " + std::to_string(m_playerFood)
    );
    treasuryText.setPosition(screenW - panelWidth - padding + 15.f, padding + 10.f);
    window.draw(treasuryText);

    if (isPanelVisible) {
        sf::Text targetText;
        targetText.setFont(font);
        targetText.setCharacterSize(18);
        targetText.setFillColor(sf::Color::Cyan);
        targetText.setString(m_targetResourceStr);
        targetText.setPosition(screenW - panelWidth - padding + 15.f, padding + 120.f);
        window.draw(targetText);
    }

    if (m_showSidePanel) {
        float panelW = 220.0f;

        sf::RectangleShape bg(sf::Vector2f(panelW, screenH));
        bg.setPosition(screenW - panelW, 0);
        bg.setFillColor(sf::Color(20, 20, 20, 200));
        bg.setOutlineColor(sf::Color(100, 100, 100));
        bg.setOutlineThickness(-2.0f);

        window.draw(bg);

        sf::Text title("Selected Group", font, 22);
        title.setPosition(screenW - panelW + 15, 20);
        title.setFillColor(sf::Color::Yellow);
        title.setStyle(sf::Text::Bold);
        window.draw(title);

        sf::RectangleShape line(sf::Vector2f(panelW - 30, 2));
        line.setPosition(screenW - panelW + 15, 55);
        line.setFillColor(sf::Color(100, 100, 100));
        window.draw(line);

        float startY = 70.0f;
        for (const auto* u : m_selectedUnits) {
            std::string status = u->getName();
            std::string apText = "AP: " + std::to_string(u->getCurrentAP()) + " / " + std::to_string(u->getMaxAP());

            sf::Text nameText(status, font, 18);
            nameText.setPosition(screenW - panelW + 20, startY);

            sf::Text subText(apText, font, 14);
            subText.setPosition(screenW - panelW + 20, startY + 22);

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