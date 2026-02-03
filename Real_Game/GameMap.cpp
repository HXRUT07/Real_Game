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
    generateTerrain(TerrainType::Water, 4, 40);    // ทะเลสาบ 3 แห่ง แห่งละ 40 ช่อง
    generateTerrain(TerrainType::Mountain, 5, 30); // ภูเขา 4 ก้อน ก้อนละ 30 ช่อง
    generateTerrain(TerrainType::Forest, 7, 25);   // ป่า 6 ผืน ผืนละ 25 ช่อง

    // 4. อัปเดตสีใหม่ตามประเภทที่สุ่มได้
    // (เพราะตอนแรกเราสร้าง Shape เป็นสีเขียวหมด เราต้องมาทาสีทับใหม่)
    updateColors();

    // [เพิ่ม] เปิดแมพตรงกลางหรือจุดเริ่มเกม (เช่น แถว 0 หลัก 0)
    revealFog(0, 0, 3);
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
        if (!tile.isExplored) {
            // --- กรณี: ยังไม่เปิดแมพ (FOG) ---
            tile.shape.setFillColor(sf::Color(10, 10, 10)); // สีดำเกือบสนิท
            tile.shape.setOutlineColor(sf::Color(20, 20, 20)); // ขอบสีเทาเข้มมาก
        }
        else {
            // --- กรณี: เปิดแมพแล้ว (เหมือนเดิม) ---
            sf::Color color;
            if (tile.type == TerrainType::Grass) color = sf::Color(100, 200, 100);
            else if (tile.type == TerrainType::Water) color = sf::Color(50, 100, 200);
            else if (tile.type == TerrainType::Mountain) color = sf::Color(120, 120, 120);
            else if (tile.type == TerrainType::Forest) color = sf::Color(34, 139, 34);

            tile.shape.setFillColor(color);
            tile.shape.setOutlineColor(sf::Color(30, 30, 30)); // ขอบดำปกติ
        }
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
    hex.setOutlineThickness(-1.0f);
    hex.setOutlineColor(sf::Color::Transparent);

    return hex;
}

void GameMap::draw(sf::RenderWindow& window) {
    // รอบที่ 1: วาดพื้นหลัง (เหมือนเดิม)
    for (const auto& tile : tiles) {
        window.draw(tile.shape);
    }

    // รอบที่ 2: วาด Highlight
    for (const auto& tile : tiles) {
        if (tile.isHovered) {
            sf::ConvexShape highlight = tile.shape; // ก๊อปปี้ Shape เดิมมาเป๊ะๆ

            // --- สูตรสำเร็จให้ขอบขาวเนียน ---
            highlight.setFillColor(sf::Color::Transparent); // ทำให้ข้างในใส จะได้ไม่ทับสี Terrain
            highlight.setOutlineColor(sf::Color::White);     // ตั้งขอบเป็นสีขาว

            // ใช้ค่าลบที่ "หนาขึ้น" (เช่น -4.0f หรือ -5.0f)
            // ยิ่งค่าลบมาก เส้นขาวจะยิ่งขยับเข้าหาจุดศูนย์กลางช่อง 
            // ทำให้เหลือพื้นที่ขอบนอกสุดให้ "เส้นสีดำ" เดิมโชว์ออกมาได้ครับ
            highlight.setOutlineThickness(-4.0f);

            window.draw(highlight); // วาดทับเป็น Layer สุดท้าย
            break;
        }
    }
}

void GameMap::updateHighlight(sf::Vector2f mousePos) {
    for (auto& tile : tiles) {
        // ตรวจสอบว่าเมาส์ชี้อยู่ในขอบเขตของหกเหลี่ยมช่องนั้นหรือไม่
        // (Logic เดิมมีการเช็ค 2 รอบ ผมยุบให้เหลือรอบเดียวเพื่อความกระชับครับ)
        if (tile.shape.getGlobalBounds().contains(mousePos)) {
            tile.isHovered = true;
        }
        else {
            tile.isHovered = false; // <--- สำคัญมาก: ต้องปิดสถานะช่องอื่นด้วย
        }
    }
}

void GameMap::revealFog(int centerR, int centerC, int radius) {
    bool somethingChanged = false;

    for (auto& tile : tiles) {
        // คำนวณระยะห่างแบบง่าย (Box Distance)
        int distR = std::abs(tile.gridR - centerR);
        int distC = std::abs(tile.gridC - centerC);

        // ถ้าอยู่ในระยะที่กำหนด และยังไม่เคยเปิดมาก่อน
        if (distR <= radius && distC <= radius) {
            if (!tile.isExplored) {
                tile.isExplored = true;
                somethingChanged = true;
            }
        }
    }

    // สั่งวาดสีใหม่เฉพาะเมื่อมีการเปลี่ยนแปลง (เพื่อประหยัดทรัพยากร)
    if (somethingChanged) {
        updateColors();
    }
}