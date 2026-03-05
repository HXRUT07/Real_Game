#include "UpkeepManager.h"
#include <iostream>

void UpkeepManager::processPlayerUpkeep(std::vector<Unit>& units, City* playerCity) {
    if (playerCity == nullptr) return; // ถ้าเมืองพังหรือยังไม่สร้าง ก็ไม่ต้องทำอะไร

    int foodPerUnit = 10; // กิน 10 อาหารต่อตัว
    int fedCount = 0;
    int hungryCount = 0;

    for (auto& u : units) {
        if (u.getOwner() == 1) { // เฉพาะทหารเรา
            if (playerCity->getFood() >= foodPerUnit) {
                playerCity->addFood(-foodPerUnit); // กินอิ่ม
                fedCount++;
            }
            else {
                // อาหารหมด! ริบ AP คืน (ทหารหิว เดินไม่ได้)
                u.consumeAP(u.getCurrentAP());
                hungryCount++;
            }
        }
    }
    std::cout << "[UPKEEP] Player Fed: " << fedCount << " units | Hungry (AP=0): " << hungryCount << " units.\n";
}

void UpkeepManager::processAIUpkeep(std::vector<Unit>& units, int& aiFood) {
    int foodPerUnit = 10;
    int fedCount = 0;
    int hungryCount = 0;

    for (auto& u : units) {
        if (u.getOwner() == 2) { // เฉพาะทหาร AI
            if (aiFood >= foodPerUnit) {
                aiFood -= foodPerUnit; // กินอิ่ม
                fedCount++;
            }
            else {
                // AI ก็หิวได้เหมือนกัน!
                u.consumeAP(u.getCurrentAP());
                hungryCount++;
            }
        }
    }
    std::cout << "[UPKEEP] AI Fed: " << fedCount << " units | Hungry (AP=0): " << hungryCount << " units.\n";
}