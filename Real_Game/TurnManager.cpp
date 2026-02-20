#include "TurnManager.h"
#include <iostream>

TurnManager::TurnManager(int totalPlayers) {
    m_totalPlayers = totalPlayers;
    m_currentPlayer = 1; // เริ่มเกมเป็นตาของ Player 1
    m_currentTurn = 1;   // รอบที่ 1
}

// ฟังก์ชันจบเทิร์น
void TurnManager::endTurn(std::vector<Unit>& units) {
    // 1. เปลี่ยนตัวผู้เล่น
    m_currentPlayer++;

    // 2. ถ้าเล่นครบทุกคนแล้ว ให้วนกลับไปคนที่ 1 และขึ้นรอบใหม่
    if (m_currentPlayer > m_totalPlayers) {
        m_currentPlayer = 1;
        m_currentTurn++;
    }

    std::cout << "========== TURN " << m_currentTurn << " ==========" << std::endl;
    std::cout << "Player " << m_currentPlayer << "'s Turn!" << std::endl;

    // 3. รีเซ็ต AP ให้ **เฉพาะทหารของผู้เล่นที่เพิ่งถึงตา**
    for (auto& u : units) {
        if (u.getOwner() == m_currentPlayer) {
            u.resetAP();
        }
    }
}