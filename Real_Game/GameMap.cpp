#include "GameMap.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue> // <--- [สำคัญมาก] ต้องมีบรรทัดนี้ ไม่งั้น Error std::queue

const float PI = 3.14159265f;

// [สำคัญ] ต้องประกาศ struct Node ไว้ตรงนี้ ก่อนที่จะถูกเรียกใช้
struct Node {
    int r, c;
    int cost;
};

// --------------------------------------------------------
// Constructor
// --------------------------------------------------------
GameMap::GameMap(int r, int c) {
    this->rows = r;
    this->cols = c;
    this->m_gameStarted = false;

    float width = std::sqrt(3.0f) * HEX_SIZE;
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

            // ตอนเริ่มเกม เปิดให้เห็นกระดานเปล่าก่อน
            tile.isExplored = true;
            tile.isVisible = true;

            tiles.push_back(tile);
        }
    }
    updateColors();
}

// --------------------------------------------------------
// Interaction & Helpers
// --------------------------------------------------------
bool GameMap::getGridCoords(sf::Vector2f mousePos, int& outR, int& outC) {
    for (const auto& tile : tiles) {
        if (tile.shape.getGlobalBounds().contains(mousePos)) {
            outR = tile.gridR;
            outC = tile.gridC;
            return true;
        }
    }
    return false;
}

void GameMap::handleMouseClick(sf::Vector2f mousePos) {
    if (!m_gameStarted) {
        int r, c;
        if (getGridCoords(mousePos, r, c)) {
            startGame(r, c);
        }
    }
}

// --------------------------------------------------------
// Game Logic (Start / Gen World)
// --------------------------------------------------------
void GameMap::startGame(int spawnR, int spawnC) {
    if (m_gameStarted) return;
    m_gameStarted = true;

    generateWorldResources();
    spawnStarterResources(spawnR, spawnC);

    // เคลียร์จุดเกิดให้เป็นหญ้า
    int idx = spawnR * cols + spawnC;
    if (idx >= 0 && idx < tiles.size()) tiles[idx].type = TerrainType::Grass;

    // รีเซ็ตหมอกให้มืดทั้งโลก
    for (auto& tile : tiles) {
        tile.isExplored = false;
        tile.isVisible = false;
    }

    // เปิดหมอกจุดเกิด
    revealFog(spawnR, spawnC, 1);
    updateColors();
}

void GameMap::generateWorldResources() {
    int sectorSize = 5;
    for (int sr = 0; sr < rows; sr += sectorSize) {
        for (int sc = 0; sc < cols; sc += sectorSize) {
            int roll = std::rand() % 100;
            TerrainType type = TerrainType::Grass;
            int size = 0;

            if (roll < 30) { type = TerrainType::Forest; size = 10; }
            else if (roll < 45) { type = TerrainType::Mountain; size = 8; }
            else if (roll < 60) { type = TerrainType::Water; size = 15; }
            else continue;

            int rr = std::min(rows - 1, sr + std::rand() % sectorSize);
            int cc = std::min(cols - 1, sc + std::rand() % sectorSize);
            createCluster(type, rr, cc, size);
        }
    }
}

void GameMap::spawnStarterResources(int r, int c) {
    int fr = std::min(rows - 1, std::max(0, r + 2));
    int fc = std::min(cols - 1, std::max(0, c + 2));
    createCluster(TerrainType::Forest, fr, fc, 5);

    int mr = std::min(rows - 1, std::max(0, r - 2));
    int mc = std::min(cols - 1, std::max(0, c - 2));
    createCluster(TerrainType::Mountain, mr, mc, 4);
}

void GameMap::createCluster(TerrainType type, int startR, int startC, int clusterSize) {
    std::vector<int> frontier;
    int startIdx = startR * cols + startC;
    if (startIdx >= 0 && startIdx < tiles.size()) {
        tiles[startIdx].type = type;
        frontier.push_back(startIdx);
    }

    int currentSize = 1;
    while (currentSize < clusterSize && !frontier.empty()) {
        int randIdx = std::rand() % frontier.size();
        int centerIdx = frontier[randIdx];
        int cr = centerIdx / cols;
        int cc = centerIdx % cols;

        int moveR = (std::rand() % 3) - 1;
        int moveC = (std::rand() % 3) - 1;
        int nr = cr + moveR;
        int nc = cc + moveC;

        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
            int nIdx = nr * cols + nc;
            if (tiles[nIdx].type == TerrainType::Grass) {
                tiles[nIdx].type = type;
                frontier.push_back(nIdx);
                currentSize++;
            }
        }
    }
}

// --------------------------------------------------------
// Pathfinding & Visibility
// --------------------------------------------------------
void GameMap::revealFog(int centerR, int centerC, int sightRange) {
    bool changed = false;
    for (auto& tile : tiles) {
        // สูตรระยะห่างวงกลม (Axial Distance)
        int r1 = centerR;
        int c1 = centerC - (centerR - (centerR & 1)) / 2;
        int r2 = tile.gridR;
        int c2 = tile.gridC - (tile.gridR - (tile.gridR & 1)) / 2;
        int dist = (std::abs(r1 - r2) + std::abs(c1 - c2) + std::abs((r1 - r2) + (c1 - c2))) / 2;

        if (dist <= sightRange) {
            // เปิดหมอก (Explored) และทำให้มองเห็น (Visible)
            if (!tile.isExplored || !tile.isVisible) {
                tile.isExplored = true;
                tile.isVisible = true;
                changed = true;
            }
        }
        else {
            // นอกระยะสายตา -> ปิดการมองเห็น (แต่ยัง Explored อยู่ถ้าเคยเดินผ่าน)
            if (tile.isVisible) {
                tile.isVisible = false;
                changed = true;
            }
        }
    }
    if (changed) updateColors();
}

