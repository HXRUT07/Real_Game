#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "GameMap.h" // <--- อย่าลืม include ตัวนี้
#include "MouseUI.h" // <--- USER INTERFACE MOUSE (PLAY)

int main() {
    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1080, 720), "Hexa-Conquest", sf::Style::Default, settings);

    //--------------(ยู)----------------//
    //----Map system----//
    // 1. สร้าง Map แค่บรรทัดเดียว! (ขนาด 20 แถว x 25 คอลัมน์)
    GameMap worldMap(20, 25);

    while (window.isOpen()) {

        // --- จุดสำคัญ: อัปเดตตำแหน่งเมาส์ทุกเฟรม ---
        // ใช้ mapPixelToCoords เพื่อให้พิกัดเมาส์แม่นยำแม้จะมีการย่อขยายหน้าต่าง
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // --- Logic การตรวจสอบ Highlight ---
        // เราจะส่ง mousePos ไปให้ worldMap ตรวจสอบว่าชี้ที่ช่องไหน
        worldMap.updateHighlight(mousePos);

        window.clear(sf::Color(20, 20, 30)); // พื้นหลังสีน้ำเงินเข้มๆ เหมือนอวกาศ

        // 2. สั่งวาด Map แค่บรรทัดเดียว!
        worldMap.draw(window);

        window.display();
    }

    //--------------(เพลย์)----------------//

    //--------------(เปรม)----------------//

    //--------------(ปลื้ม)----------------//

    //--------------(อาฉะ)----------------//

    return 0;
}