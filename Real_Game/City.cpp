#include "City.h"
#include <sstream>
#include <cmath> // เปรมทำ - สำหรับ cos, sin
#include <algorithm> // เปรมทำ - สำหรับ std::max

const City::UpgradeCost City::upgradeCosts[4] = {
    { 100,  80,  50 },   // lv 1 -> 2
    { 200, 150, 100 },   // lv 2 -> 3
    { 400, 300, 200 },   // lv 3 -> 4
    { 800, 600, 400 },   // lv 4 -> 5
};

static const int MAX_BUILDINGS_PER_LEVEL[5] = { 2, 4, 7, 11, 16 };

City::City(int r, int c, sf::Vector2f pos)
    : gridR(r), gridC(c), center(pos)
{
    name = "New City";

    // เปรมทำ - โหลดรูปปราสาทและ clip เป็น hex shape
    if (castleTexture.loadFromFile("Blue Castle.png")) {
        float size = 30.0f * 2.0f;

        // เปรมทำ - สร้าง RenderTexture ขนาดเท่า hex
        hexMaskTexture.create((unsigned int)size, (unsigned int)size);
        hexMaskTexture.clear(sf::Color::Transparent);

        // เปรมทำ - วาด hex shape ก่อนเป็น mask
        sf::ConvexShape hexMask;
        hexMask.setPointCount(6);
        for (int i = 0; i < 6; ++i) {
            float angle = 60.f * i - 30.f;
            float rad = angle * 3.14159f / 180.f;
            hexMask.setPoint(i, sf::Vector2f(
                size / 2.f + (size / 2.f) * std::cos(rad),
                size / 2.f + (size / 2.f) * std::sin(rad)
            ));
        }
        hexMask.setFillColor(sf::Color::White);
        hexMaskTexture.draw(hexMask);

        // เปรมทำ - วาดรูปปราสาททับ โดยใช้ BlendMode ให้โชว์เฉพาะในพื้นที่ hex
        sf::Sprite castleSprite(castleTexture);
        sf::Vector2u texSize = castleTexture.getSize();
        castleSprite.setScale(size / texSize.x, size / texSize.y);

        sf::BlendMode blendDestIn(
            sf::BlendMode::DstAlpha,
            sf::BlendMode::Zero,
            sf::BlendMode::Add,
            sf::BlendMode::Zero,
            sf::BlendMode::SrcAlpha,
            sf::BlendMode::Add
        );
        hexMaskTexture.draw(castleSprite, sf::RenderStates(blendDestIn));

        hexMaskTexture.display();

        // เปรมทำ - ใช้ texture ที่ clip แล้ว
        hexMaskedSprite.setTexture(hexMaskTexture.getTexture());
        hexMaskedSprite.setOrigin(size / 2.f, size / 2.f);
    }
    hexMaskedSprite.setPosition(center);
    // เปรมทำ - จบ

    // ---ให้ทรัพยากรตั้งต้นนิดหน่อยตอนตั้งเมือง ---
    stockpile.gold = 100;
    stockpile.wood = 50;
    stockpile.food = 50;
}

City::UpgradeCost City::getUpgradeCost() const {
    if (isMaxLevel()) return { 0, 0, 0 };
    return upgradeCosts[level - 1]; // level 1 -> index 0
}

bool City::canUpgrade() const {
    if (isMaxLevel()) return false;
    UpgradeCost cost = getUpgradeCost();
    return stockpile.gold >= cost.gold &&
        stockpile.wood >= cost.wood &&
        stockpile.food >= cost.food;
}

bool City::upgrade() {
    if (!canUpgrade()) return false;
    UpgradeCost cost = getUpgradeCost();
    stockpile.gold -= cost.gold;
    stockpile.wood -= cost.wood;
    stockpile.food -= cost.food;
    level++;
    return true;
}

//นับจำนวน building รวมทั้งหมดในเมืองนี้
int City::getTotalBuildingCount() const {
    int total = 0;
    for (int i = 0; i < 4; i++) total += buildings[i].count;
    return total;
}

//จำนวน building รวมสูงสุดที่เลเวลปัจจุบัน
int City::getMaxTotalBuildings() const {
    return MAX_BUILDINGS_PER_LEVEL[level - 1];
}

//จำนวนสูงสุดของ building แต่ละประเภท = เลเวลเมือง
int City::getMaxPerBuilding() const {
    return level;
}

//เช็คว่าสร้าง building ได้ไหม
bool City::canBuild(int idx) const {
    if (idx < 0 || idx > 3) return false;
    //เช็ค building limit รวม
    if (getTotalBuildingCount() >= getMaxTotalBuildings()) return false;

    //เช็ค limit ต่อประเภท
    if (buildings[idx].count >= getMaxPerBuilding()) return false;

    BuildingCost cost = buildings[idx].getCost();
    return stockpile.gold >= cost.gold &&
        stockpile.wood >= cost.wood &&
        stockpile.food >= cost.food;
}

//สร้าง building 
bool City::build(int idx) {
    if (!canBuild(idx)) return false;
    BuildingCost cost = buildings[idx].getCost();
    stockpile.gold -= cost.gold;
    stockpile.wood -= cost.wood;
    stockpile.food -= cost.food;
    buildings[idx].count++;
    return true;
}

// เรียกทุกเทิร์น -> บวกผลผลิตเข้า stockpile
void City::produceTurn() {
    for (int i = 0; i < 4; i++) {
        stockpile.gold += buildings[i].goldPerTurn();
        stockpile.food += buildings[i].foodPerTurn();
        stockpile.wood += buildings[i].woodPerTurn();
    }
}

void City::draw(sf::RenderWindow& window)
{
    // เปรมทำ - วาด hexMaskedSprite แทน baseIcon
    window.draw(hexMaskedSprite);
    // เปรมทำ - จบ
}

sf::FloatRect City::getBounds() const
{
    // เปรมทำ
    return hexMaskedSprite.getGlobalBounds();
    // เปรมทำ - จบ
}

std::string City::getCityInfo() const {
    std::stringstream ss;
    ss << "City Name: " << name << "\n\n";
    ss << "Level: " << level << " / " << maxLevel << "\n\n";
    ss << "Wood: " << stockpile.wood << "\n";
    ss << "Gold: " << stockpile.gold << "\n";
    ss << "Food: " << stockpile.food << "\n\n";
    if (!isMaxLevel()) {
        UpgradeCost cost = getUpgradeCost();
        ss << "Upgrade Cost:\n";
        ss << "  Gold: " << cost.gold << "\n";
        ss << "  Wood: " << cost.wood << "\n";
        ss << "  Food: " << cost.food << "\n";
    }
    else {
        ss << "MAX LEVEL\n";
    }
    return ss.str();
}

ResourceYield City::getTotalResource() const {
    return stockpile;
}
