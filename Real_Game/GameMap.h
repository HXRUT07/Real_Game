#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "City.h"
#include "ResourceManage.h" 

// ขนาดของ Hexagon
const float HEX_SIZE = 30.0f;

// โครงสร้างข้อมูลของแต่ละช่อง
struct HexTile {
    sf::ConvexShape shape;
    int gridR = 0, gridC = 0;
    TerrainType type = TerrainType::Grass;

    bool isHovered = false;   // สถานะเมาส์ชี้
    bool isExplored = false;  // เคยสำรวจหรือยัง (หมอกดำ)

    // [สำคัญ] ต้องมีตัวนี้ครับ ไม่งั้น Error!
    bool isVisible = false;   // มองเห็นอยู่ไหม (ในระยะสายตา)
    bool isPath = false;

    
    int gold = 0;
    int wood = 0;
    int food = 0;
};

class GameMap {
public:
    // Constructor
    GameMap(int r, int c);

    // Main interaction
    void draw(sf::RenderWindow& window);
    void drawCities(sf::RenderWindow& window);
    void updateHighlight(sf::Vector2f mousePos);
    void handleMouseClick(sf::Vector2f mousePos);
    City* getSelectedCity() { return selectedCity; }

    // Getters / Checkers
    bool isGameStarted() const { return m_gameStarted; }
    bool getGridCoords(sf::Vector2f mousePos, int& outR, int& outC);
    bool isValidMove(int r, int c);

    // Movement & Fog Logic
    void calculateValidMoves(int startR, int startC, int moveRange);
    void clearHighlight();

    // [สำคัญ] ต้องอยู่ตรง Public นะครับ ห้ามเอาไปซ่อนใน Private
    void revealFog(int centerR, int centerC, int sightRange);

    // ---  ฟังก์ชันขอ Pointer ของ Tile เพื่อไปแก้ค่าข้างใน (เพิ่มตรงนี้) ---
    HexTile* getTile(int r, int c);

    // ---  ฟังก์ชันเช็คว่าช่องนี้มีเมืองตั้งอยู่ไหม (สำหรับคลิกขวาดูคลังหลวง) ---
    City* getCityAt(int r, int c);

private:
    std::vector<City> cities;
    City* selectedCity = nullptr;
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