void GameMap::calculateValidMoves(int startR, int startC, int moveRange) {
    clearHighlight();

    std::vector<bool> visited(rows * cols, false);
    std::queue<Node> q;
    q.push({ startR, startC, 0 });
    visited[startR * cols + startC] = true;

    int evenDir[6][2] = { {-1,-1}, {-1,0}, {0,-1}, {0,1}, {1,-1}, {1,0} };
    int oddDir[6][2] = { {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,0}, {1,1} };

    while (!q.empty()) {
        Node curr = q.front();
        q.pop();

        if (curr.cost > 0) {
            int idx = curr.r * cols + curr.c;
            tiles[idx].isPath = true; // Highlight
        }

        if (curr.cost >= moveRange) continue;

        for (int i = 0; i < 6; ++i) {
            int nr, nc;
            if (curr.r % 2 == 0) {
                nr = curr.r + evenDir[i][0];
                nc = curr.c + evenDir[i][1];
            }
            else {
                nr = curr.r + oddDir[i][0];
                nc = curr.c + oddDir[i][1];
            }

            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                int nIdx = nr * cols + nc;

                // กฎการเดิน: ไม่เคยไป + ไม่ใช่เขา/น้ำ + ต้องมองเห็นอยู่ (isVisible)
                if (!visited[nIdx] &&
                    tiles[nIdx].type != TerrainType::Mountain &&
                    tiles[nIdx].type != TerrainType::Water &&
                    tiles[nIdx].isVisible) { // <--- เช็คตรงนี้

                    visited[nIdx] = true;
                    q.push({ nr, nc, curr.cost + 1 });
                }
            }
        }
    }
}

void GameMap::clearHighlight() {
    for (auto& tile : tiles) tile.isPath = false;
}

bool GameMap::isValidMove(int r, int c) {
    int idx = r * cols + c;
    if (idx >= 0 && idx < tiles.size()) {
        return tiles[idx].isPath;
    }
    return false;
}

// --------------------------------------------------------
// Graphics & Visuals
// --------------------------------------------------------
void GameMap::updateColors() {
    for (auto& tile : tiles) {
        if (!tile.isExplored) {
            // ยังไม่เคยมา -> ดำมืด
            tile.shape.setFillColor(sf::Color(10, 10, 10));
            tile.shape.setOutlineColor(sf::Color(20, 20, 20));
        }
        else if (!tile.isVisible) {
            // เคยมาแต่ตอนนี้มองไม่เห็น -> สีเทาๆ (Fog of War)
            sf::Color c;
            switch (tile.type) {
            case TerrainType::Grass: c = sf::Color(50, 100, 50); break;
            case TerrainType::Water: c = sf::Color(25, 50, 100); break;
            case TerrainType::Mountain: c = sf::Color(60, 60, 60); break;
            case TerrainType::Forest: c = sf::Color(17, 70, 17); break;
            }
            tile.shape.setFillColor(c);
            tile.shape.setOutlineColor(sf::Color(30, 30, 30));
        }
        else {
            // มองเห็นชัดเจน -> สีสดใส
            sf::Color c;
            switch (tile.type) {
            case TerrainType::Grass: c = sf::Color(100, 200, 100); break;
            case TerrainType::Water: c = sf::Color(50, 100, 200); break;
            case TerrainType::Mountain: c = sf::Color(120, 120, 120); break;
            case TerrainType::Forest: c = sf::Color(34, 139, 34); break;
            }
            tile.shape.setFillColor(c);
            tile.shape.setOutlineColor(sf::Color(30, 30, 30));
        }
    }
}

void GameMap::updateHighlight(sf::Vector2f mousePos) {
    for (auto& tile : tiles) {
        tile.isHovered = tile.shape.getGlobalBounds().contains(mousePos);
    }
}

void GameMap::draw(sf::RenderWindow& window) {
    for (const auto& tile : tiles) {
        window.draw(tile.shape);
    }

    // วาดช่องทางเดิน (Highlight สีเขียว)
    for (const auto& tile : tiles) {
        if (tile.isPath) {
            sf::ConvexShape h = tile.shape;
            h.setFillColor(sf::Color(0, 255, 0, 100));
            h.setOutlineColor(sf::Color::Green);
            h.setOutlineThickness(2.0f);
            window.draw(h);
        }
    }

    // วาดกรอบเมาส์ชี้ (Mouse Hover)
    for (const auto& tile : tiles) {
        if (tile.isHovered && tile.isExplored) {
            sf::ConvexShape h = tile.shape;
            h.setFillColor(sf::Color::Transparent);
            h.setOutlineColor(sf::Color::White);
            h.setOutlineThickness(3.0f);
            window.draw(h);
            break;
        }
    }
}

sf::ConvexShape GameMap::createHexShape(float x, float y, TerrainType type) {
    sf::ConvexShape hex;
    hex.setPointCount(6);
    for (int i = 0; i < 6; ++i) {
        float angle = 60.0f * i - 30.0f;
        float rad = angle * (PI / 180.0f);
        hex.setPoint(i, sf::Vector2f(x + HEX_SIZE * std::cos(rad), y + HEX_SIZE * std::sin(rad)));
    }
    hex.setOutlineThickness(-1.0f);
    return hex;
}