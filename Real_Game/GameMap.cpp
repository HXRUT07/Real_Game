#include "GameMap.h"
#include <cstdlib> 
#include <ctime>   
#include <algorithm> // สำหรับ std::min, std::max
#include <cmath>     // สำหรับ sqrt, cos, sin

const float PI = 3.14159265f;

GameMap::GameMap(int r, int c) {
    this->rows = r;
    this->cols = c;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // --- 1. สร้างพื้นหญ้า (Canvas) ---
    float width = sqrt(3.0f) * HEX_SIZE;
    float height = 2.0f * HEX_SIZE;
    float horizDist = width;
    float vertDist = height * 0.75f;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = col * horizDist + 50.0f;
            float y = row * vertDist + 50.0f;
            if (row % 2 != 0) x += width / 2.0f;

            HexTile tile;
            tile.shape = createHexShape(x, y, TerrainType::Grass);
            tile.gridR = row;
            tile.gridC = col;
            tile.type = TerrainType::Grass;

            // ตั้งค่าเป็น true ไว้ก่อน เพื่อแก้ปัญหาจอดำ (ถ้าอยากเทสหมอกค่อยเปลี่ยนเป็น false)
            tile.isExplored = false;

            tiles.push_back(tile);
        }
    }

    // --- 2. ระบบกระจายทรัพยากรแบบแบ่งโซน (Sector Based) ---
    int sectorSize = 5; // แบ่งโซนละ 5x5

    for (int secR = 0; secR < rows; secR += sectorSize) {
        for (int secC = 0; secC < cols; secC += sectorSize) {

            int roll = rand() % 100;
            TerrainType selectedType = TerrainType::Grass;
            int size = 0;

            if (roll < 30) {
                selectedType = TerrainType::Forest;
                size = 10 + (rand() % 10);
            }
            else if (roll < 45) {
                selectedType = TerrainType::Mountain;
                size = 8 + (rand() % 8);
            }
            else if (roll < 60) {
                selectedType = TerrainType::Water;
                size = 15 + (rand() % 15);
            }
            else {
                continue; // เป็นหญ้าปกติ ข้ามไปโซนถัดไป
            }

            // สุ่มจุดเริ่มในโซนนั้น
            int offsetX = rand() % sectorSize;
            int offsetY = rand() % sectorSize;

            int finalR = std::min(rows - 1, secR + offsetY);
            int finalC = std::min(cols - 1, secC + offsetX);

            // เรียกฟังก์ชัน createCluster (ต้องประกาศใน .h ด้วยนะ)
            createCluster(selectedType, finalR, finalC, size);
        }
    }

    updateColors();
    revealFog(0, 0, 4); // เปิดหมอกจุดเริ่มต้น
}

// *** จุดสำคัญที่แก้ Error: ต้องมี GameMap:: นำหน้า ***
void GameMap::createCluster(TerrainType type, int startR, int startC, int clusterSize) {
    std::vector<int> frontier;
    int startIndex = startR * cols + startC;

    if (startIndex >= 0 && startIndex < tiles.size()) {
        tiles[startIndex].type = type;
        frontier.push_back(startIndex);
    }

    int currentSize = 1;
    while (currentSize < clusterSize && !frontier.empty()) {
        int randIndexInFrontier = rand() % frontier.size();
        int centerIndex = frontier[randIndexInFrontier];

        int centerR = centerIndex / cols;
        int centerC = centerIndex % cols;

        int moveR = (rand() % 3) - 1;
        int moveC = (rand() % 3) - 1;

        int newR = centerR + moveR;
        int newC = centerC + moveC;

        if (newR >= 0 && newR < rows && newC >= 0 && newC < cols) {
            int newIndex = newR * cols + newC;

            // เงื่อนไข: ทับหญ้าได้เท่านั้น
            if (tiles[newIndex].type == TerrainType::Grass) {
                tiles[newIndex].type = type;
                frontier.push_back(newIndex);
                currentSize++;
            }
        }
    }
}

void GameMap::updateColors() {
    for (auto& tile : tiles) {
        if (!tile.isExplored) {
            // ยังไม่เปิดแมพ -> สีดำ
            tile.shape.setFillColor(sf::Color(10, 10, 10));
            tile.shape.setOutlineColor(sf::Color(20, 20, 20));
        }
        else {
            // เปิดแมพแล้ว -> สีจริง
            sf::Color color;
            if (tile.type == TerrainType::Grass) color = sf::Color(100, 200, 100);
            else if (tile.type == TerrainType::Water) color = sf::Color(50, 100, 200);
            else if (tile.type == TerrainType::Mountain) color = sf::Color(120, 120, 120);
            else if (tile.type == TerrainType::Forest) color = sf::Color(34, 139, 34);

            tile.shape.setFillColor(color);
            tile.shape.setOutlineColor(sf::Color(30, 30, 30));
        }
    }
}

sf::ConvexShape GameMap::createHexShape(float x, float y, TerrainType type) {
    sf::ConvexShape hex;
    hex.setPointCount(6);

    for (int i = 0; i < 6; ++i) {
        float angle = 60.0f * i - 30.0f;
        float rad = angle * (PI / 180.0f);
        float px = x + HEX_SIZE * cos(rad);
        float py = y + HEX_SIZE * sin(rad);
        hex.setPoint(i, sf::Vector2f(px, py));
    }

    // กำหนดสีเริ่มต้น (เผื่อ updateColors ยังไม่ทำงาน)
    hex.setFillColor(sf::Color::White);
    hex.setOutlineColor(sf::Color(30, 30, 30));
    hex.setOutlineThickness(2.0f);

    return hex;
}

void GameMap::draw(sf::RenderWindow& window) {
    // รอบที่ 1: วาดพื้นหลัง
    for (const auto& tile : tiles) {
        window.draw(tile.shape);
    }

    // รอบที่ 2: วาด Highlight (เฉพาะช่องที่เปิดแล้ว)
    for (const auto& tile : tiles) {
        if (tile.isHovered && tile.isExplored) {
            sf::ConvexShape highlightShape = tile.shape;
            highlightShape.setOutlineColor(sf::Color::White);
            highlightShape.setOutlineThickness(4.0f);
            highlightShape.setFillColor(sf::Color::Transparent);

            window.draw(highlightShape);
            break;
        }
    }
}

void GameMap::updateHighlight(sf::Vector2f mousePos) {
    for (auto& tile : tiles) {
        if (tile.shape.getGlobalBounds().contains(mousePos)) {
            tile.isHovered = true;
        }
        else {
            tile.isHovered = false;
        }
    }
}

void GameMap::revealFog(int centerR, int centerC, int radius) {
    bool somethingChanged = false;
    for (auto& tile : tiles) {
        int distR = std::abs(tile.gridR - centerR);
        int distC = std::abs(tile.gridC - centerC);

        if (distR <= radius && distC <= radius) {
            if (!tile.isExplored) {
                tile.isExplored = true;
                somethingChanged = true;
            }
        }
    }
    if (somethingChanged) {
        updateColors();
    }
}