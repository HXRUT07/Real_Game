#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Unit.h"
#include "GameMap.h"
#include "TurnManager.h"
#include "CombatManager.h"

class AIManager {
private:
    int aiBaseR, aiBaseC;
    int aiGold, aiWood, aiFood;
    int aiCityLevel;

    sf::Clock aiTimer;
    int hexDistance(int r1, int c1, int r2, int c2) const;

    // ระบบตัดสินใจเชิงกลยุทธ์
    int  countAIUnits(const std::vector<Unit>& units) const;
    int  countRalliedUnits(const std::vector<Unit>& units) const; // นับตัวที่รวมพลแล้ว
    bool shouldUpgradeCity(const std::vector<Unit>& units) const;
    bool shouldRecruitUnit(const std::vector<Unit>& units) const;
    bool shouldLaunchAttack(const std::vector<Unit>& units) const; // ถึงเวลาบุกหรือยัง

    // ระบบรวมพล (Rally)
    bool m_isAttackMode = false; // false = รวมพลอยู่, true = โหมดบุก
    int  m_rallyR = 0, m_rallyC = 0; // จุดนัดรวมพล

public:
    AIManager();
    void initBase(int r, int c);

    // Getter สำหรับ main.cpp ที่ยังใช้ตัวแปร aiFood โดยตรง (UpkeepManager)
    int& getAIFood() { return aiFood; }

    bool processTurn(std::vector<Unit>& units, GameMap& worldMap, TurnManager& turnSys, CombatManager& combatSys, sf::Sound& sndMove, sf::Sound& sndDice);
};
