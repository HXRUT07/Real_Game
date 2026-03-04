#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

// ประกาศชื่อล่วงหน้า ป้องกันงูกินหาง
class Unit;
class GameMap;
class TurnManager;
class CombatManager;

class AIManager {
private:
    int aiBaseR, aiBaseC;
    int aiGold, aiWood, aiFood;
    int aiCityLevel;
    bool aiIsExploring;
    sf::Clock aiTimer;

    int hexDistance(int r1, int c1, int r2, int c2);

public:
    AIManager();
    void initBase(int r, int c);

    // คืนค่า true ถ้ายูนิต AI ทุกตัวใช้ AP หมดแล้ว (จบเทิร์น)
    bool processTurn(std::vector<Unit>& units, GameMap& worldMap, TurnManager& turnSys, CombatManager& combatSys, sf::Sound& sndMove, sf::Sound& sndDice);
};