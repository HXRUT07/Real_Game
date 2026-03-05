#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ResourceManage.h"   

class City {
private:
    int gridR, gridC;
    sf::Vector2f center;
    sf::Texture castleTexture;
    sf::Sprite baseIcon;
    sf::RenderTexture hexMaskTexture;
    sf::Sprite hexMaskedSprite;
    std::string name;
    int level = 1;
    int maxLevel = 5;
    ResourceYield stockpile;

    // ค่าใช้จ่ายในการอัพเกรดแต่ละเลเวล (index 0 = lv1->2, index 3 = lv4->5)
    struct UpgradeCost { int gold, wood, food; };
    static const UpgradeCost upgradeCosts[4];

public:
    City(int r, int c, sf::Vector2f pos);
    City(const City&) = delete;
    City& operator=(const City&) = delete;

    int getR() const { return gridR; }
    int getC() const { return gridC; }
    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    // Level
    int getLevel() const { return level; }
    int getMaxLevel() const { return maxLevel; }
    bool isMaxLevel() const { return level >= maxLevel; }

    // ดึงค่าใช้จ่ายของเลเวลปัจจุบัน
    UpgradeCost getUpgradeCost() const;

    // เช็คว่าทรัพยากรพอหรือเปล่า
    bool canUpgrade() const;

    // อัพเกรด (หักทรัพยากรเอง) - คืน true ถ้าสำเร็จ
    bool upgrade();

    // Stockpile
    int getGold() const { return stockpile.gold; }
    int getWood() const { return stockpile.wood; }
    int getFood() const { return stockpile.food; }
    void addGold(int amount) { stockpile.gold += amount; }
    void addWood(int amount) { stockpile.wood += amount; }
    void addFood(int amount) { stockpile.food += amount; }
    ResourceYield getTotalResource() const;

    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    std::string getCityInfo() const;
};