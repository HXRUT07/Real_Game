#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

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
    bool isVisible = false;   // (เผื่ออนาคต: เห็นแต่เดินไม่ได้ vs ไม่เห็นเลย)
};

class GameMap {
public:
    // Constructor
    GameMap(int r, int c);

    // ฟังก์ชันวาดและอัปเดตพื้นฐาน
    void draw(sf::RenderWindow& window);
    void updateHighlight(sf::Vector2f mousePos);

    // ฟังก์ชันรับการคลิกจาก Main (เพื่อเลือกจุดเกิด หรือ สั่งเดิน)
    void handleMouseClick(sf::Vector2f mousePos);

    // เช็คว่าเกมเริ่มหรือยัง
    bool isGameStarted() const { return m_gameStarted; }

    // คืนค่ารายการช่องที่ "เดินไปถึงได้" (BFS)
    std::vector<HexTile*> getReachableTiles(int startR, int startC, int movePoints);

    // เปิดหมอกในระยะสายตา
    void revealFog(int centerR, int centerC, int sightRange);

private:
    // ตัวแปรเก็บข้อมูล
    std::vector<HexTile> tiles;
    int rows;
    int cols;

    // ตัวแปรเช็คสถานะ
    bool m_gameStarted = false;

    // Helper: สร้างรูปทรงหกเหลี่ยม
    sf::ConvexShape createHexShape(float x, float y, TerrainType type);

    // Helper: สร้างก้อนทรัพยากร
    void createCluster(TerrainType type, int startR, int startC, int clusterSize);

    // Helper: อัปเดตสี (เรียกเมื่อมีการเปลี่ยนแปลง Type หรือ เปิดหมอก)
    void updateColors();

    // Helper: เริ่มเกมจริง (ถูกเรียกจาก handleMouseClick)
    void startGame(int spawnR, int spawnC);

    // Helper: สุ่มทรัพยากรทั้งโลก
    void generateWorldResources();

    // Helper: เสกทรัพยากรการันตีรอบตัวจุดเกิด
    void spawnStarterResources(int r, int c);
};