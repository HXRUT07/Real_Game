#include "CityPanel.h"
#include <sstream>

CityPanel::CityPanel(float winW, float winH)
    : winWidth(winW), winHeight(winH)
{
    panelW = 320.f;
    panelH = winH;
    panelX = winW - panelW;
    panelY = 0.f;

    if (!font.loadFromFile("assets/fonts/Trajan Pro Regular.ttf"))
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    // Background
    background.setSize(sf::Vector2f(panelW, panelH));
    background.setPosition(panelX, panelY);
    background.setFillColor(sf::Color(15, 12, 10, 245));

    // Top Bar
    topBar.setSize(sf::Vector2f(panelW, 56.f));
    topBar.setPosition(panelX, panelY);
    topBar.setFillColor(sf::Color(70, 50, 15));

    labelTopBar.setFont(font);
    labelTopBar.setCharacterSize(14);
    labelTopBar.setFillColor(sf::Color(220, 180, 80));
    labelTopBar.setString("CITY INFO");
    labelTopBar.setPosition(panelX + 20, panelY + 18);

    // ชื่อเมือง
    labelCityName.setFont(font);
    labelCityName.setCharacterSize(11);
    labelCityName.setFillColor(sf::Color(160, 130, 60));
    labelCityName.setString("CITY NAME");
    labelCityName.setPosition(panelX + 20, panelY + 66);

    textCityName.setFont(font);
    textCityName.setCharacterSize(21);
    textCityName.setFillColor(sf::Color(255, 225, 130));
    textCityName.setPosition(panelX + 20, panelY + 82);

    // Rename
    labelRename.setFont(font);
    labelRename.setCharacterSize(11);
    labelRename.setFillColor(sf::Color(160, 130, 60));
    labelRename.setString("RENAME");
    labelRename.setPosition(panelX + 20, panelY + 116);

    renameBox.setSize(sf::Vector2f(176.f, 28.f));
    renameBox.setPosition(panelX + 20, panelY + 132);
    renameBox.setFillColor(sf::Color(35, 28, 18));
    renameBox.setOutlineColor(sf::Color(100, 80, 30));
    renameBox.setOutlineThickness(1.f);

    renameInput.setFont(font);
    renameInput.setCharacterSize(13);
    renameInput.setFillColor(sf::Color::White);
    renameInput.setPosition(panelX + 26, panelY + 138);

    btnRename.setSize(sf::Vector2f(70.f, 28.f));
    btnRename.setPosition(panelX + 204, panelY + 132);
    btnRename.setFillColor(sf::Color(90, 70, 20));
    btnRename.setOutlineColor(sf::Color(180, 140, 50));
    btnRename.setOutlineThickness(1.f);

    btnRenameText.setFont(font);
    btnRenameText.setCharacterSize(12);
    btnRenameText.setFillColor(sf::Color(255, 220, 100));
    btnRenameText.setString("OK");
    btnRenameText.setPosition(panelX + 229, panelY + 138);

    // Divider 1
    divider1.setSize(sf::Vector2f(260.f, 1.f));
    divider1.setPosition(panelX + 20, panelY + 172);
    divider1.setFillColor(sf::Color(80, 60, 20));

    // เลเวล
    labelLevel.setFont(font);
    labelLevel.setCharacterSize(11);
    labelLevel.setFillColor(sf::Color(160, 130, 60));
    labelLevel.setString("CITY LEVEL");
    labelLevel.setPosition(panelX + 20, panelY + 182);

    textLevel.setFont(font);
    textLevel.setCharacterSize(19);
    textLevel.setFillColor(sf::Color(255, 200, 80));
    textLevel.setPosition(panelX + 20, panelY + 198);

    levelBarBg.setSize(sf::Vector2f(260.f, 10.f));
    levelBarBg.setPosition(panelX + 20, panelY + 224);
    levelBarBg.setFillColor(sf::Color(50, 40, 20));
    levelBarBg.setOutlineColor(sf::Color(100, 80, 30));
    levelBarBg.setOutlineThickness(1.f);

    levelBarFill.setSize(sf::Vector2f(0.f, 10.f));
    levelBarFill.setPosition(panelX + 20, panelY + 224);
    levelBarFill.setFillColor(sf::Color(200, 160, 40));

    // Divider 2
    divider2.setSize(sf::Vector2f(260.f, 1.f));
    divider2.setPosition(panelX + 20, panelY + 244);
    divider2.setFillColor(sf::Color(80, 60, 20));

    // Resources (stockpile)
    labelResources.setFont(font);
    labelResources.setCharacterSize(11);
    labelResources.setFillColor(sf::Color(160, 130, 60));
    labelResources.setString("STOCKPILE");
    labelResources.setPosition(panelX + 20, panelY + 254);

    goldIcon.setSize(sf::Vector2f(11.f, 11.f));
    goldIcon.setFillColor(sf::Color(255, 200, 0));
    goldIcon.setPosition(panelX + 20, panelY + 274);
    textGold.setFont(font); textGold.setCharacterSize(14);
    textGold.setFillColor(sf::Color(255, 220, 100));
    textGold.setPosition(panelX + 36, panelY + 270);

    woodIcon.setSize(sf::Vector2f(11.f, 11.f));
    woodIcon.setFillColor(sf::Color(100, 200, 80));
    woodIcon.setPosition(panelX + 20, panelY + 294);
    textWood.setFont(font); textWood.setCharacterSize(14);
    textWood.setFillColor(sf::Color(150, 220, 120));
    textWood.setPosition(panelX + 36, panelY + 290);

    foodIcon.setSize(sf::Vector2f(11.f, 11.f));
    foodIcon.setFillColor(sf::Color(220, 100, 80));
    foodIcon.setPosition(panelX + 20, panelY + 314);
    textFood.setFont(font); textFood.setCharacterSize(14);
    textFood.setFillColor(sf::Color(220, 150, 120));
    textFood.setPosition(panelX + 36, panelY + 310);

    // Divider 3
    divider3.setSize(sf::Vector2f(260.f, 1.f));
    divider3.setPosition(panelX + 20, panelY + 340);
    divider3.setFillColor(sf::Color(80, 60, 20));

    // Upgrade Cost Label
    labelUpgradeCost.setFont(font);
    labelUpgradeCost.setCharacterSize(11);
    labelUpgradeCost.setFillColor(sf::Color(160, 130, 60));
    labelUpgradeCost.setString("UPGRADE COST");
    labelUpgradeCost.setPosition(panelX + 20, panelY + 350);

    // cost: Gold (แถวบน ซ้าย)
    costGoldIcon.setSize(sf::Vector2f(10.f, 10.f));
    costGoldIcon.setFillColor(sf::Color(255, 200, 0));
    costGoldIcon.setPosition(panelX + 20, panelY + 372);
    textCostGold.setFont(font); textCostGold.setCharacterSize(13);
    textCostGold.setPosition(panelX + 36, panelY + 368);

    // cost: Wood (แถวบน กลาง)
    costWoodIcon.setSize(sf::Vector2f(10.f, 10.f));
    costWoodIcon.setFillColor(sf::Color(100, 200, 80));
    costWoodIcon.setPosition(panelX + 110, panelY + 372);
    textCostWood.setFont(font); textCostWood.setCharacterSize(13);
    textCostWood.setPosition(panelX + 126, panelY + 368);

    // cost: Food (แถวบน ขวา)
    costFoodIcon.setSize(sf::Vector2f(10.f, 10.f));
    costFoodIcon.setFillColor(sf::Color(220, 100, 80));
    costFoodIcon.setPosition(panelX + 200, panelY + 372);
    textCostFood.setFont(font); textCostFood.setCharacterSize(13);
    textCostFood.setPosition(panelX + 216, panelY + 368);

    // Upgrade Button
    btnUpgrade.setSize(sf::Vector2f(260.f, 46.f));
    btnUpgrade.setPosition(panelX + 20, panelY + 396);
    btnUpgrade.setFillColor(sf::Color(50, 90, 30));
    btnUpgrade.setOutlineColor(sf::Color(100, 180, 60));
    btnUpgrade.setOutlineThickness(2.f);

    btnUpgradeText.setFont(font);
    btnUpgradeText.setCharacterSize(15);
    btnUpgradeText.setFillColor(sf::Color(180, 255, 120));
    btnUpgradeText.setString("UPGRADE CITY");
    btnUpgradeText.setPosition(panelX + 58, panelY + 410);

    // ข้อความ "ทรัพยากรไม่พอ"
    textNotEnough.setFont(font);
    textNotEnough.setCharacterSize(12);
    textNotEnough.setFillColor(sf::Color(255, 80, 80));
    textNotEnough.setString("! Not enough resources");
    textNotEnough.setPosition(panelX + 20, panelY + 450);

    //Divider 4
    divider4.setSize(sf::Vector2f(260.f, 1.f));
    divider4.setPosition(panelX + 20, panelY + 468);
    divider4.setFillColor(sf::Color(80, 60, 20));

    //Buildings section
    labelBuildings.setFont(font);
    labelBuildings.setCharacterSize(11);
    labelBuildings.setFillColor(sf::Color(160, 130, 60));
    labelBuildings.setString("BUILDINGS");
    labelBuildings.setPosition(panelX + 20, panelY + 478);

    textBuildingSlots.setFont(font);
    textBuildingSlots.setCharacterSize(13);
    textBuildingSlots.setFillColor(sf::Color(200, 180, 100));
    textBuildingSlots.setPosition(panelX + 160, panelY + 476);

    // Village, Barracks, Restaurant, Lumbermill
    static const sf::Color ROW_COLORS[4] = {
        sf::Color(255, 220, 100),   // Village — ทอง
        sf::Color(200, 120, 100),   // Barracks — แดง
        sf::Color(220, 150, 120),   // Restaurant — ส้ม
        sf::Color(150, 220, 120),   // Lumbermill — เขียว
    };
    for (int i = 0; i < 4; i++) {
        textBuildingRows[i].setFont(font);
        textBuildingRows[i].setCharacterSize(13);
        textBuildingRows[i].setFillColor(ROW_COLORS[i]);
        textBuildingRows[i].setPosition(panelX + 20, panelY + 498 + i * 20.f);
    }

    textBuildingLimit.setFont(font);
    textBuildingLimit.setCharacterSize(11);
    textBuildingLimit.setFillColor(sf::Color(130, 110, 60));
    textBuildingLimit.setPosition(panelX + 20, panelY + 582);

    // Close Button
    btnClose.setSize(sf::Vector2f(26.f, 26.f));
    btnClose.setPosition(panelX + panelW - 36, panelY + 15);
    btnClose.setFillColor(sf::Color(110, 30, 30));
    btnClose.setOutlineColor(sf::Color(200, 60, 60));
    btnClose.setOutlineThickness(1.f);

    btnCloseText.setFont(font);
    btnCloseText.setCharacterSize(15);
    btnCloseText.setFillColor(sf::Color(255, 200, 200));
    btnCloseText.setString("X");
    btnCloseText.setPosition(panelX + panelW - 28, panelY + 17);
}

