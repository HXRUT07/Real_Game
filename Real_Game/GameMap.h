#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

const float HEX_SIZE = 30.0f;

enum class TerrainType {
    Grass,
    Water,
    Mountain,
    Forest // <--- เพิ่มป่าเข้ามา
};

struct HexTile {
    sf::ConvexShape shape;
    int gridR, gridC = 0;
    TerrainType type = TerrainType::Grass;
    bool isHovered = false; // <--- เพิ่มไว้เช็คสถานะเมาส์ชี้

    // <--- [FOG] เพิ่มตัวแปรเช็คว่าเปิดแมพหรือยัง (เริ่มต้นเป็น false คือมืด)
    bool isExplored = false;
};

class GameMap {
private:
    std::vector<HexTile> tiles;
    int rows;
    int cols;

    sf::ConvexShape createHexShape(float x, float y, TerrainType type);

    // <--- เพิ่มฟังก์ชันส่วนตัว (Private Helper)
    void generateTerrain(TerrainType type, int numClusters, int clusterSize);
    void updateColors();

public:
    GameMap(int r, int c);
    void draw(sf::RenderWindow& window);

    // Helper function สำหรับเพื่อนบ้าน (ถ้าจะทำเดี๋ยวมาเพิ่มทีหลัง)

    // <--- ฟังก์ชันหัวใจหลักสำหรับระบบ Highlight
    void updateHighlight(sf::Vector2f mousePos);

    // <--- [FOG] เพิ่มฟังก์ชันสำหรับเปิดหมอก
    // รับพิกัด (r, c) และรัศมี (radius) ที่จะเปิด
    void revealFog(int r, int c, int radius);

};