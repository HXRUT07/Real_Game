#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "City.h"

// ขนาดของ Hexagon
const float HEX_SIZE = 30.0f;

// ประเภทของพื้นที่
enum class TerrainType {
    Grass,
    Water,
    Mountain,
    Forest
};

// โครงสร้างข้อมูลของแต่ละช่อง
struct HexTile {
    sf::ConvexShape shape;
    int gridR = 0, gridC = 0;
    TerrainType type = TerrainType::Grass;

    bool isHovered = false;   // สถานะเมาส์ชี้ (กรอบขาว)
    bool isExplored = false;  // สถานะหมอก (false = มืด/มองไม่เห็น)
    City* city = nullptr; // ถ้ามีเมืองอยู่บนช่องนี้ จะชี้ไปที่ City Object
};

class GameMap {
public:
    // Constructor
    GameMap(int r, int c);
    ~GameMap();

    // ฟังก์ชันวาดและอัปเดตพื้นฐาน
    void draw(sf::RenderWindow& window);
    void updateHighlight(sf::Vector2f mousePos);

    // [ใหม่] ฟังก์ชันรับการคลิกจาก Main (เพื่อเลือกจุดเกิด)
    void handleMouseClick(sf::Vector2f mousePos);

    // [ใหม่] เช็คว่าเกมเริ่มหรือยัง (Main อาจจะอยากรู้)
    bool isGameStarted() const { return m_gameStarted; }

private:
    // ตัวแปรเก็บข้อมูล
    std::vector<HexTile> tiles;
    int rows;
    int cols;

    // [ใหม่] ตัวแปรเช็คสถานะ (True = เลือกจุดเกิดแล้ว/กำลังเล่น, False = รอเลือกจุดเกิด)
    bool m_gameStarted = false;

    // Helper: สร้างรูปทรงหกเหลี่ยม
    sf::ConvexShape createHexShape(float x, float y, TerrainType type);

    // Helper: สร้างก้อนทรัพยากร (ใช้สร้างป่า/เขา เป็นกลุ่มๆ)
    void createCluster(TerrainType type, int startR, int startC, int clusterSize);

    // Helper: อัปเดตสี (เรียกเมื่อมีการเปลี่ยนแปลง Type หรือ เปิดหมอก)
    void updateColors();

    // Helper: เปิดหมอก (Fog of War)
    void revealFog(int centerR, int centerC, int radius);

    // [ใหม่] ฟังก์ชันเริ่มเกมจริง (ถูกเรียกจาก handleMouseClick)
    void startGame(int spawnR, int spawnC);

    // [ใหม่] สุ่มทรัพยากรทั้งโลก (Sector Based)
    void generateWorldResources();

    // [ใหม่] เสกทรัพยากรการันตีรอบตัวจุดเกิด (Starter Pack)
    void spawnStarterResources(int r, int c);
};