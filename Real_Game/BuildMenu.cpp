#include "BuildMenu.h"
#include <sstream>

static const sf::Color COL_BG = sf::Color(12, 10, 6, 230);
static const sf::Color COL_TOPBAR = sf::Color(20, 16, 8, 255);
static const sf::Color COL_TOPBAR_WAIT = sf::Color(20, 50, 90, 255);
static const sf::Color COL_GOLD = sf::Color(200, 170, 90, 255);
static const sf::Color COL_DIM = sf::Color(160, 130, 45, 200);
static const sf::Color COL_CARD = sf::Color(18, 14, 8, 240);
static const sf::Color COL_BTN_OK = sf::Color(45, 85, 25);
static const sf::Color COL_BTN_NO = sf::Color(70, 35, 25);
static const sf::Color COL_BTN_WAIT = sf::Color(20, 60, 100);
static const sf::Color COL_RIM = sf::Color(160, 130, 45, 200);
static const sf::Color COL_RECRUIT = sf::Color(30, 60, 100);
BuildMenu::BuildMenu(float winW, float winH) {
    panelW = 320.f;
    panelH = winH - 16.f;
    panelX = 8.f;
    panelY = 8.f;

    if (!font.loadFromFile("assets/fonts/Trajan Pro Regular.ttf"))
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    background.setSize({ panelW, panelH });
    background.setPosition(panelX, panelY);
    background.setFillColor(COL_BG);

    topBar.setSize({ panelW, 54.f });
    topBar.setPosition(panelX, panelY);
    topBar.setFillColor(COL_TOPBAR);

    labelTitle.setFont(font);
    labelTitle.setCharacterSize(16);
    labelTitle.setStyle(sf::Text::Bold);
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

    placementHintText.setFont(font);
    placementHintText.setCharacterSize(14);
    placementHintText.setFillColor(sf::Color(100, 220, 255));
    placementHintText.setPosition(panelX + 18, panelY + panelH - 80);

    initSlots();
}

