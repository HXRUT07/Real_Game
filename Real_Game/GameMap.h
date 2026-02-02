#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

const float HEX_SIZE = 30.0f;

enum class TerrainType {
    Grass,
    Water,
    Mountain,
    Forest // <--- à¾ÔèÁ»èÒà¢éÒÁÒ
};

struct HexTile {
    sf::ConvexShape shape;
    int gridR, gridC = 0;
    TerrainType type = TerrainType::Grass;
    bool isHovered = false; // <--- เพิ่มไว้เช็คสถานะเมาส์ชี้
};

class GameMap {
private:
    std::vector<HexTile> tiles;
    int rows;
    int cols;

    sf::ConvexShape createHexShape(float x, float y, TerrainType type);

    // <--- à¾ÔèÁ¿Ñ§¡ìªÑ¹ÊèÇ¹µÑÇ (Private Helper)
    void generateTerrain(TerrainType type, int numClusters, int clusterSize);
    void updateColors();

public:
    GameMap(int r, int c);
    void draw(sf::RenderWindow& window);

    // Helper function ÊÓËÃÑºà¾×èÍ¹ºéÒ¹ (¶éÒ¨Ð·Óà´ÕëÂÇÁÒà¾ÔèÁ·ÕËÅÑ§)

    // <--- ฟังก์ชันหัวใจหลักสำหรับระบบ Highlight
    void updateHighlight(sf::Vector2f mousePos);

};