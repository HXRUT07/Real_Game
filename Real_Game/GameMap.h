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

    bool isHovered = false;   // สถานะเมาส์ชี้
    bool isExplored = false;  // เคยสำรวจหรือยัง (หมอกดำ)

    // [สำคัญ] ต้องมีตัวนี้ครับ ไม่งั้น Error!
    bool isVisible = false;   // มองเห็นอยู่ไหม (ในระยะสายตา)
    bool isPath = false;      // เป็นช่องทางเดินที่เลือกได้หรือไม่ (สีเขียว)
};

class GameMap {
public:
    // Constructor
    GameMap(int r, int c);

    // Main interaction
    void draw(sf::RenderWindow& window);
    void updateHighlight(sf::Vector2f mousePos);
    void handleMouseClick(sf::Vector2f mousePos);

    // Getters / Checkers
    bool isGameStarted() const { return m_gameStarted; }

private:
    std::vector<HexTile> tiles;
    int rows;
    int cols;
    bool m_gameStarted = false;

    // Internal Helpers
    sf::ConvexShape createHexShape(float x, float y, TerrainType type);
    void createCluster(TerrainType type, int startR, int startC, int clusterSize);
    void updateColors();
    void startGame(int spawnR, int spawnC);
    void generateWorldResources();
    void spawnStarterResources(int r, int c);
};
//การตั้งค่าตำแหน่งเมืองผู้เล่น (Main อาจจะอยากรู้) Prame
void setPlayerCity(sf::Vector2f worldPos);