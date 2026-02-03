#include "GameMap.h"
#include <cstdlib> 
#include <ctime>   
#include <algorithm> 
#include <cmath>     

const float PI = 3.14159265f;

// --------------------------------------------------------
// 1. Constructor: สร้างกระดานเปล่าๆ รอรับคำสั่ง
// --------------------------------------------------------
GameMap::GameMap(int r, int c) {
    this->rows = r;
    this->cols = c;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

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

            // [TRICK] ให้เป็น True ไว้ก่อน เพื่อให้ผู้เล่นเห็นกระดานเปล่าๆ ตอนเลือกจุด
            tile.isExplored = false;

            tiles.push_back(tile);
        }
    }

    // อัปเดตสีครั้งแรก (จะเป็นสีเขียวล้วน)
    updateColors();
}

// --------------------------------------------------------
// 2. Main Interaction: เมื่อผู้เล่นคลิกเมาส์
// --------------------------------------------------------
void GameMap::handleMouseClick(sf::Vector2f mousePos) {

    // 1. กรณีเกมเริ่มแล้ว (Game Started) -> ให้ถือว่าคลิกคือ "การเดิน"
    if (m_gameStarted) {
        for (auto& tile : tiles) {
            if (tile.shape.getGlobalBounds().contains(mousePos)) {
                // สมมติว่าเดินมาช่องนี้ -> เปิดหมอกรอบตัว 1 ช่อง
                revealFog(tile.gridR, tile.gridC, 1);
                break;
            }
        }
        return; // จบการทำงาน (ไม่ไปทำ logic เลือกจุดเกิดด้านล่าง)
    }

    // 2. กรณีเกมยังไม่เริ่ม (Selection Phase) -> เลือกจุดเกิด
    for (auto& tile : tiles) {
        if (tile.shape.getGlobalBounds().contains(mousePos)) {
            startGame(tile.gridR, tile.gridC);
            break;
        }
    }
}

// --------------------------------------------------------
// 3. Game Start Sequence: เริ่มกระบวนการสร้างโลก
// --------------------------------------------------------
void GameMap::startGame(int spawnR, int spawnC) {
    if (m_gameStarted) return;
    m_gameStarted = true;

    // A. สุ่มทรัพยากรทั้งโลก (Background Generation)
    generateWorldResources();

    // B. เสกทรัพยากรการันตีใกล้ตัว (Starter Pack)
    spawnStarterResources(spawnR, spawnC);

    // C. บังคับจุดเกิดให้เป็นหญ้าเสมอ (กันเกิดทับภูเขา)
    int spawnIndex = spawnR * cols + spawnC;
    if (spawnIndex >= 0 && spawnIndex < tiles.size()) {
        tiles[spawnIndex].type = TerrainType::Grass;
    }

    // D. [สำคัญ] รีเซ็ตหมอกให้มืดทั้งโลกก่อน (Fog Reset)
    // เพราะตอนแรกเราเปิดไว้ให้เห็นกระดานเปล่า ตอนนี้ต้องปิดเพื่อเริ่มเล่นจริง
    for (auto& tile : tiles) {
        tile.isExplored = false;
    }

    // E. เปิดหมอกเฉพาะจุดเกิด
    revealFog(spawnR, spawnC, 1);

    // F. อัปเดตสีทั้งหมดใหม่ (วาดป่า/เขา และวาดสีดำทับส่วนที่ยังไม่เปิด)
    updateColors();
}

// --------------------------------------------------------
// 4. Resource Generation Logic
// --------------------------------------------------------
void GameMap::generateWorldResources() {
    int sectorSize = 5;
    for (int secR = 0; secR < rows; secR += sectorSize) {
        for (int secC = 0; secC < cols; secC += sectorSize) {

            int roll = rand() % 100;
            TerrainType selectedType = TerrainType::Grass;
            int size = 0;

            // ปรับเปอร์เซ็นต์ตรงนี้
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
                continue;
            }

            int offsetX = rand() % sectorSize;
            int offsetY = rand() % sectorSize;
            int finalR = std::min(rows - 1, secR + offsetY);
            int finalC = std::min(cols - 1, secC + offsetX);

            createCluster(selectedType, finalR, finalC, size);
        }
    }
}

