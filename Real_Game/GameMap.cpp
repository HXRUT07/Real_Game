#include "GameMap.h"
#include <cstdlib> // ÊÓËÃÑº rand()
#include <ctime>   // ÊÓËÃÑº time()

const float PI = 3.14159265f;

GameMap::GameMap(int r, int c) {
    this->rows = r;
    this->cols = c;

    // 1. àÃÔèÁµé¹¡ÒÃÊØèÁ (Seeding)
    // ¶éÒäÁèãÊèºÃÃ·Ñ´¹Õé áÁ¾¨ÐË¹éÒµÒàËÁ×Í¹à´ÔÁ·Ø¡¤ÃÑé§·Õèà»Ô´à¡Á
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 2. ÊÃéÒ§â¤Ã§ÊÃéÒ§¾×é¹°Ò¹ (¶ÁË­éÒãËéàµçÁ¡èÍ¹)
    float width = sqrt(3.0f) * HEX_SIZE;
    float height = 2.0f * HEX_SIZE;
    float horizDist = width;
    float vertDist = height * 0.75f;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = col * horizDist + 50.0f;
            float y = row * vertDist + 50.0f;
            if (row % 2 != 0) x += width / 2.0f;

            // ÊÃéÒ§ Shape
            // µÍ¹áÃ¡ãËé·Ø¡ªèÍ§à»ç¹ Grass ä»¡èÍ¹ (Canvas ÇèÒ§à»ÅèÒ)
            HexTile tile;
            tile.shape = createHexShape(x, y, TerrainType::Grass);
            tile.gridR = row;
            tile.gridC = col;
            tile.type = TerrainType::Grass; // Default

            tiles.push_back(tile);
        }
    }

    // 3. àÃÔèÁ¡ÃÐºÇ¹¡ÒÃÊÃéÒ§âÅ¡ (World Generation)
    generateTerrain(TerrainType::Water, 3, 40);    // ·ÐàÅÊÒº 3 áËè§ áËè§ÅÐ 40 ªèÍ§
    generateTerrain(TerrainType::Mountain, 4, 30); // ÀÙà¢Ò 4 ¡éÍ¹ ¡éÍ¹ÅÐ 30 ªèÍ§
    generateTerrain(TerrainType::Forest, 6, 25);   // »èÒ 6 ¼×¹ ¼×¹ÅÐ 25 ªèÍ§

    // 4. ÍÑ»à´µÊÕãËÁèµÒÁ»ÃÐàÀ··ÕèÊØèÁä´é
    // (à¾ÃÒÐµÍ¹áÃ¡àÃÒÊÃéÒ§ Shape à»ç¹ÊÕà¢ÕÂÇËÁ´ àÃÒµéÍ§ÁÒ·ÒÊÕ·ÑºãËÁè)
    updateColors();
}

// ¿Ñ§¡ìªÑ¹ "à´Ô¹ÊØèÁ" (Random Walk) à¾×èÍÊÃéÒ§¡ÅØèÁ¡éÍ¹·ÕèµèÍà¹×èÍ§¡Ñ¹
void GameMap::generateTerrain(TerrainType type, int numClusters, int clusterSize) {
    for (int i = 0; i < numClusters; ++i) {
        // 1. ÊÃéÒ§ "ÃÒÂ¡ÒÃªèÍ§·Õè¨Ð¢ÂÒÂµèÍ" (Frontier)
        std::vector<int> frontier;

        // 2. ÊØèÁ¨Ø´àÃÔèÁµé¹ 1 ¨Ø´
        int r = rand() % rows;
        int c = rand() % cols;
        int startIndex = r * cols + c;

        // à»ÅÕèÂ¹ÊÕ¨Ø´áÃ¡ áÅÐÂÑ´ãÊèÃÒÂ¡ÒÃ
        if (startIndex >= 0 && startIndex < tiles.size()) {
            tiles[startIndex].type = type;
            frontier.push_back(startIndex);
        }

        // 3. Ç¹ÅÙ»à¾×èÍ "ÅÒÁ" ÍÍ¡ä»àÃ×èÍÂæ ¨¹¤Ãº¢¹Ò´·ÕèµéÍ§¡ÒÃ
        int currentSize = 1;
        while (currentSize < clusterSize && !frontier.empty()) {

            // A. ÊØèÁËÂÔºªèÍ§¹Ö§¨Ò¡ÃÒÂ¡ÒÃ¢ÂÒÂ (à¾×èÍãËéÁÑ¹ÅÒÁÁÑèÇæ äÁèà»ç¹àÊé¹µÃ§)
            int randIndexInFrontier = rand() % frontier.size();
            int centerIndex = frontier[randIndexInFrontier];

            // á»Å§ Index ¡ÅÑºà»ç¹ r, c
            int centerR = centerIndex / cols;
            int centerC = centerIndex % cols;

            // B. ÊØèÁËÒà¾×èÍ¹ºéÒ¹ 1 ªèÍ§ (º¹/ÅèÒ§/«éÒÂ/¢ÇÒ)
            int moveR = (rand() % 3) - 1; // -1, 0, 1
            int moveC = (rand() % 3) - 1; // -1, 0, 1

            int newR = centerR + moveR;
            int newC = centerC + moveC;

            // µÃÇ¨ÊÍºÇèÒäÁèËÅØ´áÁ¾
            if (newR >= 0 && newR < rows && newC >= 0 && newC < cols) {
                int newIndex = newR * cols + newC;

                // C. ¶éÒªèÍ§¹ÕéÂÑ§äÁèà»ç¹ Type ¹Ñé¹ -> à»ÅÕèÂ¹ÊÕÁÑ¹«Ð!
                if (tiles[newIndex].type != type) {
                    tiles[newIndex].type = type;
                    frontier.push_back(newIndex); // à¾ÔèÁªèÍ§ãËÁèà¢éÒä»ã¹ÃÒÂ¡ÒÃ¢ÂÒÂ
                    currentSize++;
                }
            }
        }
    }
}

