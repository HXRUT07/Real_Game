#include "AIManager.h"
#include <cmath>
#include <iostream>
#include <algorithm>

// ค่าใช้จ่ายอัพเกรดเมือง (ตรงกับ City.cpp)
static const int UPGRADE_GOLD[4] = { 100, 200, 400, 800 };
static const int UPGRADE_WOOD[4] = {  80, 150, 300, 600 };
static const int UPGRADE_FOOD[4] = {  50, 100, 200, 400 };

// ค่าใช้จ่ายเกณฑ์ทหาร (ตรงกับ main.cpp)
static const int RECRUIT_GOLD = 20;
static const int RECRUIT_FOOD = 50;

// จำนวนทหารขั้นต่ำก่อนบุก = ครึ่งหนึ่งของโควต้า แต่ต้องมีอย่างน้อย 2 ตัว
static const int MIN_RALLY_RATIO_NUM = 2; // ตัวเศษ
static const int MIN_RALLY_RATIO_DEN = 2; // ตัวส่วน (ครึ่งหนึ่ง)

// ระยะห่างจากบ้านที่ถือว่า "รวมพลแล้ว"
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
    // จุดนัดรวมพลอยู่ห่างจากบ้าน 3 ช่องไปทางแผนที่กลาง
    m_rallyR = std::max(0, std::min(49, r + 3));
    m_rallyC = std::max(0, std::min(49, c));
}

int AIManager::hexDistance(int r1, int c1, int r2, int c2) const {
    int ac1 = c1 - (r1 - (r1 & 1)) / 2;
    int ac2 = c2 - (r2 - (r2 & 1)) / 2;
    return (std::abs(r1 - r2) + std::abs(ac1 - ac2) + std::abs((r1 - r2) + (ac1 - ac2))) / 2;
}

// นับทหาร AI ทั้งหมด
int AIManager::countAIUnits(const std::vector<Unit>& units) const {
    int count = 0;
    for (const auto& u : units)
        if (u.getOwner() == 2) count++;
    return count;
}

