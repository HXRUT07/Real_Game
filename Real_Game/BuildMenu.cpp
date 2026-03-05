#include "BuildMenu.h"
#include <sstream>

static const sf::Color COL_BG = sf::Color(15, 12, 10, 248);
static const sf::Color COL_TOPBAR = sf::Color(50, 35, 10);
static const sf::Color COL_GOLD = sf::Color(220, 180, 80);
static const sf::Color COL_DIM = sf::Color(130, 110, 60);
static const sf::Color COL_CARD = sf::Color(30, 25, 15);
static const sf::Color COL_BTN_OK = sf::Color(45, 85, 25);
static const sf::Color COL_BTN_NO = sf::Color(70, 35, 25);

BuildMenu::BuildMenu(float winW, float winH) {
    panelW = 300.f;
    panelH = winH;
    panelX = 0.f;
    panelY = 0.f;

    if (!font.loadFromFile("assets/fonts/Trajan Pro Regular.ttf"))
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    background.setSize({ panelW, panelH });
    background.setPosition(panelX, panelY);
    background.setFillColor(COL_BG);

    topBar.setSize({ panelW, 54.f });
    topBar.setPosition(panelX, panelY);
    topBar.setFillColor(COL_TOPBAR);

    labelTitle.setFont(font);
    labelTitle.setCharacterSize(14);
    labelTitle.setFillColor(COL_GOLD);
    labelTitle.setString("BUILD MENU");
    labelTitle.setPosition(panelX + 18, panelY + 17);

    btnClose.setSize({ 26.f, 26.f });
    btnClose.setPosition(panelX + panelW - 36, panelY + 14);
    btnClose.setFillColor(sf::Color(110, 30, 30));
    btnClose.setOutlineColor(sf::Color(200, 60, 60));
    btnClose.setOutlineThickness(1.f);

    btnCloseText.setFont(font);
    btnCloseText.setCharacterSize(15);
    btnCloseText.setFillColor(sf::Color(255, 200, 200));
    btnCloseText.setString("X");
    btnCloseText.setPosition(panelX + panelW - 28, panelY + 16);

    feedbackText.setFont(font);
    feedbackText.setCharacterSize(13);
    feedbackText.setFillColor(sf::Color(100, 255, 100));

    initSlots();
}

void BuildMenu::initSlots() {
    float cardH = 130.f;
    float gap = 14.f;
    float startY = 70.f;

    for (int i = 0; i < 4; i++) {
        float y = panelY + startY + i * (cardH + gap);

        slots[i].card.setSize({ panelW - 24.f, cardH });
        slots[i].card.setPosition(panelX + 12, y);
        slots[i].card.setFillColor(COL_CARD);
        slots[i].card.setOutlineColor(sf::Color(80, 60, 20));
        slots[i].card.setOutlineThickness(1.f);

        slots[i].nameText.setFont(font);
        slots[i].nameText.setCharacterSize(15);
        slots[i].nameText.setFillColor(COL_GOLD);
        slots[i].nameText.setPosition(panelX + 22, y + 8);

        slots[i].countText.setFont(font);
        slots[i].countText.setCharacterSize(12);
        slots[i].countText.setFillColor(COL_DIM);
        slots[i].countText.setPosition(panelX + 22, y + 30);

        slots[i].descText.setFont(font);
        slots[i].descText.setCharacterSize(12);
        slots[i].descText.setFillColor(sf::Color(180, 200, 160));
        slots[i].descText.setPosition(panelX + 22, y + 48);

        slots[i].costText.setFont(font);
        slots[i].costText.setCharacterSize(12);
        slots[i].costText.setPosition(panelX + 22, y + 68);

        slots[i].btnBuild.setSize({ panelW - 44.f, 30.f });
        slots[i].btnBuild.setPosition(panelX + 22, y + cardH - 36);
        slots[i].btnBuild.setOutlineThickness(1.f);

        slots[i].btnText.setFont(font);
        slots[i].btnText.setCharacterSize(13);
        slots[i].btnText.setPosition(panelX + 80, y + cardH - 30);
    }
}