void BuildMenu::initSlots() {
    float cardH = 140.f;
    float gap = 14.f;
    float startY = 90.f;

    for (int i = 0; i < 4; i++) {
        float y = panelY + startY + i * (cardH + gap);

        slots[i].card.setSize({ panelW - 24.f, cardH });
        slots[i].card.setPosition(panelX + 12, y);
        slots[i].card.setFillColor(COL_CARD);
        slots[i].card.setOutlineColor(sf::Color(160, 130, 45, 150));
        slots[i].card.setOutlineThickness(1.2f);

        slots[i].nameText.setFont(font);
        slots[i].nameText.setCharacterSize(17);
        slots[i].nameText.setStyle(sf::Text::Bold);
        slots[i].nameText.setFillColor(COL_GOLD);
        slots[i].nameText.setPosition(panelX + 22, y + 8);

        slots[i].countText.setFont(font);
        slots[i].countText.setCharacterSize(13);
        slots[i].countText.setFillColor(COL_DIM);
        slots[i].countText.setPosition(panelX + 22, y + 30);

        slots[i].descText.setFont(font);
        slots[i].descText.setCharacterSize(13);
        slots[i].descText.setFillColor(sf::Color(180, 200, 160));
        slots[i].descText.setPosition(panelX + 22, y + 48);

        slots[i].costText.setFont(font);
        slots[i].costText.setCharacterSize(13);
        slots[i].costText.setPosition(panelX + 22, y + 68);

        float btnW = (i == 1) ? (panelW - 54.f) / 2.f - 4.f : panelW - 44.f;
        slots[i].btnBuild.setSize({ btnW, 30.f });
        slots[i].btnBuild.setPosition(panelX + 22, y + cardH - 36);
        slots[i].btnBuild.setOutlineThickness(1.f);

        slots[i].btnText.setFont(font);
        slots[i].btnText.setCharacterSize(14);
        slots[i].btnText.setStyle(sf::Text::Bold);
        slots[i].btnText.setPosition(panelX + 30, y + cardH - 30);

        // ปุ่ม RECRUIT เฉพาะ Barracks (index 1)
        if (i == 1) {
            float recruitX = panelX + 22 + btnW + 8.f;
            slots[i].btnRecruit.setSize({ btnW, 30.f });
            slots[i].btnRecruit.setPosition(recruitX, y + cardH - 36);
            slots[i].btnRecruit.setOutlineThickness(1.f);
            slots[i].btnRecruit.setFillColor(COL_RECRUIT);
            slots[i].btnRecruit.setOutlineColor(sf::Color(80, 140, 220));

            slots[i].btnRecruitText.setFont(font);
            slots[i].btnRecruitText.setCharacterSize(12);
            slots[i].btnRecruitText.setFillColor(sf::Color(150, 200, 255));
            slots[i].btnRecruitText.setPosition(recruitX + 6, y + cardH - 30);
        }
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

    std::string costStr =
        "G:" + std::to_string(cost.gold) +
        "  W:" + std::to_string(cost.wood) +
        "  F:" + std::to_string(cost.food);
    slots[i].costText.setString(costStr);
    slots[i].costText.setFillColor(canAfford ? sf::Color(200, 180, 100) : sf::Color(220, 80, 80));

    if (pendingBuildIdx == i) {
        slots[i].btnBuild.setFillColor(COL_BTN_WAIT);
        slots[i].btnBuild.setOutlineColor(sf::Color(80, 160, 255));
        slots[i].btnText.setFillColor(sf::Color(150, 210, 255));
        slots[i].btnText.setString("PLACING...");
    }
    else if (canAfford) {
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

    // อัปเดตปุ่ม RECRUIT (เฉพาะ Barracks)
    if (i == 1) {
        BuildingCost rc = b.getRecruitCost();
        bool hasBarracks = b.count > 0;
        bool canRecruit = hasBarracks &&
            city->getGold() >= rc.gold &&
            city->getFood() >= rc.food;

        if (!hasBarracks) {
            slots[i].btnRecruit.setFillColor(sf::Color(40, 40, 40));
            slots[i].btnRecruit.setOutlineColor(sf::Color(80, 80, 80));
            slots[i].btnRecruitText.setFillColor(sf::Color(100, 100, 100));
            slots[i].btnRecruitText.setString("No Barracks");
        }
        else if (canRecruit) {
            slots[i].btnRecruit.setFillColor(COL_RECRUIT);
            slots[i].btnRecruit.setOutlineColor(sf::Color(80, 140, 220));
            slots[i].btnRecruitText.setFillColor(sf::Color(150, 200, 255));
            slots[i].btnRecruitText.setString("RECRUIT");
        }
        else {
            slots[i].btnRecruit.setFillColor(sf::Color(40, 40, 60));
            slots[i].btnRecruit.setOutlineColor(sf::Color(80, 80, 120));
            slots[i].btnRecruitText.setFillColor(sf::Color(120, 120, 160));
            slots[i].btnRecruitText.setString("Need 20G 50F");
        }
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
    pendingBuildIdx = -1;
}

void BuildMenu::handleEvent(const sf::Event& event) {
    if (!city) return;
    if (event.type != sf::Event::MouseButtonPressed ||
        event.mouseButton.button != sf::Mouse::Left) return;

    sf::Vector2f mp((float)event.mouseButton.x, (float)event.mouseButton.y);
    recruitedThisFrame = false;

    // Right-click ยกเลิก placement mode
    if (event.mouseButton.button == sf::Mouse::Right) {
        if (pendingBuildIdx != -1) {
            cancelTileSelect();
        }
        return;
    }

    if (event.mouseButton.button != sf::Mouse::Left) return;

    // ถ้ากำลัง placement mode และคลิกนอก panel → ให้ main.cpp จัดการ 
    sf::FloatRect panelBounds(panelX, panelY, panelW, panelH);
    if (pendingBuildIdx != -1 && !panelBounds.contains(mp)) {
        return; // main.cpp จะรับ event นี้แทน
    }

    if (btnClose.getGlobalBounds().contains(mp)) {
        pendingBuildIdx = -1;
        clear();
        return;
    }

    for (int i = 0; i < 4; i++) {
        if (slots[i].btnBuild.getGlobalBounds().contains(mp)) {
            if (pendingBuildIdx == i) {
                // กดซ้ำ → ยกเลิก placement
                cancelTileSelect();
                return;
            }
            if (city->canBuild(i)) {
                // เข้า placement mode — รอผู้ใช้คลิกแผนที่
                pendingBuildIdx = i;
                feedbackText.setString("Click on map to place!");
                feedbackText.setFillColor(sf::Color(100, 220, 255));
                feedbackTimer = 300;
            }
            else {
                feedbackText.setString("Not enough resources!");
                feedbackText.setFillColor(sf::Color(255, 80, 80));
                feedbackTimer = 120;
            }
            return;
        }

        if (i == 1 && slots[i].btnRecruit.getGlobalBounds().contains(mp)) {
            const Building& b = city->getBuilding(1);
            BuildingCost rc = b.getRecruitCost();
            if (b.count > 0 &&
                city->getGold() >= rc.gold &&
                city->getFood() >= rc.food)
            {
                city->addGold(-rc.gold);
                city->addFood(-rc.food);
                if (units) units->emplace_back("Swordsman", city->getR(), city->getC(), 1);
                recruitedThisFrame = true;
                feedbackText.setString("Swordsman recruited!");
                feedbackText.setFillColor(sf::Color(100, 180, 255));
                feedbackTimer = 120;
            }
            else {
                feedbackText.setString("Need Barracks + 20G 50F!");
                feedbackText.setFillColor(sf::Color(255, 80, 80));
                feedbackTimer = 120;
            }
            return;
        }
    }
}

void BuildMenu::confirmTileSelect(bool success) {
    if (success) {
        const Building& b = city->getBuilding(pendingBuildIdx);
        feedbackText.setString("Built: " + b.getName() + "!");
        feedbackText.setFillColor(sf::Color(100, 255, 100));
        lastBuiltIdx = pendingBuildIdx;
        feedbackTimer = 180;
    }
    else {
        feedbackText.setString("Cannot build here!");
        feedbackText.setFillColor(sf::Color(255, 80, 80));
        feedbackTimer = 120;
    }
    pendingBuildIdx = -1;
}

void BuildMenu::cancelTileSelect() {
    pendingBuildIdx = -1;
    feedbackText.setString("Placement cancelled.");
    feedbackText.setFillColor(sf::Color(200, 200, 100));
    feedbackTimer = 90;
}

void BuildMenu::draw(sf::RenderWindow& window) {
    if (!city) return;

    float x = panelX, y = panelY, pw = panelW, ph = panelH;
    float rad = 10.f;
    sf::Color cRim = COL_RIM;

    sf::RectangleShape bgFull({ pw, ph });
    bgFull.setPosition(x, y);
    bgFull.setFillColor(COL_BG);
    window.draw(bgFull);

    sf::RectangleShape tb({ pw, 54.f });
    tb.setPosition(x, y);
    tb.setFillColor(COL_TOPBAR);
    window.draw(tb);

    sf::RectangleShape tbDiv({ pw - 20.f, 1.f });
    tbDiv.setPosition(x + 10.f, y + 54.f);
    tbDiv.setFillColor(sf::Color(160, 130, 45, 150));
    window.draw(tbDiv);

    // ขอบโค้งมนสีทอง
    sf::VertexArray ol(sf::LineStrip, 9);
    ol[0] = { {x + rad,      y},            cRim };
    ol[1] = { {x + pw - rad, y},            cRim };
    ol[2] = { {x + pw,       y + rad},      cRim };
    ol[3] = { {x + pw,       y + ph - rad}, cRim };
    ol[4] = { {x + pw - rad, y + ph},       cRim };
    ol[5] = { {x + rad,      y + ph},       cRim };
    ol[6] = { {x,            y + ph - rad}, cRim };
    ol[7] = { {x,            y + rad},      cRim };
    ol[8] = { {x + rad,      y},            cRim };
    window.draw(ol);

    labelTitle.setString(pendingBuildIdx != -1
        ? "SELECT LOCATION"
        : "BUILD MENU");
    window.draw(labelTitle);
    window.draw(btnClose);
    window.draw(btnCloseText);

    sf::Text stockText;
    stockText.setFont(font);
    stockText.setCharacterSize(15);
    stockText.setFillColor(sf::Color(180, 160, 80));
    stockText.setString(
        "G:" + std::to_string(city->getGold()) +
        "  W:" + std::to_string(city->getWood()) +
        "  F:" + std::to_string(city->getFood())
    );
    stockText.setPosition(panelX + 18, panelY + 62);
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

        if (i == 1) {
            window.draw(slots[i].btnRecruit);
            window.draw(slots[i].btnRecruitText);
        }
    }
    if (pendingBuildIdx != -1) {
        placementHintText.setString(
            "Click tile to place\n[Right-click] Cancel"
        );
        window.draw(placementHintText);
    }

    if (feedbackTimer > 0) {
        feedbackText.setPosition(panelX + 18, panelY + panelH - 40);
        window.draw(feedbackText);
        feedbackTimer--;
    }
}