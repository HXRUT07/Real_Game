#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <memory> // เปรมทำ - สำหรับ unique_ptr
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

    // เปรมทำ - getter สำหรับดึง city แรก
    City* getFirstCity() {
        if (!cities.empty()) return cities[0].get();
        return nullptr;
    }
    // เปรมทำ - จบ

    // Getters / Checkers
    bool isGameStarted() const { return m_gameStarted; }
    bool getGridCoords(sf::Vector2f mousePos, int& outR, int& outC);
    bool isValidMove(int r, int c);

    // Movement & Fog Logic
    void calculateValidMoves(int startR, int startC, int moveRange);
    void clearHighlight();

    // ต้องอยู่ public
    void revealFog(int centerR, int centerC, int sightRange);

    // ดึง pointer tile
    HexTile* getTile(int r, int c);

    // เช็คว่าช่องนี้มีเมืองไหม
    City* getCityAt(int r, int c);

    // --- [NEW] ฟังก์ชันสำหรับดึงค่า Starter Pack ออกไปให้ main.cpp ใช้ ---
    ResourceYield getStarterPackValues() const { return m_starterPack; }

private:
    // เปรมทำ - เปลี่ยนเป็น unique_ptr เพราะ RenderTexture copy/move ไม่ได้
    std::vector<std::unique_ptr<City>> cities;
    // เปรมทำ - จบ

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