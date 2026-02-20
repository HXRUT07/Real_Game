#pragma once
#include <vector>
#include "Unit.h"

class TurnManager {
public:
    TurnManager(int totalPlayers = 2);

    void endTurn(std::vector<Unit>& units);
    int getCurrentPlayer() const { return m_currentPlayer; }
    int getTurnNumber() const { return m_currentTurn; }

private:
    int m_currentPlayer;
    int m_currentTurn;
    int m_totalPlayers;
};