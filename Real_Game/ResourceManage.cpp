#include "ResourceManage.h"
#include <cstdlib> // สำหรับ rand()

// ฟังก์ชันช่วยสุ่มตัวเลข
int ResourceManage::randomRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}

ResourceYield ResourceManage::generateResources(TerrainType type) {
    ResourceYield loot;

    // กำหนด Logic การดรอปตามประเภทพื้นที่
    switch (type) {
    case TerrainType::Forest:
        // ป่า: ไม้เยอะที่สุด (High Wood)
        loot.wood = randomRange(20, 50); // ได้ไม้ 20-50
        loot.gold = randomRange(0, 5);   // ทองน้อยมาก
        loot.food = randomRange(5, 15);  // อาหารปานกลาง (ของป่า/สัตว์ป่า)
        break;

    case TerrainType::Mountain:
        // ภูเขา: ทองเยอะที่สุด (High Gold)
        loot.wood = randomRange(0, 5);   // ไม้น้อยมาก
        loot.gold = randomRange(20, 50); // ได้ทอง 20-50
        loot.food = randomRange(0, 5);   // อาหารน้อยมาก
        break;

    case TerrainType::Water:
        // แม่น้ำ: อาหารเยอะที่สุด (High Food)
        loot.wood = randomRange(0, 5);
        loot.gold = randomRange(0, 5);
        loot.food = randomRange(20, 50); // ได้อาหาร 20-50 (ปลา/น้ำดื่ม)
        break;

    case TerrainType::Grass:
        // ทุ่งหญ้า: ค่ามาตรฐาน (Balanced) สุ่มได้ทุกอย่างนิดหน่อย
        loot.wood = randomRange(5, 15);
        loot.gold = randomRange(5, 15);
        loot.food = randomRange(5, 15);
        break;

    default:
        // กรณีเผื่อเหลือเผื่อขาด
        loot.wood = 1;
        loot.gold = 1;
        loot.food = 1;
        break;
    }

    return loot;
}