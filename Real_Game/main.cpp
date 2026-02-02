#include <SFML/Graphics.hpp>
#include "GameMap.h" // <--- อย่าลืม include ตัวนี้

int main() {
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Hexa-Conquest");

    // 1. สร้าง Map แค่บรรทัดเดียว! (ขนาด 20 แถว x 25 คอลัมน์)
    GameMap worldMap(20, 25);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(20, 20, 30)); // พื้นหลังสีน้ำเงินเข้มๆ เหมือนอวกาศ

        // 2. สั่งวาด Map แค่บรรทัดเดียว!
        worldMap.draw(window);

        window.display();
    }

    return 0;
}