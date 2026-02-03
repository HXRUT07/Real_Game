#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "GameMap.h" // <--- อย่าลืม include ตัวนี้
#include "MouseUI.h" // <--- USER INTERFACE MOUSE (PLAY)
#include "GameCamera.h" // <--- GAME CAMERA SYSTEM (Yu)

int main() {
    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1080, 720), "Hexa-Conquest", sf::Style::Default, settings);

    //----Map system----//(Yu)
    // 1. สร้าง Map แค่บรรทัดเดียว! (ขนาด 20 แถว x 25 คอลัมน์)
    GameMap worldMap(20, 25);

    // 2. สร้าง Object กล้อง
    GameCamera camera(1080, 720);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // 3. ส่ง Event ให้กล้องจัดการ (คลิก/ปล่อย/หมุนล้อ)
            camera.handleEvent(event, window);
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