void CityPanel::setCity(City* c) {
    city = c;
    renameBuffer = "";
    isRenaming = false;
    showNotEnough = false;
    // รีเซ็ตปุ่ม
    btnUpgrade.setFillColor(sf::Color(50, 90, 30));
    btnUpgrade.setOutlineColor(sf::Color(100, 180, 60));
    btnUpgradeText.setFillColor(sf::Color(180, 255, 120));
    btnUpgradeText.setString("UPGRADE CITY");
}

void CityPanel::clear() {
    city = nullptr;
    renameBuffer = "";
    isRenaming = false;
    showNotEnough = false;
}

void CityPanel::handleEvent(const sf::Event& event) {
    if (!city) return;

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp((float)event.mouseButton.x, (float)event.mouseButton.y);

        if (btnClose.getGlobalBounds().contains(mp)) { clear(); return; }

        if (renameBox.getGlobalBounds().contains(mp)) {
            isRenaming = true;
            renameBox.setOutlineColor(sf::Color(220, 180, 80));
            return;
        }
        if (btnRename.getGlobalBounds().contains(mp)) {
            if (!renameBuffer.empty()) { city->setName(renameBuffer); renameBuffer = ""; }
            isRenaming = false;
            renameBox.setOutlineColor(sf::Color(100, 80, 30));
            return;
        }

        if (btnUpgrade.getGlobalBounds().contains(mp)) {
            if (!city->isMaxLevel()) {
                if (city->canUpgrade()) {
                    city->upgrade(); // หักทรัพยากรและขึ้น level
                    showNotEnough = false;
                    if (city->isMaxLevel()) {
                        btnUpgradeText.setString("MAX LEVEL");
                        btnUpgrade.setFillColor(sf::Color(60, 40, 100));
                        btnUpgrade.setOutlineColor(sf::Color(120, 80, 200));
                        btnUpgradeText.setFillColor(sf::Color(200, 180, 255));
                    }
                }
                else {
                    showNotEnough = true; // ทรัพยากรไม่พอ
                }
            }
            return;
        }

        isRenaming = false;
        renameBox.setOutlineColor(sf::Color(100, 80, 30));
    }

    if (isRenaming && event.type == sf::Event::TextEntered) {
        uint32_t ch = event.text.unicode;
        if (ch == 8) { if (!renameBuffer.empty()) renameBuffer.pop_back(); }
        else if (ch == 13) {
            if (!renameBuffer.empty()) { city->setName(renameBuffer); renameBuffer = ""; }
            isRenaming = false;
            renameBox.setOutlineColor(sf::Color(100, 80, 30));
        }
        else if (ch >= 32 && ch < 128 && renameBuffer.size() < 20) {
            renameBuffer += static_cast<char>(ch);
        }
    }
}