void GameMap::updateColors() {
    for (auto& tile : tiles) {
        // àÃÕÂ¡ãªé¿Ñ§¡ìªÑ¹à´ÔÁ áµèÊè§ Type ãËÁèà¢éÒä»à¾×èÍàÍÒÊÕ
        // ÇÔ¸Õ¹ÕéÍÒ¨¨ÐäÁè Efficient ÁÒ¡ áµèà¢ÕÂ¹§èÒÂÊØ´ÊÓËÃÑºµÍ¹¹Õé

        sf::Color color;
        if (tile.type == TerrainType::Grass) color = sf::Color(100, 200, 100);
        else if (tile.type == TerrainType::Water) color = sf::Color(50, 100, 200);
        else if (tile.type == TerrainType::Mountain) color = sf::Color(120, 120, 120);
        else if (tile.type == TerrainType::Forest) color = sf::Color(34, 139, 34); // ÊÕà¢ÕÂÇà¢éÁ»èÒäÁé

        tile.shape.setFillColor(color);
    }
}

sf::ConvexShape GameMap::createHexShape(float x, float y, TerrainType type) {
    sf::ConvexShape hex;
    hex.setPointCount(6);

    // ÇÒ´¨Ø´ 6 ÁØÁ
    for (int i = 0; i < 6; ++i) {
        float angle = 60.0f * i - 30.0f;
        float rad = angle * (PI / 180.0f);
        float px = x + HEX_SIZE * cos(rad);
        float py = y + HEX_SIZE * sin(rad);
        hex.setPoint(i, sf::Vector2f(px, py));
    }

    // ¡ÓË¹´ÊÕµÒÁ»ÃÐàÀ·¾×é¹·Õè
    if (type == TerrainType::Grass) hex.setFillColor(sf::Color(100, 200, 100)); // à¢ÕÂÇ
    else if (type == TerrainType::Water) hex.setFillColor(sf::Color(50, 100, 200)); // ¿éÒ
    else if (type == TerrainType::Mountain) hex.setFillColor(sf::Color(120, 120, 120)); // à·Ò

    hex.setOutlineColor(sf::Color(30, 30, 30)); // ¢Íº´Ó¨Ò§æ
    hex.setOutlineThickness(2.0f);

    return hex;
}

void GameMap::draw(sf::RenderWindow& window) {
    const HexTile* hoveredTile = nullptr;

    // รอบที่ 1: วาดทุกช่อง (Grass, Water, Mountain, Forest) ตามปกติ (PLAY)
    for (const auto& tile : tiles) {
        window.draw(tile.shape);
    }

    // รอบที่ 2: วนหาช่องที่โดน Highlight แล้ววาดทับข้างบนสุดเพียงช่องเดียว
    for (const auto& tile : tiles) {
        if (tile.isHovered) {
            sf::ConvexShape highlightShape = tile.shape; // สร้างตัวสำรองมาวาด
            highlightShape.setOutlineColor(sf::Color::White); // ขอบสีขาว
            highlightShape.setOutlineThickness(4.0f);          // ความหนาพิเศษเพื่อให้เห็นเต็มช่อง
            window.draw(highlightShape);
            break; // เมื่อวาดช่องที่ Highlight เสร็จแล้วให้หยุดลูปทันที
        }
    }
}

void GameMap::updateHighlight(sf::Vector2f mousePos) {
    for (auto& tile : tiles) {
        // ตรวจสอบว่าเมาส์ชี้อยู่ในขอบเขตของหกเหลี่ยมช่องนั้นหรือไม่
        tile.isHovered = tile.shape.getGlobalBounds().contains(mousePos);
        if (tile.shape.getGlobalBounds().contains(mousePos)) {
            tile.isHovered = true;
        } else {
            tile.isHovered = false; // <--- สำคัญมาก: ต้องปิดสถานะช่องอื่นด้วย
        }
    }
}
