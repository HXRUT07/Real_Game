#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

// เปรมทำ - ระบบก้อนเมฆ pixel ลอยผ่านจอ
struct CloudPuff {
    sf::Vector2f offset; // ตำแหน่งของ puff แต่ละก้อนใน cluster
    float radius;        // ขนาด
};

class Cloud {
private:
    sf::Vector2f position;     // ตำแหน่งหลัก
    float speed;               // ความเร็วเคลื่อนที่
    float alpha;               // ความโปร่งใส
    bool movingRight;          // ทิศทาง
    float screenWidth;
    float screenHeight;
    std::vector<CloudPuff> puffs; // กลุ่ม pixel puffs

public:
    Cloud(float screenW, float screenH);
    void reset(bool spawnOffscreen); // สุ่มใหม่
    void update(float dt);
    void draw(sf::RenderWindow& window, float zoomLevel);
    bool isOffscreen() const;
};

class CloudSystem {
private:
    std::vector<Cloud> clouds;
    float screenWidth;
    float screenHeight;

public:
    CloudSystem(float screenW, float screenH, int count = 6);
    void update(float dt);
    void draw(sf::RenderWindow& window, float zoomLevel);
};
// เปรมทำ - จบ