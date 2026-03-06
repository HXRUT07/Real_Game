#include "CombatManager.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

CombatManager::CombatManager() : m_isRolling(false), m_hasFont(false), m_isArmyAttack(true), m_diceCalculated(false) {}

void CombatManager::setFont(const sf::Font& font) {
    m_font = font;
    m_hasFont = true;
}

int CombatManager::rollDiceRisk(int numDice) {
    int highest = 0;
    for (int i = 0; i < numDice; i++) {
        int roll = (std::rand() % 6) + 1;
        if (roll > highest) highest = roll;
    }
    return highest;
}

void CombatManager::initiateCombat(int atkR, int atkC, int defR, int defC, int attackerOwner, sf::Sound& diceSound, bool isArmyAttack) {
    m_atkStartR = atkR;
    m_atkStartC = atkC;
    m_defTargetR = defR;
    m_defTargetC = defC;
    m_attackerOwner = attackerOwner;
    m_isArmyAttack = isArmyAttack;

    m_isRolling = true;
    m_diceCalculated = false; // รีเซ็ตเพื่อให้นับและทอยใหม่ใน updateAndDraw
    m_animTimer.restart();
    diceSound.play();
}

void CombatManager::updateAndDraw(sf::RenderWindow& window, std::vector<Unit>& units, GameMap& worldMap, sf::Sound& diceSound, sf::Sound& hitSound) {
    if (!m_isRolling) return;

    // ==============================================================
    // นับทหาร เก็บชื่อ และทอยแต้มแยกทีละตัว (ทำแค่เฟรมแรกของการต่อสู้)
    // ==============================================================
    if (!m_diceCalculated) {
        m_atkUnitNames.clear();
        m_defUnitNames.clear();
        m_atkDiceValues.clear();
        m_defDiceValues.clear();

        // กวาดนับทหารที่อยู่ในช่องบุก และ ช่องรับ เพื่อเก็บชื่อ
        for (const auto& u : units) {
            if (u.getR() == m_atkStartR && u.getC() == m_atkStartC && u.getOwner() == m_attackerOwner) {
                m_atkUnitNames.push_back(u.getName());
            }
            if (u.getR() == m_defTargetR && u.getC() == m_defTargetC && u.getOwner() != m_attackerOwner) {
                m_defUnitNames.push_back(u.getName());
            }
        }

        // กันเหนียว (ขั้นต่ำต้องมี 1 ตัว)
        if (m_atkUnitNames.empty()) m_atkUnitNames.push_back("Atk_Ghost");
        if (m_defUnitNames.empty()) m_defUnitNames.push_back("Def_Ghost");

        // ทอยเต๋าแยกทีละตัวเก็บไว้เลยเพื่อเอาไปวาด
        for (size_t i = 0; i < m_atkUnitNames.size(); ++i) m_atkDiceValues.push_back((std::rand() % 6) + 1);
        for (size_t i = 0; i < m_defUnitNames.size(); ++i) m_defDiceValues.push_back((std::rand() % 6) + 1);

        // คำนวณ m_finalAtkRoll, m_finalDefRoll ใหม่ (แต้มสูงสุดของกองทัพ) เพื่อตัดสินผล
        m_finalAtkRoll = 0;
        for (int v : m_atkDiceValues) if (v > m_finalAtkRoll) m_finalAtkRoll = v;
        m_finalDefRoll = 0;
        for (int v : m_defDiceValues) if (v > m_finalDefRoll) m_finalDefRoll = v;

        m_diceCalculated = true; // ป้องกันการนับซ้ำ
    }

    float elapsed = m_animTimer.getElapsedTime().asSeconds();

    if (elapsed > 2.5f) {
        m_isRolling = false;
        diceSound.stop();
        hitSound.play();

        auto atkIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == m_atkStartR && u.getC() == m_atkStartC && u.getOwner() == m_attackerOwner; });
        auto defIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == m_defTargetR && u.getC() == m_defTargetC && u.getOwner() != m_attackerOwner; });

        if (atkIt != units.end() && defIt != units.end()) {
            if (m_finalAtkRoll > m_finalDefRoll) {
                // ระบบปล้นเสบียง: ฝ่ายบุกชนะ!
                if (m_attackerOwner == 1) {
                    City* myCity = worldMap.getFirstCity();
                    if (myCity) {
                        myCity->addGold(20);
                        myCity->addFood(50);
                        std::cout << "[COMBAT] Victory! Looted 20 Gold and 50 Food from Goblin corpse!\n";
                    }
                }

                units.erase(defIt);

                bool tileEmpty = true;
                for (auto& u : units) {
                    if (u.getR() == m_defTargetR && u.getC() == m_defTargetC && u.getOwner() != m_attackerOwner) {
                        tileEmpty = false; break;
                    }
                }

                if (tileEmpty) {
                    if (m_isArmyAttack) {
                        for (auto& u : units) {
                            if (u.getR() == m_atkStartR && u.getC() == m_atkStartC && u.getOwner() == m_attackerOwner) {
                                u.moveTo(m_defTargetR, m_defTargetC); u.consumeAP(1);
                            }
                        }
                    }
                    else {
                        auto singleAtk = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == m_atkStartR && u.getC() == m_atkStartC && u.getOwner() == m_attackerOwner; });
                        if (singleAtk != units.end()) { singleAtk->moveTo(m_defTargetR, m_defTargetC); singleAtk->consumeAP(1); }
                    }
                    if (m_attackerOwner == 1) worldMap.revealFog(m_defTargetR, m_defTargetC, 1);
                }
                else {
                    if (m_isArmyAttack) {
                        for (auto& u : units) {
                            if (u.getR() == m_atkStartR && u.getC() == m_atkStartC && u.getOwner() == m_attackerOwner) u.consumeAP(1);
                        }
                    }
                    else {
                        auto singleAtk = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == m_atkStartR && u.getC() == m_atkStartC && u.getOwner() == m_attackerOwner; });
                        if (singleAtk != units.end()) singleAtk->consumeAP(1);
                    }
                }
            }
            else {
                // ระบบปล้นเสบียง: ฝ่ายรับชนะ! (คนบุกตาย)
                if (m_attackerOwner == 2) {
                    City* myCity = worldMap.getFirstCity();
                    if (myCity) {
                        myCity->addGold(20);
                        myCity->addFood(50);
                        std::cout << "[COMBAT] Defense Successful! Looted 20 Gold and 50 Food from dead Goblin attacker!\n";
                    }
                }

                units.erase(atkIt);
            }
        }
        return;
    }

    if (!m_hasFont) return;

    sf::RectangleShape darkOverlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    darkOverlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(darkOverlay);

    // ==============================================================
    // วาดหน้าจอการต่อสู้ใหม่แบบแยกตัว
    // ==============================================================
    std::string titleStr = "COMBAT PHASE";
    sf::Text titleText(titleStr, m_font, 50);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setPosition(window.getSize().x / 2.0f - 200, 50);
    window.draw(titleText);

    // --- [NEW] เปลี่ยนชื่อเป็น GOBLIN ---
    std::string atkTitle = (m_attackerOwner == 1 ? "PLAYER (Attacker)" : "GOBLIN (Attacker)");
    std::string defTitle = (m_attackerOwner == 1 ? "GOBLIN (Defender)" : "PLAYER (Defender)");

    // วาด VS ตรงกลาง
    sf::Text VS_text("VS", m_font, 50);
    VS_text.setFillColor(sf::Color::White);
    VS_text.setPosition(window.getSize().x / 2.0f - 30, 140);
    window.draw(VS_text);

    // ฝั่งบุก (Attacker) - ด้านซ้าย
    sf::Text atkSideText(atkTitle, m_font, 30);
    atkSideText.setFillColor(sf::Color(255, 100, 100));
    atkSideText.setPosition(window.getSize().x / 4.0f - 100, 150);
    window.draw(atkSideText);

    // ฝั่งรับ (Defender) - ด้านขวา
    sf::Text defSideText(defTitle, m_font, 30);
    defSideText.setFillColor(sf::Color(100, 150, 255));
    defSideText.setPosition(window.getSize().x * 3.0f / 4.0f - 100, 150);
    window.draw(defSideText);

    float startY = 220.0f; // จุดเริ่มวาดรายการแรก
    float spacingY = 50.0f; // ระยะห่างแนวตั้ง

    // วนวาดรายการ Attacker ทีละตัว
    for (size_t i = 0; i < m_atkUnitNames.size(); ++i) {
        // วาดชื่อทหาร
        sf::Text nameText(m_atkUnitNames[i], m_font, 25);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition(window.getSize().x / 4.0f - 150, startY + i * spacingY);
        window.draw(nameText);

        // วาดลูกเต๋า (ถ้ายังทอยอยู่ให้สุ่มโชว์ ถ้าทอยเสร็จให้โชว์แต้มจริง)
        sf::Text diceText;
        if (elapsed < 1.5f) diceText.setString(std::to_string((std::rand() % 6) + 1));
        else diceText.setString(std::to_string(m_atkDiceValues[i]));
        diceText.setFont(m_font);
        diceText.setCharacterSize(25);
        diceText.setFillColor(sf::Color(255, 215, 0)); // สีทอง
        diceText.setPosition(window.getSize().x / 4.0f + 100, startY + i * spacingY);
        window.draw(diceText);
    }

    // วนวาดรายการ Defender ทีละตัว
    for (size_t i = 0; i < m_defUnitNames.size(); ++i) {
        // วาดชื่อทหาร
        sf::Text nameText(m_defUnitNames[i], m_font, 25);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition(window.getSize().x * 3.0f / 4.0f - 150, startY + i * spacingY);
        window.draw(nameText);

        // วาดลูกเต๋า
        sf::Text diceText;
        if (elapsed < 1.5f) diceText.setString(std::to_string((std::rand() % 6) + 1));
        else diceText.setString(std::to_string(m_defDiceValues[i]));
        diceText.setFont(m_font);
        diceText.setCharacterSize(25);
        diceText.setFillColor(sf::Color(255, 215, 0)); // สีทอง
        diceText.setPosition(window.getSize().x * 3.0f / 4.0f + 100, startY + i * spacingY);
        window.draw(diceText);
    }

    // ส่วนแสดงผลสรุป (ย้ายลงมาล่างสุด)
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
        // ย้ายตำแหน่งไปอยู่ล่างๆ หน้าจอ
        resultText.setPosition(window.getSize().x / 2.0f - 220, window.getSize().y - 150);
        window.draw(resultText);
    }
}