void BuildMenu::updateSlot(int i) {
    if (!city) return;
    const Building& b = city->getBuilding(i);
    BuildingCost cost = b.getCost();
    bool canAfford = city->canBuild(i);

    slots[i].nameText.setString(b.getName());
    slots[i].countText.setString("Built: " + std::to_string(b.count));
    slots[i].descText.setString(b.getDescription());

    // สีตัวเลขค่าสร้าง (แดง = ไม่พอ)
    std::string costStr =
        "G:" + std::to_string(cost.gold) +
        "  W:" + std::to_string(cost.wood) +
        "  F:" + std::to_string(cost.food);
    slots[i].costText.setString(costStr);
    slots[i].costText.setFillColor(canAfford ? sf::Color(200, 180, 100) : sf::Color(220, 80, 80));

    if (canAfford) {
        slots[i].btnBuild.setFillColor(COL_BTN_OK);
        slots[i].btnBuild.setOutlineColor(sf::Color(100, 180, 60));
        slots[i].btnText.setFillColor(sf::Color(180, 255, 120));
        slots[i].btnText.setString("BUILD");
    }
    else {
        slots[i].btnBuild.setFillColor(COL_BTN_NO);
        slots[i].btnBuild.setOutlineColor(sf::Color(160, 60, 40));
        slots[i].btnText.setFillColor(sf::Color(200, 120, 100));
        slots[i].btnText.setString("NOT ENOUGH");
    }
}

void BuildMenu::setCity(City* c) {
    city = c;
    lastBuiltIdx = -1;
    feedbackTimer = 0;
}

void BuildMenu::clear() {
    city = nullptr;
    feedbackTimer = 0;
}

void BuildMenu::handleEvent(const sf::Event& event) {
    if (!city) return;
    if (event.type != sf::Event::MouseButtonPressed ||
        event.mouseButton.button != sf::Mouse::Left) return;

    sf::Vector2f mp((float)event.mouseButton.x, (float)event.mouseButton.y);

    if (btnClose.getGlobalBounds().contains(mp)) { clear(); return; }

    for (int i = 0; i < 4; i++) {
        if (slots[i].btnBuild.getGlobalBounds().contains(mp)) {
            if (city->build(i)) {
                lastBuiltIdx = i;
                feedbackText.setString("Built: " + city->getBuilding(i).getName() + "!");
                feedbackText.setFillColor(sf::Color(100, 255, 100));
                feedbackTimer = 120;
            }
            else {
                feedbackText.setString("Not enough resources!");
                feedbackText.setFillColor(sf::Color(255, 80, 80));
                feedbackTimer = 120;
            }
            return;
        }
    }
}

void BuildMenu::draw(sf::RenderWindow& window) {
    if (!city) return;

    window.draw(background);
    window.draw(topBar);
    window.draw(labelTitle);
    window.draw(btnClose);
    window.draw(btnCloseText);

    // แสดง stockpile ย่อๆ ใต้ topbar
    sf::Text stockText;
    stockText.setFont(font);
    stockText.setCharacterSize(12);
    stockText.setFillColor(sf::Color(180, 160, 80));
    stockText.setString(
        "G:" + std::to_string(city->getGold()) +
        "  W:" + std::to_string(city->getWood()) +
        "  F:" + std::to_string(city->getFood())
    );
    stockText.setPosition(panelX + 18, panelY + 56);
    window.draw(stockText);

    for (int i = 0; i < 4; i++) {
        updateSlot(i);
        window.draw(slots[i].card);
        window.draw(slots[i].nameText);
        window.draw(slots[i].countText);
        window.draw(slots[i].descText);
        window.draw(slots[i].costText);
        window.draw(slots[i].btnBuild);
        window.draw(slots[i].btnText);
    }

    if (feedbackTimer > 0) {
        feedbackText.setPosition(panelX + 18, panelY + panelH - 40);
        window.draw(feedbackText);
        feedbackTimer--;
    }
}