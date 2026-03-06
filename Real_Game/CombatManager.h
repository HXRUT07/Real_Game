#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector> 
#include <string>

#include "Unit.h"
#include "GameMap.h"

class CombatManager {
private:
    bool m_isRolling;
    bool m_diceCalculated;
    sf::Clock m_animTimer;
    int m_finalAtkRoll, m_finalDefRoll;
    int m_displayAtkRoll, m_displayDefRoll;
    int m_atkDiceCount, m_defDiceCount;
    int m_atkStartR, m_atkStartC;
    int m_defTargetR, m_defTargetC;
    int m_attackerOwner;
    bool m_isArmyAttack;

    std::vector<std::string> m_atkUnitNames;
    std::vector<std::string> m_defUnitNames;
    std::vector<int> m_atkDiceValues;
    std::vector<int> m_defDiceValues;

    sf::Font m_font;
    bool m_hasFont;

    int rollDiceRisk(int numDice);

public:
    CombatManager();
    void setFont(const sf::Font& font);
    bool isCombatActive() const { return m_isRolling; }

    void initiateCombat(int atkR, int atkC, int defR, int defC, int attackerOwner, sf::Sound& diceSound, bool isArmyAttack);
    void updateAndDraw(sf::RenderWindow& window, std::vector<Unit>& units, GameMap& worldMap, sf::Sound& diceSound, sf::Sound& hitSound);
};