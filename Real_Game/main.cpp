#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib> // <--- เพิ่มตัวนี้สำหรับ rand()
#include <ctime>   // <--- เพิ่มตัวนี้สำหรับ time()
#include "GameMap.h" // <--- Game map system (Yu)
#include "MouseUI.h" // <--- USER INTERFACE MOUSE (PLAY)
#include "GameCamera.h" // <--- GAME CAMERA SYSTEM (Yu)

int main() {
    // ตั้งค่า Seed สำหรับการสุ่ม
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);

    //----Map system----//(Yu)
    // 1. สร้าง Map แค่บรรทัดเดียว!
    GameMap worldMap(50, 50);

    // 2. สร้าง Object กล้อง
    GameCamera camera(1080, 720);

    // <--- NEW: ตัวแปรเช็คสถานะว่าผู้เล่นเลือกจุดเกิดไปหรือยัง
    bool isPlayerSpawned = false;

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

            // 3. ส่ง Event ให้กล้องจัดการ (คลิก/ปล่อย/หมุนล้อ)
            camera.handleEvent(event, window);

            // -----------------------------------------------------------------------
            // เพิ่มส่วนตรวจสอบการคลิกซ้าย เพื่อเลือกจุดเกิด (Spawn Selection)
            // -----------------------------------------------------------------------
           // ... (ภายใน while pollEvent) ...

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
                    gui.showInfo(mousePosScreen, "Zone Information", "Type: Grassland\nYield: +2 Food");
                }
                else if (event.mouseButton.button == sf::Mouse::Left) {
                    // คลิกซ้ายเพื่อซ่อน (เลือกอย่างใดอย่างหนึ่ง)
                    gui.hideInfo();
                }
            }

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

        camera.update(window);
        window.setView(camera.getView());

        // --- Logic การตรวจสอบ Highlight ---
        // จุดสำคัญ: ต้องส่ง view ของ camera เข้าไปใน mapPixelToCoords ด้วย
        // ไม่อย่างนั้นเมาส์จะชี้ไม่ตรงตำแหน่งเมื่อมีการเลื่อนกล้อง
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
        // เราจะส่ง mousePos ไปให้ worldMap ตรวจสอบว่าชี้ที่ช่องไหน
        worldMap.updateHighlight(mousePos);
        gui.update(mousePosScreen, 100, 50);

        window.clear(sf::Color(20, 20, 30));

        window.setView(camera.getView());
        worldMap.draw(window);

        window.setView(window.getDefaultView()); // คืนค่า View ปกติเพื่อวาด UI ทับข้างบนสุด
        gui.draw(window);

        window.display();
    }

    return 0;
}