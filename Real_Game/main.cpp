#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib> // <--- เพิ่มตัวนี้สำหรับ rand()
#include <ctime>   // <--- เพิ่มตัวนี้สำหรับ time()
#include <iostream> // <--- เพิ่มสำหรับ cout
#include <string>   // <--- เพิ่มสำหรับ string

#include "GameMap.h"     // <--- Game map system (Yu)
#include "MouseUI.h"     // <--- USER INTERFACE MOUSE (PLAY)
#include "GameCamera.h"  // <--- GAME CAMERA SYSTEM (Yu)
#include "Unit.h"        // <--- UNIT SYSTEM
#include "ResourceManage.h" // <--- [NEW] เพิ่ม Header ของระบบทรัพยากร

int main() {
    // ตั้งค่า Seed สำหรับการสุ่ม (ใส่ใน Main ทีเดียวจบ)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);

    // (แถม) จำกัดเฟรมเรตหน่อย เครื่องจะได้ไม่ทำงานหนักเกินไปตอน Fullscreen
    window.setFramerateLimit(60);

    //----Map system----//(Yu)
    // 1. สร้าง Map แค่บรรทัดเดียว!
    GameMap worldMap(50, 50);

    // 2. สร้าง Object กล้อง
    GameCamera camera(window.getSize().x, window.getSize().y);

    MouseUI gui; //(PLAY)

    //----Unit System----//
    std::vector<Unit> units;       // เก็บยูนิตทั้งหมด
    Unit* selectedUnit = nullptr;  // ตัวที่กำลังเลือกอยู่
    bool isGameRunning = false;    // ตัวแปรเช็คว่าจบช่วงเลือกจุดเกิดหรือยัง
    int unitNameCounter = 1;       // ตัวนับสำหรับตั้งชื่อ Unit อัตโนมัติ

    while (window.isOpen()) {

        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // กด Escape เพื่อออกจากเกมได้ (สำคัญมากตอนเทส Fullscreen ไม่งั้นออกยาก)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            // 3. ส่ง Event ให้กล้องจัดการ (คลิก/ปล่อย/หมุนล้อ)
            camera.handleEvent(event, window);

            // -----------------------------------------------------------------------
            // ส่วนตรวจสอบการคลิกซ้าย: เลือกจุดเกิด (Spawn) และ ควบคุมยูนิต (Gameplay)
            // -----------------------------------------------------------------------
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());
                sf::Vector2f uiPos = window.mapPixelToCoords(pixelPos, window.getDefaultView()); // ตำแหน่งสำหรับ UI

                if (event.mouseButton.button == sf::Mouse::Right) {
                    // คลิกขวา: ยกเลิกการเลือก และ แสดง Resource Panel
                    gui.clearSelection();
                    selectedUnit = nullptr;
                    worldMap.clearHighlight();

                    // --- ส่วนคำนวณทรัพยากร (Resource Integration) ---
                    int r_res = 0, c_res = 0;

                    // 1. เช็คว่าคลิกโดน Grid จริงๆ หรือไม่
                    if (worldMap.getGridCoords(worldPos, r_res, c_res)) {

                        // --- [UPDATED] ระบบ Lock Resource (สุ่มครั้งเดียวจำตลอดไป) ---

                        // 2. ดึง Pointer ของช่องนั้นมา (ตัวจริงจาก Memory)
                        HexTile* tile = worldMap.getTile(r_res, c_res);

                        if (tile != nullptr) {
                            // 3. เช็คว่าเคยสุ่มของไปหรือยัง?
                            if (tile->hasResourcesGenerated) {
                                // A. เคยสุ่มแล้ว (LOAD OLD DATA): ให้ดึงค่าเดิมที่บันทึกไว้มาใช้เลย
                                std::cout << "[LOAD] Tile (" << r_res << "," << c_res << ") - Loading stored resources." << std::endl;

                                // ส่งค่าเดิมไปโชว์ที่ UI
                                gui.showResourcePanel((float)window.getSize().x, tile->storedWood, tile->storedGold, tile->storedFood);
                            }
                            else {
                                // B. ยังไม่เคยสุ่ม (GENERATE NEW): ให้สุ่มใหม่ แล้วบันทึกเก็บไว้ (Save)
                                std::cout << "[NEW] Tile (" << r_res << "," << c_res << ") - Generating first time." << std::endl;

                                // สุ่มทรัพยากรตามประเภทพื้นที่
                                ResourceYield loot = ResourceManage::generateResources(tile->type);

                                // บันทึกค่าลง Tile ทันที
                                tile->storedWood = loot.wood;
                                tile->storedGold = loot.gold;
                                tile->storedFood = loot.food;
                                tile->hasResourcesGenerated = true; // <--- ติ๊กถูกว่าช่องนี้มีของแล้ว ห้ามสุ่มใหม่

                                // ส่งค่าใหม่ไปโชว์ที่ UI
                                gui.showResourcePanel((float)window.getSize().x, loot.wood, loot.gold, loot.food);
                            }
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Left) {
                    // คลิกซ้าย: ซ่อน Info Panel เดิมก่อน
                    gui.hideInfo();

                    // --- PHASE 1: เลือกจุดเกิด ---
                    if (!isGameRunning) {
                        // 3. ส่งให้ GameMap จัดการเลือกจุดเกิด
                        worldMap.handleMouseClick(worldPos);

                        // เช็คว่า Map เริ่มเกมสำเร็จหรือยัง? (ถ้าเลือกจุดเกิดแล้ว Map จะตั้ง flag ว่าเริ่มเกม)
                        if (worldMap.isGameStarted()) {
                            isGameRunning = true;

                            // *** Spawn ทหารตัวแรกตรงจุดที่คลิก ***
                            int spawnR = 0, spawnC = 0;
                            // ต้องใช้ฟังก์ชัน getGridCoords ที่เพิ่มใน GameMap.h
                            if (worldMap.getGridCoords(worldPos, spawnR, spawnC)) {
                                units.emplace_back("Commander", spawnR, spawnC); // ตั้งชื่อ Commander
                                std::cout << "Commander Spawned at " << spawnR << "," << spawnC << std::endl;
                            }
                        }
                    }
                    // --- PHASE 2: ควบคุมทหาร (Gameplay) ---
                    else {
                        int r = 0, c = 0;
                        // ตรวจสอบว่าคลิกโดนช่องไหนใน Grid
                        if (worldMap.getGridCoords(worldPos, r, c)) {

                            // 1. หา Unit ทั้งหมดที่อยู่ในช่องนี้ (Stacking)
                            std::vector<Unit*> stackInTile;
                            for (auto& u : units) {
                                if (u.getR() == r && u.getC() == c) {
                                    stackInTile.push_back(&u);
                                }
                            }

                            // กรณี A: คลิกโดนช่องที่มี Unit (เลือก Unit)
                            if (!stackInTile.empty()) {
                                // ส่งรายการ Unit ไปให้ UI แสดงผลทางขวา
                                gui.setSelectionList(stackInTile);

                                // Auto-select: เลือกตัวแรกที่มี AP เหลือ
                                selectedUnit = nullptr;
                                for (auto* u : stackInTile) {
                                    if (u->hasAP()) {
                                        selectedUnit = u;
                                        break;
                                    }
                                }

                                // ถ้าเลือกได้ ให้คำนวณและแสดงช่องเดิน
                                if (selectedUnit) {
                                    worldMap.calculateValidMoves(selectedUnit->getR(), selectedUnit->getC(), selectedUnit->getMoveRange());
                                    std::cout << "Unit Selected: " << selectedUnit->getName() << std::endl;
                                }
                                else {
                                    worldMap.clearHighlight(); // ไม่มีตัวไหนมี AP
                                    std::cout << "All units in this stack have no AP." << std::endl;
                                }
                            }
                            // กรณี B: คลิกพื้นที่ว่าง และมี Unit ถูกเลือกอยู่ (สั่งเดิน)
                            else if (selectedUnit != nullptr) {
                                // ตรวจสอบว่าช่องเป้าหมายเดินไปได้หรือไม่ (สีเขียว)
                                if (worldMap.isValidMove(r, c)) {
                                    // 1. ย้ายตำแหน่ง
                                    selectedUnit->moveTo(r, c);
                                    // 2. หัก AP
                                    selectedUnit->consumeAP(1);
                                    // 3. เปิดหมอก (ระยะ 1 ช่อง)
                                    worldMap.revealFog(r, c, 1);

                                    // จบการเดิน: เคลียร์ไฮไลท์ และ ยกเลิกการเลือก
                                    worldMap.clearHighlight();
                                    gui.clearSelection(); // ปิดแถบขวา
                                    selectedUnit = nullptr;
                                    std::cout << "Unit Moved!" << std::endl;
                                }
                                else {
                                    // เดินไม่ได้ (อาจจะติดสิ่งกีดขวาง หรือ อยู่นอกระยะ)
                                    // ให้ยกเลิกการเลือกไปเลย หรือ จะแค่แจ้งเตือนก็ได้
                                    // ในที่นี้เลือกที่จะยกเลิกการเลือกเพื่อความลื่นไหล
                                    gui.clearSelection();
                                    selectedUnit = nullptr;
                                    worldMap.clearHighlight();
                                    std::cout << "Cannot move! (Blocked or unseen)" << std::endl;
                                }
                            }
                            // กรณี C: คลิกพื้นที่ว่างเปล่า โดยไม่มี Unit ถูกเลือก
                            else {
                                gui.clearSelection();
                                selectedUnit = nullptr;
                                worldMap.clearHighlight();
                            }
                        }
                    }
                }
            }
            // -----------------------------------------------------------------------

            // [DEBUG / TEST] Key Controls
            if (event.type == sf::Event::KeyPressed) {
                // Spacebar: สุ่มเสก Unit เพิ่ม (เฉพาะตอนเริ่มเกมแล้ว)
                if (event.key.code == sf::Keyboard::Space && isGameRunning) {
                    int r = std::rand() % 50;
                    int c = std::rand() % 50;
                    std::string name = "Unit " + std::to_string(unitNameCounter++);
                    units.emplace_back(name, r, c);
                    std::cout << "Spawned new unit: " << name << " at " << r << "," << c << std::endl;
                }
                // R: รีเซ็ต AP ของทุก Unit (จำลองการจบ Turn)
                if (event.key.code == sf::Keyboard::R) {
                    for (auto& u : units) u.resetAP();
                    std::cout << "Next Turn: All AP Reset" << std::endl;
                }
            }
        }

        // 4. อัปเดตกล้อง (คำนวณการเลื่อน)
        camera.update(window);

        // 5. นำ View จากกล้องมาใส่ window ก่อนจะทำอย่างอื่น
        window.setView(camera.getView());

        // --- Logic การตรวจสอบ Highlight ---
        // จุดสำคัญ: ต้องส่ง view ของ camera เข้าไปใน mapPixelToCoords ด้วย
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());

        // เราจะส่ง mousePos ไปให้ worldMap ตรวจสอบว่าชี้ที่ช่องไหน
        worldMap.updateHighlight(mousePos);

        // อัปเดต UI (ไม่ได้ทำอะไรมากในตอนนี้ แต่ใส่ไว้ตามโครงสร้างเดิม)
        // gui.update(mousePosScreen); 

        window.clear(sf::Color(20, 20, 30)); // พื้นหลังสีน้ำเงินเข้มๆ เหมือนอวกาศ

        // สั่งวาด Map แค่บรรทัดเดียว!
        window.setView(camera.getView()); // ใช้ View กล้องวาดแมพ
        worldMap.draw(window);

        // --- วาดทหาร (ต้องวาดหลังจาก Map แต่อยู่ใน View ของกล้อง) ---
        for (auto& unit : units) {
            unit.draw(window);
        }

        window.setView(window.getDefaultView()); // คืนค่า View ปกติเพื่อวาด UI ทับข้างบนสุด
        gui.draw(window);

        window.display();
    }

    return 0;
}