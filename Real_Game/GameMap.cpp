#include "GameMap.h"
#include <cstdlib> // สำหรับ rand()
#include <ctime>   // สำหรับ time()

const float PI = 3.14159265f;

GameMap::GameMap(int r, int c) {
    this->rows = r;
    this->cols = c;

    // 1. เริ่มต้นการสุ่ม (Seeding)
    // ถ้าไม่ใส่บรรทัดนี้ แมพจะหน้าตาเหมือนเดิมทุกครั้งที่เปิดเกม
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 2. สร้างโครงสร้างพื้นฐาน (ถมหญ้าให้เต็มก่อน)
    float width = sqrt(3.0f) * HEX_SIZE;
    float height = 2.0f * HEX_SIZE;
    float horizDist = width;
    float vertDist = height * 0.75f;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = col * horizDist + 50.0f;
            float y = row * vertDist + 50.0f;
            if (row % 2 != 0) x += width / 2.0f;

            // สร้าง Shape
            // ตอนแรกให้ทุกช่องเป็น Grass ไปก่อน (Canvas ว่างเปล่า)
            HexTile tile;
            tile.shape = createHexShape(x, y, TerrainType::Grass);
            tile.gridR = row;
            tile.gridC = col;
            tile.type = TerrainType::Grass; // Default

            tiles.push_back(tile);
        }
    }

    // 3. เริ่มกระบวนการสร้างโลก (World Generation)
    generateTerrain(TerrainType::Water, 3, 40);    // ทะเลสาบ 3 แห่ง แห่งละ 40 ช่อง
    generateTerrain(TerrainType::Mountain, 4, 30); // ภูเขา 4 ก้อน ก้อนละ 30 ช่อง
    generateTerrain(TerrainType::Forest, 6, 25);   // ป่า 6 ผืน ผืนละ 25 ช่อง

    // 4. อัปเดตสีใหม่ตามประเภทที่สุ่มได้
    // (เพราะตอนแรกเราสร้าง Shape เป็นสีเขียวหมด เราต้องมาทาสีทับใหม่)
    updateColors();
}

// ฟังก์ชัน "เดินสุ่ม" (Random Walk) เพื่อสร้างกลุ่มก้อนที่ต่อเนื่องกัน
void GameMap::generateTerrain(TerrainType type, int numClusters, int clusterSize) {
    for (int i = 0; i < numClusters; ++i) {
        // 1. สร้าง "รายการช่องที่จะขยายต่อ" (Frontier)
        std::vector<int> frontier;

        // 2. สุ่มจุดเริ่มต้น 1 จุด
        int r = rand() % rows;
        int c = rand() % cols;
        int startIndex = r * cols + c;

        // เปลี่ยนสีจุดแรก และยัดใส่รายการ
        if (startIndex >= 0 && startIndex < tiles.size()) {
            tiles[startIndex].type = type;
            frontier.push_back(startIndex);
        }

        // 3. วนลูปเพื่อ "ลาม" ออกไปเรื่อยๆ จนครบขนาดที่ต้องการ
        int currentSize = 1;
        while (currentSize < clusterSize && !frontier.empty()) {

            // A. สุ่มหยิบช่องนึงจากรายการขยาย (เพื่อให้มันลามมั่วๆ ไม่เป็นเส้นตรง)
            int randIndexInFrontier = rand() % frontier.size();
            int centerIndex = frontier[randIndexInFrontier];

            // แปลง Index กลับเป็น r, c
            int centerR = centerIndex / cols;
            int centerC = centerIndex % cols;

            // B. สุ่มหาเพื่อนบ้าน 1 ช่อง (บน/ล่าง/ซ้าย/ขวา)
            int moveR = (rand() % 3) - 1; // -1, 0, 1
            int moveC = (rand() % 3) - 1; // -1, 0, 1

            int newR = centerR + moveR;
            int newC = centerC + moveC;

            // ตรวจสอบว่าไม่หลุดแมพ
            if (newR >= 0 && newR < rows && newC >= 0 && newC < cols) {
                int newIndex = newR * cols + newC;

                // C. ถ้าช่องนี้ยังไม่เป็น Type นั้น -> เปลี่ยนสีมันซะ!
                if (tiles[newIndex].type != type) {
                    tiles[newIndex].type = type;
                    frontier.push_back(newIndex); // เพิ่มช่องใหม่เข้าไปในรายการขยาย
                    currentSize++;
                }
            }
        }
    }
}

void GameMap::updateColors() {
    for (auto& tile : tiles) {
        // เรียกใช้ฟังก์ชันเดิม แต่ส่ง Type ใหม่เข้าไปเพื่อเอาสี
        // วิธีนี้อาจจะไม่ Efficient มาก แต่เขียนง่ายสุดสำหรับตอนนี้
        sf::Color color;
        if (tile.type == TerrainType::Grass) color = sf::Color(100, 200, 100);
        else if (tile.type == TerrainType::Water) color = sf::Color(50, 100, 200);
        else if (tile.type == TerrainType::Mountain) color = sf::Color(120, 120, 120);
        else if (tile.type == TerrainType::Forest) color = sf::Color(34, 139, 34); // สีเขียวเข้มป่าไม้

        tile.shape.setFillColor(color);
    }
}

sf::ConvexShape GameMap::createHexShape(float x, float y, TerrainType type) {
    sf::ConvexShape hex;
    hex.setPointCount(6);

    // วาดจุด 6 มุม
    for (int i = 0; i < 6; ++i) {
        float angle = 60.0f * i - 30.0f;
        float rad = angle * (PI / 180.0f);
        float px = x + HEX_SIZE * cos(rad);
        float py = y + HEX_SIZE * sin(rad);
        hex.setPoint(i, sf::Vector2f(px, py));
    }

    // กำหนดสีตามประเภทพื้นที่
    if (type == TerrainType::Grass) hex.setFillColor(sf::Color(100, 200, 100)); // เขียว
    else if (type == TerrainType::Water) hex.setFillColor(sf::Color(50, 100, 200)); // ฟ้า
    else if (type == TerrainType::Mountain) hex.setFillColor(sf::Color(120, 120, 120)); // เทา

    hex.setOutlineColor(sf::Color(30, 30, 30)); // ขอบดำจางๆ
    hex.setOutlineThickness(2.0f);

    return hex;
}

void GameMap::draw(sf::RenderWindow& window) {
    for (const auto& tile : tiles) {
        window.draw(tile.shape);
    }
}