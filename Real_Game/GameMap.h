#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <memory>
#include "City.h"
#include "ResourceManage.h" 

const float HEX_SIZE = 30.0f;

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
    HexTile* getTile(int r, int c);
    City* getCityAt(int r, int c);

    ResourceYield getStarterPackValues() const { return m_starterPack; }

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