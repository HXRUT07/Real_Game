#include "AIManager.h"
#include <cmath>
#include <iostream>
#include <algorithm>

// ค่าใช้จ่ายอัพเกรดเมือง (ตรงกับ City.cpp)
static const int UPGRADE_GOLD[4] = { 100, 200, 400, 800 };
static const int UPGRADE_WOOD[4] = { 80, 150, 300, 600 };
static const int UPGRADE_FOOD[4] = { 50, 100, 200, 400 };

// ค่าใช้จ่ายเกณฑ์ทหาร (ตรงกับ main.cpp)
static const int RECRUIT_GOLD = 20;
static const int RECRUIT_FOOD = 50;

static const int MIN_RALLY_RATIO_NUM = 2;
static const int MIN_RALLY_RATIO_DEN = 2;
static const int RALLY_RADIUS = 3;

AIManager::AIManager() {
    aiBaseR = 0; aiBaseC = 0;
    aiGold = 0; aiWood = 0; aiFood = 0;
    aiCityLevel = 1;
    m_isAttackMode = false;
    m_rallyR = 0; m_rallyC = 0;
}

void AIManager::initBase(int r, int c) {
    aiBaseR = r;
    aiBaseC = c;
    m_rallyR = std::max(0, std::min(49, r + (r > 25 ? -3 : 3)));
    m_rallyC = std::max(0, std::min(49, c + (c > 25 ? -3 : 3)));

    aiGold = 100;
    aiWood = 50;
    aiFood = 50;
}

int AIManager::hexDistance(int r1, int c1, int r2, int c2) const {
    int ac1 = c1 - (r1 - (r1 & 1)) / 2;
    int ac2 = c2 - (r2 - (r2 & 1)) / 2;
    return (std::abs(r1 - r2) + std::abs(ac1 - ac2) + std::abs((r1 - r2) + (ac1 - ac2))) / 2;
}

int AIManager::countAIUnits(const std::vector<Unit>& units) const {
    int count = 0;
    for (const auto& u : units)
        if (u.getOwner() == 2) count++;
    return count;
}

int AIManager::countRalliedUnits(const std::vector<Unit>& units) const {
    int count = 0;
    for (const auto& u : units) {
        if (u.getOwner() == 2) {
            int dist = hexDistance(u.getR(), u.getC(), m_rallyR, m_rallyC);
            if (dist <= RALLY_RADIUS + 2) count++;
        }
    }
    return count;
}

bool AIManager::shouldUpgradeCity(const std::vector<Unit>& units) const {
    if (aiCityLevel >= 5) return false;
    int idx = aiCityLevel - 1;
    bool hasEnoughRes = aiGold >= UPGRADE_GOLD[idx]
        && aiWood >= UPGRADE_WOOD[idx]
        && aiFood >= UPGRADE_FOOD[idx];
    if (!hasEnoughRes) return false;
    return countAIUnits(units) >= 1;
}

bool AIManager::shouldRecruitUnit(const std::vector<Unit>& units) const {
    if (aiGold < RECRUIT_GOLD || aiFood < RECRUIT_FOOD) return false;
    int unitCount = countAIUnits(units);
    int maxUnits = aiCityLevel * 2;
    return unitCount < maxUnits;
}

bool AIManager::shouldLaunchAttack(const std::vector<Unit>& units) const {
    int quota = aiCityLevel * 2;
    int minNeeded = std::max(MIN_RALLY_RATIO_NUM, quota / MIN_RALLY_RATIO_DEN);
    int rallied = countRalliedUnits(units);
    return rallied >= minNeeded;
}

