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
#include "ResourceManage.h" // <--- เพิ่ม Header ของระบบทรัพยากร
#include "TurnManager.h" // <--- ระบบเทิร์น

int main() {
    // ตั้งค่า Seed สำหรับการสุ่ม (ใส่ใน Main ทีเดียวจบ)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);

    // จำกัดเฟรมเรตหน่อย เครื่องจะได้ไม่ทำงานหนักเกินไปตอน Fullscreen
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
    TurnManager turnSys(2); // สร้างระบบเทิร์นสำหรับ 2 ผู้เล่น

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
                    // 1. ยกเลิกการเลือกทหารและไฮไลท์
                    gui.clearSelection();
                    selectedUnit = nullptr;
                    worldMap.clearHighlight();

                    // 2. ดึงข้อมูลทรัพยากรจากช่องที่คลิก
                    int r = 0, c = 0;
                    if (worldMap.getGridCoords(worldPos, r, c)) {
                        HexTile* clickedTile = worldMap.getTile(r, c);

                        // กฎ: ต้องมีช่องนี้อยู่จริง และ "ต้องเคยสำรวจแล้ว (isExplored)" เท่านั้น
                        if (clickedTile != nullptr && clickedTile->isExplored) {
                            // โชว์ข้อมูลทรัพยากรของช่องนั้น
                            gui.showResourcePanel((float)window.getSize().x, clickedTile->gold, clickedTile->wood, clickedTile->food);
                        }
                        else {
                            // ถ้าคลิกขวาใส่หมอกดำๆ ให้ปิดหน้าต่างทิ้ง
                            gui.hideInfo();
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
                                // [อัปเดต] ใส่เลข 1 ด้านหลังเพื่อให้ตัวนี้เป็นของ Player 1 และสร้างศัตรู Player 2
                                units.emplace_back("Commander", spawnR, spawnC, 1);
                                units.emplace_back("Enemy", spawnR + 2, spawnC + 2, 2);
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

                                // ตอนจะ Auto-select หาตัวที่มี AP ให้เพิ่มเงื่อนไขตรวจเช็คเจ้าของด้วย
                                selectedUnit = nullptr;
                                for (auto* u : stackInTile) {
                                    // กฎ: ต้องมี AP และ "ต้องเป็นของ Player ปัจจุบันเท่านั้น!"
                                    if (u->hasAP() && u->getOwner() == turnSys.getCurrentPlayer()) {
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
                                    std::cout << "All units in this stack have no AP or not your turn." << std::endl;
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
                // R: รีเซ็ต AP ของทุก Unit (จำลองการจบ Turn)
                if (event.key.code == sf::Keyboard::R) {
                    for (auto& u : units) u.resetAP();
                    std::cout << "Next Turn: All AP Reset" << std::endl;
                }
            }

            // [แก้บัค] ใช้ KeyReleased (ปล่อยนิ้ว) และแก้จาก Enter เป็น Return
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Return && isGameRunning) {
                    turnSys.endTurn(units); // เรียกสลับเทิร์นและรีเซ็ต AP

                    // เคลียร์ UI ที่เลือกค้างไว้
                    gui.clearSelection();
                    selectedUnit = nullptr;
                    worldMap.clearHighlight();

                    std::cout << ">>> Switched to Player " << turnSys.getCurrentPlayer() << " <<<" << std::endl;
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

        // อัปเดต UI
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