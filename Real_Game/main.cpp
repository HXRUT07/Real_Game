#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "GameMap.h"
#include "MouseUI.h"
#include "GameCamera.h"

int main() {
    // ตั้งค่า Seed สำหรับการสุ่ม
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1080, 720), "Hexa-Conquest", sf::Style::Default, settings);

    //----Map system----//
    GameMap worldMap(50, 50);
    GameCamera camera(1080, 720);
    MouseUI gui;

    // <--- NEW: ตัวแปรเช็คสถานะว่าผู้เล่นเลือกจุดเกิดไปหรือยัง
    bool isPlayerSpawned = false;

    while (window.isOpen()) {
        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            camera.handleEvent(event, window);

            // -----------------------------------------------------------------------
            // ส่วนตรวจสอบการคลิกซ้าย
            // -----------------------------------------------------------------------
           // ... (ภายใน while pollEvent) ...

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {

                    // 1. ดึงตำแหน่งเมาส์และแปลงเป็นพิกัดโลก (จำเป็นต้องทำก่อน)
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());

                    // [จุดที่ 2] : ใส่เงื่อนไขแยกโหมดตรงนี้
                    if (!isPlayerSpawned) {
                        // --- กรณีเริ่มเกมครั้งแรก (ยังไม่เกิด) ---
                        // สั่งให้ map สร้างเมืองที่จุดนี้ และเปลี่ยนสี
                        worldMap.setPlayerCity(worldPos);

                        // เปลี่ยนตัวแปรเป็น true เพื่อบอกว่า "เกิดแล้วนะ"
                        isPlayerSpawned = true;

                        printf("Game Started: City Spawned!\n"); // (เช็คผลใน Console)
                    }
                    else {
                        // --- กรณีเกิดแล้ว (เล่นเกมปกติ) ---
                        // สั่งให้ map ทำงานปกติ (เช่น เดินทัพ/เลือกยูนิต)
                        worldMap.handleMouseClick(worldPos);
                    }
                }
            }

            // ... (โค้ดคลิกขวา หรืออื่นๆ ต่อจากนี้) ...
            // -----------------------------------------------------------------------

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    // Reset การเกิดได้ถ้าต้องการเทสใหม่ (Optional)
                    // isPlayerSpawned = false; 
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    gui.showInfo(mousePosScreen, "Zone Information", "Type: Grassland\nYield: +2 Food");
                }
                else if (event.mouseButton.button == sf::Mouse::Left) {
                    // ถ้าคลิกซ้ายตอนเล่นปกติ ให้ซ่อน GUI
                    if (isPlayerSpawned) gui.hideInfo();
                }
            }
        }

        camera.update(window);
        window.setView(camera.getView());

        // Update Highlight
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
        worldMap.updateHighlight(mousePos);

        gui.update(mousePosScreen, 100, 50);

        window.clear(sf::Color(20, 20, 30));

        window.setView(camera.getView());
        worldMap.draw(window);

        window.setView(window.getDefaultView());
        gui.draw(window);

        window.display();
    }

    return 0;
}