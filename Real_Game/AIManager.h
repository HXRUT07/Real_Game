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
    int hexDistance(int r1, int c1, int r2, int c2);

public:
    AIManager();
    void initBase(int r, int c);

    bool processTurn(std::vector<Unit>& units, GameMap& worldMap, TurnManager& turnSys, CombatManager& combatSys, sf::Sound& sndMove, sf::Sound& sndDice);
};