void CityPanel::draw(sf::RenderWindow& window) {
    if (!city) return;

    sf::View prevView = window.getView();
    window.setView(window.getDefaultView());

    window.draw(background);
    window.draw(topBar);
    window.draw(labelTopBar);

    // ชื่อเมือง
    window.draw(labelCityName);
    textCityName.setString(city->getName());
    window.draw(textCityName);

    // Rename
    window.draw(labelRename);
    window.draw(renameBox);
    renameInput.setString(renameBuffer + (isRenaming ? "|" : ""));
    window.draw(renameInput);
    window.draw(btnRename);
    window.draw(btnRenameText);

    window.draw(divider1);

    // เลเวล
    window.draw(labelLevel);
    int lv = city->getLevel();
    int maxLv = city->getMaxLevel();
    textLevel.setString("Level " + std::to_string(lv) + " / " + std::to_string(maxLv));
    window.draw(textLevel);
    window.draw(levelBarBg);
    levelBarFill.setSize(sf::Vector2f(260.f * ((float)lv / maxLv), 10.f));
    window.draw(levelBarFill);

    window.draw(divider2);

    // Stockpile
    window.draw(labelResources);
    window.draw(goldIcon);
    textGold.setString("Gold: " + std::to_string(city->getGold()));
    window.draw(textGold);
    window.draw(woodIcon);
    textWood.setString("Wood: " + std::to_string(city->getWood()));
    window.draw(textWood);
    window.draw(foodIcon);
    textFood.setString("Food: " + std::to_string(city->getFood()));
    window.draw(textFood);

    window.draw(divider3);

    // Upgrade cost / MAX LEVEL
    if (!city->isMaxLevel()) {
        auto cost = city->getUpgradeCost();
        window.draw(labelUpgradeCost);

        // เปลี่ยนสีตัวเลขถ้าทรัพยากรไม่พอ
        textCostGold.setFillColor(city->getGold() >= cost.gold ? sf::Color(255, 220, 100) : sf::Color(255, 80, 80));
        textCostWood.setFillColor(city->getWood() >= cost.wood ? sf::Color(150, 220, 120) : sf::Color(255, 80, 80));
        textCostFood.setFillColor(city->getFood() >= cost.food ? sf::Color(220, 150, 120) : sf::Color(255, 80, 80));

        window.draw(costGoldIcon);
        textCostGold.setString(std::to_string(cost.gold));
        window.draw(textCostGold);

        window.draw(costWoodIcon);
        textCostWood.setString(std::to_string(cost.wood));
        window.draw(textCostWood);

        window.draw(costFoodIcon);
        textCostFood.setString(std::to_string(cost.food));
        window.draw(textCostFood);

        // ปุ่ม upgrade เปลี่ยนสีถ้าไม่พอ
        if (!city->canUpgrade()) {
            btnUpgrade.setFillColor(sf::Color(80, 40, 30));
            btnUpgrade.setOutlineColor(sf::Color(160, 80, 60));
            btnUpgradeText.setFillColor(sf::Color(200, 120, 100));
            btnUpgradeText.setString("UPGRADE CITY");
        }
        else if (!city->isMaxLevel()) {
            btnUpgrade.setFillColor(sf::Color(50, 90, 30));
            btnUpgrade.setOutlineColor(sf::Color(100, 180, 60));
            btnUpgradeText.setFillColor(sf::Color(180, 255, 120));
            btnUpgradeText.setString("UPGRADE CITY");
        }

        window.draw(btnUpgrade);
        window.draw(btnUpgradeText);

        if (showNotEnough) window.draw(textNotEnough);
    }
    else {
        // MAX LEVEL
        btnUpgrade.setFillColor(sf::Color(60, 40, 100));
        btnUpgrade.setOutlineColor(sf::Color(120, 80, 200));
        btnUpgradeText.setFillColor(sf::Color(200, 180, 255));
        btnUpgradeText.setString("MAX LEVEL");
        window.draw(btnUpgrade);
        window.draw(btnUpgradeText);
    }

    window.draw(divider4);
    window.draw(labelBuildings);

    int used = city->getTotalBuildingCount();
    int maxB = city->getMaxTotalBuildings();
    int maxPer = city->getMaxPerBuilding();

    textBuildingSlots.setString(std::to_string(used) + " / " + std::to_string(maxB) + " used");
    textBuildingSlots.setFillColor(used >= maxB ? sf::Color(255, 80, 80) : sf::Color(180, 160, 80));
    window.draw(textBuildingSlots);

    // แต่ละ building type
    static const char* names[4] = { "Village", "Barracks", "Restaurant", "Lumbermill" };
    for (int i = 0; i < 4; i++) {
        int cnt = city->getBuilding(i).count;
        std::string row = std::string(names[i]) + ": " + std::to_string(cnt) + " / " + std::to_string(maxPer);
        if (cnt >= maxPer || used >= maxB) {
            textBuildingRows[i].setFillColor(sf::Color(100, 80, 60));
        }
        else {
            static const sf::Color ROW_COLORS[4] = {
                sf::Color(255, 220, 100),
                sf::Color(200, 120, 100),
                sf::Color(220, 150, 120),
                sf::Color(150, 220, 120),
            };
            textBuildingRows[i].setFillColor(ROW_COLORS[i]);
        }
        textBuildingRows[i].setString(row);
        window.draw(textBuildingRows[i]);
    }

    textBuildingLimit.setString("* Upgrade city to unlock more slots");
    window.draw(textBuildingLimit);

    window.draw(btnClose);
    window.draw(btnCloseText);

    window.setView(prevView);
}
