#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "GameMap.h" // <--- ÍÂèÒÅ×Á include µÑÇ¹Õé
#include "MouseUI.h" // <--- USER INTERFACE MOUSE (PLAY)

int main() {
    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1080, 720), "Hexa-Conquest");

	//--------------(ÂÙ)----------------//
	//----Map system----//
    // 1. ÊÃéÒ§ Map á¤èºÃÃ·Ñ´à´ÕÂÇ! (¢¹Ò´ 20 á¶Ç x 25 ¤ÍÅÑÁ¹ì)
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

        window.clear(sf::Color(20, 20, 30)); // ¾×é¹ËÅÑ§ÊÕ¹éÓà§Ô¹à¢éÁæ àËÁ×Í¹ÍÇ¡ÒÈ

        // 2. ÊÑè§ÇÒ´ Map á¤èºÃÃ·Ñ´à´ÕÂÇ!
        worldMap.draw(window);

        window.display();
    }

	//--------------(à¾ÅÂì)----------------//

	//--------------(à»ÃÁ)----------------//

	//--------------(»Å×éÁ)----------------//

    //--------------(ÍÒ©Ð)----------------//

    return 0;
}