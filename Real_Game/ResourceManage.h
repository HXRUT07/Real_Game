#pragma once
#include "GameMap.h" // จำเป็นต้อง include เพื่อให้รู้จัก TerrainType

// Struct สำหรับเก็บผลลัพธ์ทรัพยากรที่ดรอปได้
struct ResourceYield {
    int wood = 0;
    int gold = 0;
    int food = 0;
};

class ResourceSystem {
public:
    // ฟังก์ชันหลัก: รับประเภทพื้นที่ -> คืนค่าทรัพยากรที่คำนวณแล้ว
    // ใช้ static เพื่อให้เรียกใช้ได้เลยโดยไม่ต้องสร้าง object (เช่น ResourceSystem::generate(...))
    static ResourceYield generateResources(TerrainType type);

private:
    // Helper: ฟังก์ชันช่วยสุ่มตัวเลข (min ถึง max)
    static int randomRange(int min, int max);
};
