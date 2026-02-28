#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ResourceManage.h"
class City {
private:
    int gridR, gridC;
    sf::Vector2f center;

    // เปรมทำ - เปลี่ยนจาก RectangleShape เป็น Texture + Sprite
    sf::Texture castleTexture;
    sf::Sprite baseIcon;
    // เปรมทำ - จบ

    std::string name;
    bool upgraded = false;
    // ---  คลังหลวงเก็บทรัพยากรของเมือง ---
    ResourceYield stockpile;
public:
    // ลบ ResourceYield ในวงเล็บออก เพราะเราใช้คลังหลวงแทนแล้ว
    City(int r, int c, sf::Vector2f pos);
    int getR() const { return gridR; }
    int getC() const { return gridC; }
    void setName(const std::string& newName) { name = newName; }
    std::string getName() const { return name; }
    // ---ฟังก์ชันจัดการคลังหลวง ---
    int getGold() const { return stockpile.gold; }
    int getWood() const { return stockpile.wood; }
    int getFood() const { return stockpile.food; }
    void addGold(int amount) { stockpile.gold += amount; }
    void addWood(int amount) { stockpile.wood += amount; }
    void addFood(int amount) { stockpile.food += amount; }
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    std::string getCityInfo() const;
};