void GameMap::spawnStarterResources(int r, int c) {
    // เสกป่าใกล้ๆ
    int forestR = std::min(rows - 1, std::max(0, r + (rand() % 3 - 1)));
    int forestC = std::min(cols - 1, std::max(0, c + 2));
    createCluster(TerrainType::Forest, forestR, forestC, 5);

    // เสกภูเขาใกล้ๆ
    int mountR = std::min(rows - 1, std::max(0, r + 2));
    int mountC = std::min(cols - 1, std::max(0, c - 1));
    createCluster(TerrainType::Mountain, mountR, mountC, 4);
}

void GameMap::createCluster(TerrainType type, int startR, int startC, int clusterSize) {
    std::vector<int> frontier;
    int startIndex = startR * cols + startC;

    if (startIndex >= 0 && startIndex < tiles.size()) {
        tiles[startIndex].type = type;
        frontier.push_back(startIndex);
    }

    int currentSize = 1;
    while (currentSize < clusterSize && !frontier.empty()) {
        int randIndex = rand() % frontier.size();
        int centerIndex = frontier[randIndex];
        int centerR = centerIndex / cols;
        int centerC = centerIndex % cols;

        // สุ่มทิศทาง
        int moveR = (rand() % 3) - 1;
        int moveC = (rand() % 3) - 1;
        int newR = centerR + moveR;
        int newC = centerC + moveC;

        if (newR >= 0 && newR < rows && newC >= 0 && newC < cols) {
            int newIndex = newR * cols + newC;
            // ทับได้เฉพาะหญ้า
            if (tiles[newIndex].type == TerrainType::Grass) {
                tiles[newIndex].type = type;
                frontier.push_back(newIndex);
                currentSize++;
            }
        }
    }
}

// --------------------------------------------------------
// 5. Visual & Render Logic
// --------------------------------------------------------
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

void GameMap::revealFog(int centerR, int centerC, int radius) {
    bool somethingChanged = false;

    for (auto& tile : tiles) {
        // แปลงเป็น Axial Coordinates (q, r)
        int r1 = centerR;
        int c1 = centerC - (centerR - (centerR & 1)) / 2;

        int r2 = tile.gridR;
        int c2 = tile.gridC - (tile.gridR - (tile.gridR & 1)) / 2;

        // คำนวณความต่าง
        int dr = r1 - r2;
        int dc = c1 - c2;

        // สูตรระยะห่าง: (|dr| + |dc| + |dr + dc|) / 2
        int distance = (std::abs(dr) + std::abs(dc) + std::abs(dr + dc)) / 2;

        // -----------------------------------------------------------

        // ถ้าอยู่ในระยะวงกลม (Hex Circle)
        if (distance <= radius) {
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

sf::ConvexShape GameMap::createHexShape(float x, float y, TerrainType type) {
    sf::ConvexShape hex;
    hex.setPointCount(6);
    for (int i = 0; i < 6; ++i) {
        float angle = 60.0f * i - 30.0f;
        float rad = angle * (PI / 180.0f);
        hex.setPoint(i, sf::Vector2f(x + HEX_SIZE * cos(rad), y + HEX_SIZE * sin(rad)));
    }
    hex.setFillColor(sf::Color::White);
    hex.setOutlineThickness(2.0f);
    return hex;
}

void GameMap::draw(sf::RenderWindow& window) {
    for (const auto& tile : tiles) {
        window.draw(tile.shape);
    }
    for (const auto& tile : tiles) {
        // ต้องเมาส์ชี้ และ ต้องเปิดแมพแล้ว ถึงจะขึ้นกรอบ
        // หรือ ถ้ายังไม่เริ่มเกม (เลือกจุดเกิด) ก็ให้ขึ้นกรอบได้เลย
        bool showHighlight = tile.isHovered && (tile.isExplored || !m_gameStarted);

        if (showHighlight) {
            sf::ConvexShape h = tile.shape;
            h.setOutlineColor(sf::Color::White);
            h.setOutlineThickness(4.0f);
            h.setFillColor(sf::Color::Transparent);
            window.draw(h);
            break;
        }
    }
}

void GameMap::updateHighlight(sf::Vector2f mousePos) {
    for (auto& tile : tiles) {
        if (tile.shape.getGlobalBounds().contains(mousePos)) tile.isHovered = true;
        else tile.isHovered = false;
    }
}