bool AIManager::processTurn(std::vector<Unit>& units, GameMap& worldMap, TurnManager& turnSys, CombatManager& combatSys, sf::Sound& sndMove, sf::Sound& sndDice) {
    if (combatSys.isCombatActive()) return false;

    if (aiTimer.getElapsedTime().asSeconds() > 0.5f) {
        bool aiMovedThisTick = false;

        int evenDir[6][2] = { {-1,-1}, {-1,0}, {0,-1}, {0,1}, {1,-1}, {1,0} };
        int oddDir[6][2] = { {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,0}, {1,1} };

        // 1. เกณฑ์ทหารหรืออัปเกรดเมือง --- [NEW] เปลี่ยนชื่อเป็น Goblin
        if (shouldRecruitUnit(units)) {
            aiGold -= RECRUIT_GOLD;
            aiFood -= RECRUIT_FOOD;
            units.emplace_back("Goblin_Lv" + std::to_string(aiCityLevel), aiBaseR, aiBaseC, 2);
            std::cout << "[GOBLIN] Recruited unit! (" << countAIUnits(units) << " total)\n";
        }
        else if (shouldUpgradeCity(units)) {
            int idx = aiCityLevel - 1;
            aiGold -= UPGRADE_GOLD[idx];
            aiWood -= UPGRADE_WOOD[idx];
            aiFood -= UPGRADE_FOOD[idx];
            aiCityLevel++;
            m_isAttackMode = false;
            std::cout << "[GOBLIN] City upgraded to level " << aiCityLevel << "! Regrouping...\n";
        }

        // 2. เช็คโหมดบุก
        if (!m_isAttackMode) {
            if (shouldLaunchAttack(units)) {
                m_isAttackMode = true;
                std::cout << "[GOBLIN] Rally complete! Launching attack with "
                    << countRalliedUnits(units) << " units!\n";
            }
        }
        else {
            if (countRalliedUnits(units) == 0 && countAIUnits(units) > 0) {
                m_isAttackMode = false;
                std::cout << "[GOBLIN] Casualties too high! Retreating to rally...\n";
            }
        }

        // 3. เริ่มเดินทหารทีละตัว
        for (size_t i = 0; i < units.size(); ++i) {
            if (units[i].getOwner() != 2 || units[i].getCurrentAP() <= 0) continue;

            HexTile* currentTile = worldMap.getTile(units[i].getR(), units[i].getC());
            if (currentTile && (currentTile->gold > 0 || currentTile->wood > 0 || currentTile->food > 0)) {
                aiGold += currentTile->gold;
                aiWood += currentTile->wood;
                aiFood += currentTile->food;
                currentTile->gold = 0; currentTile->wood = 0; currentTile->food = 0;
            }

            int targetR = -1, targetC = -1;
            int minDist = 999999;

            if (m_isAttackMode) {
                for (const auto& enemy : units) {
                    if (enemy.getOwner() == 1) {
                        int dist = hexDistance(units[i].getR(), units[i].getC(), enemy.getR(), enemy.getC());
                        if (dist < minDist) {
                            minDist = dist;
                            targetR = enemy.getR();
                            targetC = enemy.getC();
                        }
                    }
                }
                if (targetR == -1) {
                    targetR = m_rallyR;
                    targetC = m_rallyC;
                }
            }
            else {
                int distToRally = hexDistance(units[i].getR(), units[i].getC(), m_rallyR, m_rallyC);
                bool isRallied = (distToRally <= RALLY_RADIUS);

                if (!isRallied) {
                    targetR = m_rallyR;
                    targetC = m_rallyC;
                }
                else {
                    bool foundResource = false;
                    for (int r = 0; r < 50; r++) {
                        for (int c = 0; c < 50; c++) {
                            HexTile* t = worldMap.getTile(r, c);
                            if (t && (t->gold > 0 || t->wood > 0 || t->food > 0)) {
                                int dist = hexDistance(units[i].getR(), units[i].getC(), r, c);
                                int distFromRally = hexDistance(r, c, m_rallyR, m_rallyC);
                                if (distFromRally <= RALLY_RADIUS + 3 && dist < minDist) {
                                    minDist = dist;
                                    targetR = r; targetC = c;
                                    foundResource = true;
                                }
                            }
                        }
                    }
                    if (!foundResource) {
                        if (hexDistance(units[i].getR(), units[i].getC(), m_rallyR, m_rallyC) == 0) {
                            targetR = std::max(0, std::min(49, m_rallyR + (std::rand() % 5 - 2)));
                            targetC = std::max(0, std::min(49, m_rallyC + (std::rand() % 5 - 2)));
                        }
                        else {
                            targetR = m_rallyR;
                            targetC = m_rallyC;
                        }
                    }
                }
            }

            if (targetR == -1) {
                targetR = std::max(0, std::min(49, aiBaseR + (std::rand() % 7 - 3)));
                targetC = std::max(0, std::min(49, aiBaseC + (std::rand() % 7 - 3)));
            }

            int bestR = units[i].getR(), bestC = units[i].getC();
            int bestDist = hexDistance(units[i].getR(), units[i].getC(), targetR, targetC);

            for (int dir = 0; dir < 6; ++dir) {
                int nr = units[i].getR() + (units[i].getR() % 2 == 0 ? evenDir[dir][0] : oddDir[dir][0]);
                int nc = units[i].getC() + (units[i].getR() % 2 == 0 ? evenDir[dir][1] : oddDir[dir][1]);
                HexTile* nTile = worldMap.getTile(nr, nc);
                if (nTile != nullptr) {
                    int d = hexDistance(nr, nc, targetR, targetC);
                    if (d < bestDist) { bestDist = d; bestR = nr; bestC = nc; }
                }
            }

            if (bestR != units[i].getR() || bestC != units[i].getC()) {
                auto playerIt = std::find_if(units.begin(), units.end(),
                    [&](Unit& u) { return u.getR() == bestR && u.getC() == bestC && u.getOwner() == 1; });

                if (playerIt != units.end()) {
                    combatSys.initiateCombat(units[i].getR(), units[i].getC(), bestR, bestC, 2, sndDice, true);
                }
                else {
                    units[i].moveTo(bestR, bestC);
                    units[i].consumeAP(1);
                    sndMove.play();
                }
            }
            else {
                units[i].consumeAP(units[i].getCurrentAP());
            }

            aiMovedThisTick = true;
            break;
        }

        if (!aiMovedThisTick) {
            turnSys.endTurn(units);
            return true;
        }

        aiTimer.restart();
    }
    return false;
}