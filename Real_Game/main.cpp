#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "GameMap.h"
#include "MouseUI.h"
#include "GameCamera.h"

int main() {
    // 1. Setup พื้นฐาน
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    // สร้างหน้าต่าง Fullscreen
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    // 2. สร้างระบบต่างๆ
    GameMap worldMap(50, 50); // Map System

    // ส่งขนาดจอจริงไปให้กล้อง (สำคัญมากสำหรับ Fullscreen)
    GameCamera camera(window.getSize().x, window.getSize().y);

    MouseUI gui; // UI System

    // 3. Main Game Loop
    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            // --- System Events ---
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            // --- Camera Events (Zoom/Pan) ---
            camera.handleEvent(event, window);

            // --- Mouse Interaction Events ---
            if (event.type == sf::Event::MouseButtonPressed) {

                // ดึงตำแหน่งเมาส์บนหน้าจอ (Pixel) เอาไว้ใช้กับ UI
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f screenPos = window.mapPixelToCoords(pixelPos, window.getDefaultView());

                // ดึงตำแหน่งเมาส์ในโลกเกม (World) เอาไว้ใช้กับ Map/Unit (ต้องผ่าน Camera View)
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());

                // [Right Click] -> เปิด UI
                if (event.mouseButton.button == sf::Mouse::Right) {
                    gui.showResourcePanel(screenPos, 100, 50, 30);
                }
                // [Left Click] -> Interaction หลัก
                else if (event.mouseButton.button == sf::Mouse::Left) {

                    // 1. จัดการ UI ก่อน (เช่น คลิกเพื่อปิดหน้าต่าง Info)
                    gui.hideInfo();

                    // 2. ส่งให้ GameMap จัดการ (เลือกจุดเกิด หรือ เดิน)
                    // ส่ง worldPos เพราะแผนที่ต้องอิงตามกล้อง
                    worldMap.handleMouseClick(worldPos);
                }
            }
        }

        // --- Update Phase ---
        camera.update(window); // คำนวณการเลื่อนกล้อง

        // คำนวณ Highlight ช่อง (Real-time Hover)
        // ต้องส่ง view ของ camera เข้าไป เพื่อให้เมาส์ชี้ถูกช่องแม้จะเลื่อนกล้องไปไกล
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
        worldMap.updateHighlight(mouseWorldPos);

        // อัปเดต UI (Animation หรือ Tooltip)
        sf::Vector2f mouseScreenPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
        gui.update(mouseScreenPos);

        // --- Draw Phase ---
        window.clear(sf::Color(20, 20, 30));

        // Layer 1: World (Map, Units, Effects)
        window.setView(camera.getView()); // ** สลับไปใช้กล้อง **
        worldMap.draw(window);

        // Layer 2: UI (HUD, Buttons, Info Panels)
        window.setView(window.getDefaultView()); // ** สลับกลับมาเป็นหน้าจอปกติ **
        gui.draw(window);

        window.display();
    }

    return 0;
}