#include "AIManager.h"
#include <cmath>
#include <iostream>
#include <algorithm>

AIManager::AIManager() {
    aiBaseR = 0; aiBaseC = 0;
    aiGold = 0; aiWood = 0; aiFood = 0;
    aiCityLevel = 1;
}

void AIManager::initBase(int r, int c) {
    aiBaseR = r;
    aiBaseC = c;
}

int AIManager::hexDistance(int r1, int c1, int r2, int c2) {
    int ac1 = c1 - (r1 - (r1 & 1)) / 2;
    int ac2 = c2 - (r2 - (r2 & 1)) / 2;
    return (std::abs(r1 - r2) + std::abs(ac1 - ac2) + std::abs((r1 - r2) + (ac1 - ac2))) / 2;
}

bool AIManager::processTurn(std::vector<Unit>& units, GameMap& worldMap, TurnManager& turnSys, CombatManager& combatSys, sf::Sound& sndMove, sf::Sound& sndDice) {
    if (combatSys.isCombatActive()) return false;

    if (aiTimer.getElapsedTime().asSeconds() > 0.5f) {
        bool aiMovedThisTick = false;

        int evenDir[6][2] = { {-1,-1}, {-1,0}, {0,-1}, {0,1}, {1,-1}, {1,0} };
        int oddDir[6][2] = { {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,0}, {1,1} };

        // อัปบ้าน
        if (aiGold >= 100 && aiWood >= 50 && aiFood >= 50) {
            aiGold -= 100; aiWood -= 50; aiFood -= 50;
            aiCityLevel++;
            units.emplace_back("Enemy_Lv" + std::to_string(aiCityLevel), aiBaseR, aiBaseC, 2);
        }

        for (size_t i = 0; i < units.size(); ++i) {
            if (units[i].getOwner() == 2 && units[i].getCurrentAP() > 0) {

                // ฟาร์ม
                HexTile* currentTile = worldMap.getTile(units[i].getR(), units[i].getC());
                if (currentTile && (currentTile->gold > 0 || currentTile->wood > 0 || currentTile->food > 0)) {
                    aiGold += currentTile->gold; aiWood += currentTile->wood; aiFood += currentTile->food;
                    currentTile->gold = 0; currentTile->wood = 0; currentTile->food = 0;
                }

                int targetR = -1, targetC = -1;
                int minDist = 999999;
                bool enemyInSight = false;

                // หาศัตรู (สายตา 5)
                for (auto& enemy : units) {
                    if (enemy.getOwner() == 1) {
                        int dist = hexDistance(units[i].getR(), units[i].getC(), enemy.getR(), enemy.getC());
                        if (dist <= 5) {
                            if (dist < minDist) {
                                minDist = dist; targetR = enemy.getR(); targetC = enemy.getC();
                                enemyInSight = true;
                            }
                        }
                    }
                }

                // หาของ
                if (!enemyInSight) {
                    minDist = 999999;
                    for (int r = 0; r < 50; r++) {
                        for (int c = 0; c < 50; c++) {
                            HexTile* t = worldMap.getTile(r, c);
                            if (t && (t->gold > 0 || t->wood > 0 || t->food > 0)) {
                                int dist = hexDistance(units[i].getR(), units[i].getC(), r, c);
                                if (dist < minDist) { minDist = dist; targetR = r; targetC = c; }
                            }
                        }
                    }
                }

                if (targetR == -1) {
                    targetR = std::max(0, std::min(49, aiBaseR + (std::rand() % 11 - 5)));
                    targetC = std::max(0, std::min(49, aiBaseC + (std::rand() % 11 - 5)));
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
                    auto playerIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == bestR && u.getC() == bestC && u.getOwner() == 1; });
                    if (playerIt != units.end()) {
                        // AI โจมตี (isArmyAttack = true)
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
        }

        if (!aiMovedThisTick) {
            turnSys.endTurn(units);
            return true;
        }

        aiTimer.restart();
    }
    return false;
}