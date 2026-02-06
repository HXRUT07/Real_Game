#include "MouseUI.h"
#include <random>
#include <array>

MouseUI::MouseUI() {
    hasFont = font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf");
    // เพิ่มบรรทัดนี้เพื่อกำหนดค่าเริ่มต้น!
    isPanelVisible = false;

    infoPanel.setSize(sf::Vector2f(220.f, 140.f));
    infoPanel.setFillColor(sf::Color(30, 30, 30, 220));
    infoPanel.setOutlineThickness(2.f);
    infoPanel.setOutlineColor(sf::Color(100, 200, 255));

    if (hasFont) {
        infoContent.setFont(font);
        infoContent.setCharacterSize(20);
        infoContent.setFillColor(sf::Color::White);
    }
}

void MouseUI::showResourcePanel(float windowWidth, int gold, int wood, int food) {
    std::array<int, 3> values = { 0, 0, 0 };

    // สุ่มค่า 10–99
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(10, 99);

    // ใส่ค่าที่ไม่ซ้ำ
    values[0] = dist(gen);
    do { values[1] = dist(gen); } while (values[1] == values[0]);
    do { values[2] = dist(gen); } while (values[2] == values[0] || values[2] == values[1]);

    gold = values[0];
    wood = values[1];
    food = values[2];

    isPanelVisible = true;

    float padding = 20.f; // ระยะห่างจากขอบจอ
    float panelWidth = infoPanel.getSize().x;

    // คำนวณตำแหน่ง: เอาความกว้างจอ ลบด้วยความกว้างแถบ และลบระยะห่าง
    float posX = windowWidth - infoPanel.getSize().x - padding;
    float posY = padding; // ห่างจากขอบบน 20 pixel

    infoPanel.setPosition(posX, posY);

    infoContent.setPosition(posX + 14.f, posY + 12.f);

    infoContent.setString(

        "Resources in this area\n"
        "\n"
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food)

    );
}

void MouseUI::hideInfo() {
    isPanelVisible = false;
}

void MouseUI::update(sf::Vector2f mousePos) {
    // ตอนนี้ยังไม่ต้องทำอะไร
}

void MouseUI::draw(sf::RenderWindow& window) {
    if (!hasFont) return;

    if (isPanelVisible) {
        window.draw(infoPanel);
        window.draw(infoContent);
    }
}
