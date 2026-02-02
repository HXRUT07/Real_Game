#ifndef HEX_LOGIC_H
#define HEX_LOGIC_H

#include <string>
#include <cstdlib>

// โครงสร้างทรัพยากร
struct Resource {
    int gold, wood, food;
    void randomize() {
        gold = std::rand() % 100;
        wood = std::rand() % 50;
        food = std::rand() % 80;
    }
    std::string toString() const {
        return "G: " + std::to_string(gold) + " W: " + std::to_string(wood) + " F: " + std::to_string(food);
    }
};

// ฟังก์ชันเช็คเพื่อนบ้าน 6 ทิศทาง (แก้ปัญหา ขึ้นซ้าย-ลงขวา)
inline bool isNeighbor(int curQ, int curR, int tQ, int tR) {
    int dq = tQ - curQ;
    int dr = tR - curR;

    // เพื่อนบ้านสำหรับแถวคู่ (r % 2 == 0)
    // ทิศ: [บนซ้าย, บนขวา, ขวา, ลงขวา, ลงซ้าย, ซ้าย]
    int neighborsEven[6][2] = { {-1,-1}, {0,-1}, {1,0}, {0,1}, {-1,1}, {-1,0} };

    // เพื่อนบ้านสำหรับแถวคี่ (r % 2 != 0)
    int neighborsOdd[6][2] = { {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,0} };

    if (curR % 2 == 0) {
        for (int i = 0; i < 6; i++) {
            if (dq == neighborsEven[i][0] && dr == neighborsEven[i][1]) return true;
        }
    }
    else {
        for (int i = 0; i < 6; i++) {
            if (dq == neighborsOdd[i][0] && dr == neighborsOdd[i][1]) return true;
        }
    }
    return false;
}

#endif