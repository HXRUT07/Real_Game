#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib> // <--- เพิ่มตัวนี้สำหรับ rand()
#include <ctime>   // <--- เพิ่มตัวนี้สำหรับ time()
#include "GameMap.h" // <--- Game map system (Yu)
#include "MouseUI.h" // <--- USER INTERFACE MOUSE (PLAY)
#include "GameCamera.h" // <--- GAME CAMERA SYSTEM (Yu)

int main() {
    // ตั้งค่า Seed สำหรับการสุ่ม (ใส่ใน Main ทีเดียวจบ)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1080, 720), "Hexa-Conquest", sf::Style::Default, settings);

    //----Map system----//(Yu)
    // 1. สร้าง Map แค่บรรทัดเดียว!
    GameMap worldMap(50, 50);

    // 2. สร้าง Object กล้อง
    GameCamera camera(1080, 720);

    MouseUI gui; //(PLAY)

    while (window.isOpen()) {

        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // 3. ส่ง Event ให้กล้องจัดการ (คลิก/ปล่อย/หมุนล้อ)
            camera.handleEvent(event, window);

            // -----------------------------------------------------------------------
            // เพิ่มส่วนตรวจสอบการคลิกซ้าย เพื่อเลือกจุดเกิด (Spawn Selection)
            // -----------------------------------------------------------------------
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // 1. ดึงตำแหน่งเมาส์บนหน้าจอ
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

                    // 2. แปลงเป็นตำแหน่งในโลกเกม (World Coords) โดยอิงตามกล้อง
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());

                    // 3. ส่งให้ GameMap จัดการ (ถ้ายังไม่เริ่มเกม มันจะใช้เลือกจุดเกิด)
                    worldMap.handleMouseClick(worldPos);
                }
            }
            // -----------------------------------------------------------------------

            // [DEBUG / TEST] กดปุ่ม Spacebar เพื่อสุ่มเปิดแมพ (ไว้เทสว่าระบบทำงานไหม)
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    int r = std::rand() % 30;
                    int c = std::rand() % 30;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    // เรียกใช้แถบข้อมูลเมื่อคลิกขวา
                    gui.showResourcePanel(mousePosScreen, 100, 50, 30);
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
        // ไม่อย่างนั้นเมาส์จะชี้ไม่ตรงตำแหน่งเมื่อมีการเลื่อนกล้อง
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
        // เราจะส่ง mousePos ไปให้ worldMap ตรวจสอบว่าชี้ที่ช่องไหน
        worldMap.updateHighlight(mousePos);
        gui.update(mousePosScreen);

        window.clear(sf::Color(20, 20, 30)); // พื้นหลังสีน้ำเงินเข้มๆ เหมือนอวกาศ

        // สั่งวาด Map แค่บรรทัดเดียว!
        window.setView(camera.getView()); // ใช้ View กล้องวาดแมพ
        worldMap.draw(window);

        window.setView(window.getDefaultView()); // คืนค่า View ปกติเพื่อวาด UI ทับข้างบนสุด
        gui.draw(window);

        window.display();
    }

    return 0;
}