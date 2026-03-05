#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include "City.h"
#include "ResourceManage.h" 
#include "Unit.h" 
const float HEX_SIZE = 30.0f;

inline sf::Color getBuildingColor(int buildingTypeIdx) {
    switch (buildingTypeIdx) {
    case 0: return sf::Color(255, 210, 50);  // Village    — ทอง
    case 1: return sf::Color(210, 60, 60);  // Barracks   — แดง
    case 2: return sf::Color(220, 130, 50);  // Restaurant — ส้ม
    case 3: return sf::Color(80, 180, 80);  // Lumbermill — เขียว
    }
    return sf::Color::White;
}

struct HexTile {
    sf::ConvexShape shape;
    int gridR = 0, gridC = 0;
    TerrainType type = TerrainType::Grass;
    bool isHovered = false;
    bool isExplored = false;
    bool isVisible = false;
    bool isPath = false;

    int gold = 0;
    int wood = 0;
    int food = 0;

    int buildingType = -1;
};

class GameMap {
public:
    GameMap(int r, int c);

    void draw(sf::RenderWindow& window);
    void drawCities(sf::RenderWindow& window);
    void updateHighlight(sf::Vector2f mousePos);
    void handleMouseClick(sf::Vector2f mousePos);

    City* getSelectedCity() { return selectedCity; }

    City* getFirstCity() {
        if (!cities.empty()) return cities[0].get();
        return nullptr;
    }

    bool isGameStarted() const { return m_gameStarted; }
    bool getGridCoords(sf::Vector2f mousePos, int& outR, int& outC);
    bool isValidMove(int r, int c);

    void calculateValidMoves(int startR, int startC, int moveRange);
    void clearHighlight();
    void revealFog(int centerR, int centerC, int sightRange);

    // --- ฟังก์ชันใหม่: กวาดสายตาทหารและเมืองทุกตัว ---
    void updateVision(const std::vector<Unit>& units, int currentPlayer);

    HexTile* getTile(int r, int c);
    City* getCityAt(int r, int c);

    ResourceYield getStarterPackValues() const { return m_starterPack; }

    void foundCity(int r, int c);

    void placeBuildingOnTile(int r, int c, int buildingTypeIdx) {
        HexTile* t = getTile(r, c);
        if (t) t->buildingType = buildingTypeIdx;
    }

private:
    std::vector<std::unique_ptr<City>> cities;
    City* selectedCity = nullptr;
    std::vector<HexTile> tiles;
    int rows;
    int cols;
    bool m_gameStarted = false;
    ResourceYield m_starterPack;

    sf::ConvexShape createHexShape(float x, float y, TerrainType type);
    void createCluster(TerrainType type, int startR, int startC, int clusterSize);
    void updateColors();
    void startGame(int spawnR, int spawnC);
    void generateWorldResources();
    void spawnStarterResources(int r, int c);
};