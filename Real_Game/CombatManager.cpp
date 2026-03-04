#include "CombatManager.h"
#include "Unit.h"    // <--- ย้าย Include มาไว้ใน .cpp แทน
#include "GameMap.h" // <--- ย้าย Include มาไว้ใน .cpp แทน
#include <cstdlib>
#include <iostream>
#include <algorithm>

CombatManager::CombatManager() : m_isRolling(false), m_hasFont(false) {}

void CombatManager::setFont(const sf::Font& font) {
    m_font = font;
    m_hasFont = true;
}

int CombatManager::rollDiceRisk(int numDice) {
    int highest = 0;
    for (int i = 0; i < numDice; i++) {
        int roll = (std::rand() % 6) + 1; // ทอยเต๋า 1-6
        if (roll > highest) highest = roll;
    }
    return highest;
}

void CombatManager::initiateCombat(int atkR, int atkC, int defR, int defC, int attackerOwner, sf::Sound& diceSound) {
    m_atkStartR = atkR;
    m_atkStartC = atkC;
    m_defTargetR = defR;
    m_defTargetC = defC;
    m_attackerOwner = attackerOwner;

    // ฝ่ายบุกทอย 3 ฝ่ายรับทอย 2 เสมอ
    m_finalAtkRoll = rollDiceRisk(3);
    m_finalDefRoll = rollDiceRisk(2);

    m_isRolling = true;
    m_animTimer.restart();
    diceSound.play();
}

void CombatManager::updateAndDraw(sf::RenderWindow& window, std::vector<Unit>& units, GameMap& worldMap, sf::Sound& diceSound, sf::Sound& hitSound) {
    if (!m_isRolling) return;

    float elapsed = m_animTimer.getElapsedTime().asSeconds();

    // 1. เช็คว่าจบอนิเมชันหรือยัง (2.5 วินาที)
    if (elapsed > 2.5f) {
        m_isRolling = false;
        diceSound.stop();
        hitSound.play();

        // ค้นหาตัวบุก และ ตัวรับ จากกระดาน
        auto atkIt = units.end();
        auto defIt = units.end();
        for (auto it = units.begin(); it != units.end(); ++it) {
            if (it->getR() == m_atkStartR && it->getC() == m_atkStartC && it->getOwner() == m_attackerOwner) atkIt = it;
            if (it->getR() == m_defTargetR && it->getC() == m_defTargetC && it->getOwner() != m_attackerOwner) defIt = it;
        }

        // ประมวลผลแพ้ชนะ ลบตัวละคร
        if (atkIt != units.end() && defIt != units.end()) {
            if (m_finalAtkRoll > m_finalDefRoll) {
                atkIt->moveTo(m_defTargetR, m_defTargetC);
                atkIt->consumeAP(1);
                if (m_attackerOwner == 1) worldMap.revealFog(m_defTargetR, m_defTargetC, 1);
                units.erase(defIt);
            }
            else {
                units.erase(atkIt);
            }
        }
        return;
    }

    // 2. วาดฉาก UI สู้ลูกเต๋า
    if (!m_hasFont) return;

    sf::RectangleShape darkOverlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    darkOverlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(darkOverlay);

    if (elapsed < 1.5f) {
        m_displayAtkRoll = (std::rand() % 6) + 1;
        m_displayDefRoll = (std::rand() % 6) + 1;
    }
    else {
        m_displayAtkRoll = m_finalAtkRoll;
        m_displayDefRoll = m_finalDefRoll;
    }

    std::string titleStr = "COMBAT PHASE";
    std::string atkStr = (m_attackerOwner == 1 ? "PLAYER (Attacker)\nRolls 3 Dice" : "AI (Attacker)\nRolls 3 Dice");
    std::string defStr = (m_attackerOwner == 1 ? "AI (Defender)\nRolls 2 Dice" : "PLAYER (Defender)\nRolls 2 Dice");

    sf::Text titleText(titleStr, m_font, 50);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setPosition(window.getSize().x / 2.0f - 200, 100);

    sf::Text atkSideText(atkStr, m_font, 30);
    atkSideText.setFillColor(sf::Color(255, 100, 100));
    atkSideText.setPosition(window.getSize().x / 4.0f - 100, 300);

    sf::Text atkRollText(std::to_string(m_displayAtkRoll), m_font, 100);
    atkRollText.setFillColor(sf::Color::White);
    atkRollText.setPosition(window.getSize().x / 4.0f, 400);

    sf::Text vsText("VS", m_font, 40);
    vsText.setFillColor(sf::Color::White);
    vsText.setPosition(window.getSize().x / 2.0f - 25, 420);

    sf::Text defSideText(defStr, m_font, 30);
    defSideText.setFillColor(sf::Color(100, 150, 255));
    defSideText.setPosition(window.getSize().x * 3.0f / 4.0f - 100, 300);

    sf::Text defRollText(std::to_string(m_displayDefRoll), m_font, 100);
    defRollText.setFillColor(sf::Color::White);
    defRollText.setPosition(window.getSize().x * 3.0f / 4.0f, 400);

    window.draw(titleText);
    window.draw(atkSideText);
    window.draw(atkRollText);
    window.draw(vsText);
    window.draw(defSideText);
    window.draw(defRollText);

    if (elapsed >= 1.5f) {
        sf::Text resultText("", m_font, 60);
        if (m_finalAtkRoll > m_finalDefRoll) {
            resultText.setString("ATTACKER WINS!");
            resultText.setFillColor(sf::Color::Green);
        }
        else {
            resultText.setString("DEFENDER WINS!");
            resultText.setFillColor(sf::Color::Red);
        }
        resultText.setPosition(window.getSize().x / 2.0f - 220, 600);
        window.draw(resultText);
    }
}