// นับทหารที่รวมพลแล้ว (อยู่ในระยะ RALLY_RADIUS จากจุดนัดรวมพล)
int AIManager::countRalliedUnits(const std::vector<Unit>& units) const {
    int count = 0;
    for (const auto& u : units) {
        if (u.getOwner() == 2) {
            int dist = hexDistance(u.getR(), u.getC(), m_rallyR, m_rallyC);
            if (dist <= RALLY_RADIUS) count++;
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
    int maxUnits  = aiCityLevel * 2;
    return unitCount < maxUnits;
}

// ตัดสินใจว่าถึงเวลาบุกหรือยัง
// เงื่อนไข: ทหารที่รวมพลแล้วต้องถึงครึ่งหนึ่งของโควต้า
bool AIManager::shouldLaunchAttack(const std::vector<Unit>& units) const {
    int quota    = aiCityLevel * 2;
    int minNeeded = std::max(MIN_RALLY_RATIO_NUM, quota / MIN_RALLY_RATIO_DEN);
    int rallied  = countRalliedUnits(units);
    return rallied >= minNeeded;
}

bool AIManager::processTurn(std::vector<Unit>& units, GameMap& worldMap, TurnManager& turnSys, CombatManager& combatSys, sf::Sound& sndMove, sf::Sound& sndDice) {
    if (combatSys.isCombatActive()) return false;

    if (aiTimer.getElapsedTime().asSeconds() > 0.5f) {
        bool aiMovedThisTick = false;

        int evenDir[6][2] = { {-1,-1}, {-1,0}, {0,-1}, {0,1}, {1,-1}, {1,0} };
        int oddDir[6][2]  = { {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,0}, {1,1} };

        // ================================================================
        // ช่วงที่ 1 — ตัดสินใจเชิงกลยุทธ์
        // ================================================================

        // เกณฑ์ทหารก่อน ถ้าจำนวนยังไม่ถึงโควต้า
        if (shouldRecruitUnit(units)) {
            aiGold -= RECRUIT_GOLD;
            aiFood -= RECRUIT_FOOD;
            units.emplace_back("Enemy_Lv" + std::to_string(aiCityLevel), aiBaseR, aiBaseC, 2);
            std::cout << "[AI] Recruited unit! (" << countAIUnits(units) << " total)\n";
        }
        // อัพเมืองถ้ามีทหารพอและทรัพยากรเหลือเฟือ
        else if (shouldUpgradeCity(units)) {
            int idx = aiCityLevel - 1;
            aiGold -= UPGRADE_GOLD[idx];
            aiWood -= UPGRADE_WOOD[idx];
            aiFood -= UPGRADE_FOOD[idx];
            aiCityLevel++;
            // อัพเมืองแล้วให้รีเซ็ตกลับไปรวมพลใหม่ด้วยกองที่ใหญ่ขึ้น
            m_isAttackMode = false;
            std::cout << "[AI] City upgraded to level " << aiCityLevel << "! Regrouping...\n";
        }

        // ================================================================
        // ช่วงที่ 2 — อัปเดตโหมด (รวมพล vs บุก)
        // ================================================================
        if (!m_isAttackMode) {
            // ตรวจว่ารวมพลครบหรือยัง
            if (shouldLaunchAttack(units)) {
                m_isAttackMode = true;
                std::cout << "[AI] Rally complete! Launching attack with "
                          << countRalliedUnits(units) << " units!\n";
            }
        }
        else {
            // อยู่ในโหมดบุก — ถ้าทหารหายไปจนต่ำกว่าเกณฑ์ให้ถอยรวมพลใหม่
            if (countRalliedUnits(units) == 0 && countAIUnits(units) > 0) {
                m_isAttackMode = false;
                std::cout << "[AI] Casualties too high! Retreating to rally...\n";
            }
        }

        // ================================================================
        // ช่วงที่ 3 — เดินทหารทีละตัวตามโหมด
        // ================================================================
        for (size_t i = 0; i < units.size(); ++i) {
            if (units[i].getOwner() != 2 || units[i].getCurrentAP() <= 0) continue;

            // ฟาร์มทรัพยากรของช่องที่ยืนอยู่
            HexTile* currentTile = worldMap.getTile(units[i].getR(), units[i].getC());
            if (currentTile && (currentTile->gold > 0 || currentTile->wood > 0 || currentTile->food > 0)) {
                aiGold += currentTile->gold;
                aiWood += currentTile->wood;
                aiFood += currentTile->food;
                currentTile->gold = 0; currentTile->wood = 0; currentTile->food = 0;
            }

            int targetR = -1, targetC = -1;
            int minDist = 999999;

            // --- หาเป้าหมาย ขึ้นอยู่กับโหมดปัจจุบัน ---
            if (m_isAttackMode) {
                // โหมดบุก: วิ่งหาศัตรูทุกตัวที่อยู่ใกล้ที่สุด
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
                // ถ้าหาศัตรูไม่เจอเลยในแผนที่ ให้รอที่จุดรวมพล
                if (targetR == -1) {
                    targetR = m_rallyR;
                    targetC = m_rallyC;
                }
            }
            else {
                // โหมดรวมพล: ตรวจก่อนว่ารวมพลครบยัง
                int distToRally = hexDistance(units[i].getR(), units[i].getC(), m_rallyR, m_rallyC);
                bool isRallied  = (distToRally <= RALLY_RADIUS);

                if (!isRallied) {
                    // ยังไม่ถึงจุดรวมพล → เดินไปก่อน
                    targetR = m_rallyR;
                    targetC = m_rallyC;
                }
                else {
                    // รวมพลแล้ว — ฟาร์มรอบๆ จุดรวมพลระหว่างรอเพื่อน
                    bool foundResource = false;
                    for (int r = 0; r < 50 && !foundResource; r++) {
                        for (int c = 0; c < 50 && !foundResource; c++) {
                            HexTile* t = worldMap.getTile(r, c);
                            if (t && (t->gold > 0 || t->wood > 0 || t->food > 0)) {
                                int dist = hexDistance(units[i].getR(), units[i].getC(), r, c);
                                // ฟาร์มเฉพาะของที่อยู่ในระยะ RALLY_RADIUS จากจุดรวมพล
                                // เพื่อไม่ให้ทหารแตกกลุ่มออกไปไกล
                                int distFromRally = hexDistance(r, c, m_rallyR, m_rallyC);
                                if (distFromRally <= RALLY_RADIUS && dist < minDist) {
                                    minDist = dist;
                                    targetR = r; targetC = c;
                                    foundResource = true;
                                }
                            }
                        }
                    }
                    // ถ้าไม่มีของรอบๆ ก็ยืนรอที่จุดรวมพลเฉยๆ
                    if (!foundResource) {
                        targetR = m_rallyR;
                        targetC = m_rallyC;
                    }
                }
            }

            // ถ้ายังหาเป้าไม่ได้เลย ให้วนเวียนรอบบ้าน
            if (targetR == -1) {
                targetR = std::max(0, std::min(49, aiBaseR + (std::rand() % 7 - 3)));
                targetC = std::max(0, std::min(49, aiBaseC + (std::rand() % 7 - 3)));
            }

            // --- หาช่องที่เดินแล้วเข้าใกล้เป้าหมายมากที่สุด ---
            int bestR    = units[i].getR(), bestC = units[i].getC();
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

            // --- เดิน หรือ โจมตี ---
            if (bestR != units[i].getR() || bestC != units[i].getC()) {
                auto playerIt = std::find_if(units.begin(), units.end(),
                    [&](Unit& u) { return u.getR() == bestR && u.getC() == bestC && u.getOwner() == 1; });

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
                // ติดขัดเดินไม่ได้ — หมด AP ทิ้ง
                units[i].consumeAP(units[i].getCurrentAP());
            }

            aiMovedThisTick = true;
            break; // เดินทีละตัวต่อ tick เพื่อให้เห็นภาพ
        }

        if (!aiMovedThisTick) {
            turnSys.endTurn(units);
            return true;
        }

        aiTimer.restart();
    }
    return false;
}
