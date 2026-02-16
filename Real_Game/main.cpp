#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib> // <--- เพิ่มตัวนี้สำหรับ rand()
#include <ctime>   // <--- เพิ่มตัวนี้สำหรับ time()
#include <iostream> // <--- เพิ่มสำหรับ cout

#include "GameMap.h" // <--- Game map system (Yu)
#include "MouseUI.h" // <--- USER INTERFACE MOUSE (PLAY)
#include "GameCamera.h" // <--- GAME CAMERA SYSTEM (Yu)
#include "Unit.h"    // <--- UNIT SYSTEM

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
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // 1. ดึงตำแหน่งเมาส์บนหน้าจอ
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

                    // 2. แปลงเป็นตำแหน่งในโลกเกม (World Coords) โดยอิงตามกล้อง
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());

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
                                units.emplace_back(spawnR, spawnC);
                                std::cout << "Commander Spawned at " << spawnR << "," << spawnC << std::endl;
                            }
                        }
                    }
                    // --- PHASE 2: ควบคุมทหาร (Gameplay) ---
                    else {
                        bool clickedOnUnit = false;

                        // A. เช็คว่าคลิกโดนทหารไหม?
                        for (auto& unit : units) {
                            if (unit.isClicked(worldPos)) {
                                clickedOnUnit = true;
                                selectedUnit = &unit;

                                std::cout << "Unit Selected! AP: " << unit.hasAP() << std::endl;

                                // ถ้ามี AP ให้แสดงช่องเดิน (สีเขียว)
                                if (unit.hasAP()) {
                                    worldMap.calculateValidMoves(unit.getR(), unit.getC(), unit.getMoveRange());
                                }
                                else {
                                    std::cout << "No Action Points left!" << std::endl;
                                    worldMap.clearHighlight();
                                }
                                break;
                            }
                        }

                        // B. ถ้าไม่ได้คลิกโดนทหาร แต่มีตัวเลือกค้างไว้ -> สั่งเดิน
                        if (!clickedOnUnit && selectedUnit != nullptr) {
                            int r = 0, c = 0;
                            // แปลงเมาส์เป็น Grid R,C
                            if (worldMap.getGridCoords(worldPos, r, c)) {

                                // เช็คว่าช่องนี้เป็นสีเขียวไหม (เดินได้ไหม?)
                                if (worldMap.isValidMove(r, c)) {
                                    // 1. ย้ายตำแหน่ง
                                    selectedUnit->moveTo(r, c);
                                    // 2. หัก AP
                                    selectedUnit->consumeAP(1);
                                    // 3. เปิดหมอก
                                    worldMap.revealFog(r, c, 1);

                                    // จบการเดิน: เคลียร์ไฮไลท์
                                    worldMap.clearHighlight();
                                    selectedUnit = nullptr;
                                    std::cout << "Unit Moved!" << std::endl;
                                }
                                else {
                                    std::cout << "Cannot move! (Blocked or unseen)" << std::endl;
                                }
                            }
                        }
                        // C. คลิกพื้นที่ว่างๆ โดยไม่เลือกใคร -> ยกเลิกการเลือก
                        else if (!clickedOnUnit) {
                            selectedUnit = nullptr;
                            worldMap.clearHighlight();
                        }
                    }
                }
            }
            // -----------------------------------------------------------------------

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    // เรียกใช้แถบข้อมูลเมื่อคลิกขวา
                    gui.showResourcePanel((float)window.getSize().x, 100, 50, 30);

                    // (แถม) คลิกขวาให้ยกเลิกการเลือกทหารด้วย
                    selectedUnit = nullptr;
                    worldMap.clearHighlight();
                }
                else if (event.mouseButton.button == sf::Mouse::Left) {
                    // คลิกซ้ายเพื่อซ่อน (เลือกอย่างใดอย่างหนึ่ง)
                    gui.hideInfo();
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
        gui.update(mousePosScreen);

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