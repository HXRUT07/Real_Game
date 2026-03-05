#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <vector>
#include <cmath>
#include <cstdlib> // <--- เพิ่มตัวนี้สำหรับ rand()
#include <ctime>   // <--- เพิ่มตัวนี้สำหรับ time()
#include <iostream> // <--- เพิ่มสำหรับ cout
#include <string>   // <--- เพิ่มสำหรับ string
#include <algorithm> 

#include "GameMap.h"     // <--- Game map system (Yu)
#include "MouseUI.h"     // <--- USER INTERFACE MOUSE (PLAY)
#include "GameCamera.h"  // <--- GAME CAMERA SYSTEM (Yu)
#include "Unit.h"        // <--- UNIT SYSTEM
#include "ResourceManage.h" // <--- เพิ่ม Header ของระบบทรัพยากร
#include "TurnManager.h" // <--- ระบบเทิร์น
#include "MainMenu.h" //<--- ระบบเมนูหลัก
#include "CityPanel.h"

// ฟังก์ชันหาระยะทาง (สำหรับ AI และระบบทั่วไป)
int getHexDistance(int r1, int c1, int r2, int c2) {
    int ac1 = c1 - (r1 - (r1 & 1)) / 2;
    int ac2 = c2 - (r2 - (r2 & 1)) / 2;
    return (std::abs(r1 - r2) + std::abs(ac1 - ac2) + std::abs((r1 - r2) + (ac1 - ac2))) / 2;
}

int main() {
    // ตั้งค่า Seed สำหรับการสุ่ม (ใส่ใน Main ทีเดียวจบ)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);

    // จำกัดเฟรมเรตหน่อย เครื่องจะได้ไม่ทำงานหนักเกินไปตอน Fullscreen
    window.setFramerateLimit(60);

    // โหลดฟอนต์สำหรับลูกเต๋า
    sf::Font combatFont;
    bool hasCombatFont = combatFont.loadFromFile("Roboto-VariableFont_wdth,wght.ttf");
    if (!hasCombatFont) hasCombatFont = combatFont.loadFromFile("arial.ttf");
    if (!hasCombatFont) hasCombatFont = combatFont.loadFromFile("assets/fonts/Trajan Pro Regular.ttf");

    // โหลดเสียง
    sf::SoundBuffer bufMove, bufDice, bufHit, bufClick;
    bufMove.loadFromFile("assets/sounds/move.wav");
    bufDice.loadFromFile("assets/sounds/dice.wav");
    bufHit.loadFromFile("assets/sounds/hit.wav");
    bufClick.loadFromFile("assets/sounds/click.wav");

    sf::Sound sndMove(bufMove);
    sf::Sound sndDice(bufDice); sndDice.setLoop(true);
    sf::Sound sndHit(bufHit);
    sf::Sound sndClick(bufClick);

    // ----Main Menu----//(PLAY)
    {
        MainMenu menu(window, "assets/background.png", "assets/fonts/Trajan Pro Regular.ttf");
        menu.loadVideoFrames("assets/frames", 240);  // <-- เพิ่มบรรทัดนี้
        sf::Clock menuClock;

        while (window.isOpen()) {
            float dt = menuClock.restart().asSeconds();

            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                menu.handleEvent(event);
            }

            menu.update(dt);

            if (menu.getState() == MenuState::Play)  break;
            if (menu.getState() == MenuState::Exit) { window.close(); break; }

            window.clear();
            menu.draw();
            window.display();
        }
    }

    //----Map system----//(Yu)
    // 1. สร้าง Map แค่บรรทัดเดียว!
    GameMap worldMap(50, 50);

    // 2. สร้าง Object กล้อง
    GameCamera camera(window.getSize().x, window.getSize().y);

    MouseUI gui; //(PLAY)  <--- ลบตัวที่ซ้ำออกให้แล้วครับ
    CityPanel cityPanel(window.getSize().x, window.getSize().y);

    //----Unit System----//
    std::vector<Unit> units;       // เก็บยูนิตทั้งหมด

    // จองพื้นที่ล่วงหน้า 1,000 ตัว ป้องกันบัค C++ ย้ายที่อยู่หน่วยความจำ (Memory Dangling)
    units.reserve(1000);

    Unit* selectedUnit = nullptr;
    bool isGameRunning = false;
    int unitNameCounter = 1;
    TurnManager turnSys(2);

    int currentTurnNumber = 1;

    std::vector<Unit*> currentStack;
    Unit* leadUnit = nullptr;
    City* activeCityUI = nullptr;

    sf::Clock aiTimer;
    int aiBaseR = 0, aiBaseC = 0;
    int aiGold = 0, aiWood = 0, aiFood = 0;
    int aiCityLevel = 1;

    bool isRollingDice = false;
    sf::Clock diceAnimTimer;
    int finalAtkRoll = 0, finalDefRoll = 0;
    int displayAtkRoll = 1, displayDefRoll = 1;
    int atkStartR = -1, atkStartC = -1;
    int defTargetR = -1, defTargetC = -1;
    int currentAttackerOwner = 1;
    bool isArmyAttack = true;

    auto rollDiceRisk = [](int numDice) {
        int highest = 0;
        for (int i = 0; i < numDice; i++) {
            int roll = (std::rand() % 6) + 1;
            if (roll > highest) highest = roll;
        }
        return highest;
        };

    while (window.isOpen()) {
        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

            cityPanel.handleEvent(event);

            // 3. ส่ง Event ให้กล้องจัดการ (คลิก/ปล่อย/หมุนล้อ)
            if (!isRollingDice) {
                camera.handleEvent(event, window);
            }

            if (event.type == sf::Event::MouseButtonPressed && turnSys.getCurrentPlayer() == 1 && !isRollingDice) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());
                sf::Vector2f uiPos = window.mapPixelToCoords(pixelPos, window.getDefaultView());

                if (gui.isSidePanelVisible() && uiPos.x > window.getSize().x - 220.f) {
                    if (gui.isModeButtonClicked(uiPos)) {
                        gui.toggleArmyMode();
                        sndClick.play();
                        continue;
                    }
                    int clickedIdx = gui.getClickedItemIndex(uiPos);
                    if (clickedIdx != -1) {
                        gui.setSelectedIndex(clickedIdx);
                        leadUnit = currentStack[clickedIdx];
                        worldMap.calculateValidMoves(leadUnit->getR(), leadUnit->getC(), leadUnit->getMoveRange());
                        sndClick.play();
                        continue;
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Right) {
                    gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                    activeCityUI = nullptr;

                    int r = 0, c = 0;
                    if (worldMap.getGridCoords(worldPos, r, c)) {
                        City* clickedCity = worldMap.getCityAt(r, c);
                        if (clickedCity != nullptr) {
                            // ถ้ามีเมือง ให้โชว์หน้าต่างคลังหลวงของเมือง (CITY STOCKPILE)
                            cityPanel.setCity(clickedCity);
                            gui.hideInfo();
                        }
                        else {
                            cityPanel.clear();
                            HexTile* clickedTile = worldMap.getTile(r, c);
                            if (clickedTile != nullptr && clickedTile->isExplored) {
                                // ดึงชื่อพื้นที่จาก TerrainType ใน GameMap
                                std::string tName = "TILE";
                                switch (clickedTile->type) {
                                case TerrainType::Grass:    tName = "GRASSLAND";  break;
                                case TerrainType::Forest:   tName = "FOREST";     break;
                                case TerrainType::Mountain: tName = "MOUNTAIN";   break;
                                case TerrainType::Water:    tName = "WATER";      break;
                                case TerrainType::City:     tName = "CITY";       break;
                                }
                                gui.showResourcePanel((float)window.getSize().x,
                                    clickedTile->gold, clickedTile->wood, clickedTile->food, tName);
                            }
                            else {
                                gui.hideInfo();
                            }
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Left) {

                    if (gui.isEndTurnButtonClicked(uiPos) && isGameRunning) {
                        sndClick.play();
                        turnSys.endTurn(units);
                        gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                        activeCityUI = nullptr;
                        aiTimer.restart();
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                        continue;
                    }

                    gui.hideInfo();
                    activeCityUI = nullptr;

                    if (!worldMap.isGameStarted()) {
                        worldMap.handleMouseClick(worldPos);

                        if (worldMap.isGameStarted()) {
                            isGameRunning = true;
                            int spawnR = 0, spawnC = 0;
                            if (worldMap.getGridCoords(worldPos, spawnR, spawnC)) {
                                sndMove.play();
                                units.emplace_back("Commander", spawnR, spawnC, 1);

                                // --- ยัด Starter Pack ให้เมืองหลวงทันที! ---
                                City* myCity = worldMap.getFirstCity();
                                if (myCity != nullptr) {
                                    ResourceYield starter = worldMap.getStarterPackValues();
                                    myCity->addGold(starter.gold);
                                    myCity->addWood(starter.wood);
                                    myCity->addFood(starter.food);
                                    std::cout << "Received Starter Pack: " << starter.gold << "G " << starter.wood << "W " << starter.food << "F!" << std::endl;
                                }

                                int enemyR = spawnR, enemyC = spawnC;
                                int attempts = 0;
                                while (attempts < 1000) {
                                    int r = 5 + std::rand() % 40; int c = 5 + std::rand() % 40;
                                    if (getHexDistance(spawnR, spawnC, r, c) >= 15 && getHexDistance(spawnR, spawnC, r, c) <= 30) {
                                        enemyR = r; enemyC = c; break;
                                    }
                                    attempts++;
                                }
                                if (attempts >= 1000) { enemyR = std::min(44, spawnR + 15); enemyC = std::min(44, spawnC + 15); }

                                aiBaseR = enemyR; aiBaseC = enemyC;
                                units.emplace_back("Enemy", enemyR, enemyC, 2);
                            }
                        }
                    }
                    else {
                        int r = 0, c = 0;
                        if (worldMap.getGridCoords(worldPos, r, c)) {

                            HexTile* clickedTile = worldMap.getTile(r, c);

                            if (clickedTile == nullptr || !clickedTile->isVisible) {
                                gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                            }
                            else {
                                std::vector<Unit*> stackInTile;
                                bool hasOurUnit = false;
                                for (auto& u : units) {
                                    if (u.getR() == r && u.getC() == c) {
                                        stackInTile.push_back(&u);
                                        if (u.getOwner() == turnSys.getCurrentPlayer()) hasOurUnit = true;
                                    }
                                }

                                if (leadUnit != nullptr && worldMap.isValidMove(r, c)) {
                                    auto enemyIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == r && u.getC() == c && u.getOwner() != turnSys.getCurrentPlayer(); });

                                    if (enemyIt != units.end()) {
                                        atkStartR = leadUnit->getR(); atkStartC = leadUnit->getC();
                                        defTargetR = r; defTargetC = c;
                                        currentAttackerOwner = 1;
                                        isArmyAttack = gui.isArmyMode();

                                        finalAtkRoll = rollDiceRisk(3);
                                        finalDefRoll = rollDiceRisk(2);

                                        isRollingDice = true;
                                        diceAnimTimer.restart();
                                        sndDice.play();

                                        gui.clearSelection(); worldMap.clearHighlight();
                                        leadUnit = nullptr; currentStack.clear();
                                    }
                                    else {
                                        if (gui.isArmyMode()) {
                                            for (auto* u : currentStack) {
                                                if (u->hasAP() && u->getOwner() == 1) {
                                                    u->moveTo(r, c); u->consumeAP(1);
                                                }
                                            }
                                        }
                                        else {
                                            leadUnit->moveTo(r, c); leadUnit->consumeAP(1);
                                        }

                                        // --- ระบบเดินแล้วดูดทรัพยากรเข้าเมือง ---
                                        HexTile* targetTile = worldMap.getTile(r, c);
                                        if (targetTile != nullptr) {
                                            if (targetTile->gold > 0 || targetTile->wood > 0 || targetTile->food > 0) {
                                                City* myCity = worldMap.getFirstCity();
                                                if (myCity) {
                                                    myCity->addGold(targetTile->gold);
                                                    myCity->addWood(targetTile->wood);
                                                    myCity->addFood(targetTile->food);
                                                }
                                                targetTile->gold = 0; targetTile->wood = 0; targetTile->food = 0;
                                            }
                                        }

                                        worldMap.revealFog(r, c, 1);
                                        sndMove.play();

                                        worldMap.clearHighlight(); gui.clearSelection();
                                        leadUnit = nullptr; currentStack.clear();
                                    }
                                }
                                else if (!stackInTile.empty() && hasOurUnit) {
                                    currentStack = stackInTile;
                                    gui.setSelectionList(currentStack);
                                    gui.setArmyMode(true);

                                    leadUnit = nullptr;
                                    for (auto* u : currentStack) {
                                        if (u->hasAP() && u->getOwner() == turnSys.getCurrentPlayer()) { leadUnit = u; break; }
                                    }
                                    if (leadUnit) worldMap.calculateValidMoves(leadUnit->getR(), leadUnit->getC(), leadUnit->getMoveRange());
                                    else worldMap.clearHighlight();
                                }
                                else {
                                    gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                                }
                            }
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed && !isRollingDice) {
                if (event.key.code == sf::Keyboard::R) {
                    for (auto& u : units) u.resetAP();
                    std::cout << "Next Turn: All AP Reset" << std::endl;
                }

                if (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Equal) {
                    City* myCity = worldMap.getFirstCity();
                    if (myCity != nullptr) {
                        units.emplace_back("Swordsman", myCity->getR(), myCity->getC(), 1);
                        sndMove.play();
                        std::cout << "[TEST] Spawned 1 Swordsman at Capital!\n";
                    }
                }

                if (activeCityUI != nullptr && turnSys.getCurrentPlayer() == 1) {
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                        if (activeCityUI->getGold() >= 20 && activeCityUI->getFood() >= 50) {
                            activeCityUI->addGold(-20);
                            activeCityUI->addFood(-50);
                            units.emplace_back("Swordsman", activeCityUI->getR(), activeCityUI->getC(), 1);
                            sndMove.play();
                            gui.showCityResourcePanel((float)window.getSize().x, activeCityUI->getGold(), activeCityUI->getWood(), activeCityUI->getFood());
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                        if (activeCityUI->getGold() >= 50 && activeCityUI->getWood() >= 50 && activeCityUI->getFood() >= 50) {
                            activeCityUI->addGold(-50); activeCityUI->addWood(-50); activeCityUI->addFood(-50);
                            units.emplace_back("Cavalry", activeCityUI->getR(), activeCityUI->getC(), 1);
                            sndMove.play();
                            gui.showCityResourcePanel((float)window.getSize().x, activeCityUI->getGold(), activeCityUI->getWood(), activeCityUI->getFood());
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyReleased && !isRollingDice) {
                if (event.key.code == sf::Keyboard::Return && isGameRunning) {
                    if (turnSys.getCurrentPlayer() == 1) {
                        sndClick.play();
                        turnSys.endTurn(units);

                        gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                        activeCityUI = nullptr;
                        aiTimer.restart();
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                    }
                }
            }
        } // <---  End of while(window.pollEvent)

        // --- Combat Resolution ---
        if (isRollingDice && diceAnimTimer.getElapsedTime().asSeconds() > 2.5f) {
            isRollingDice = false;
            sndDice.stop();
            sndHit.play();

            auto atkIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == atkStartR && u.getC() == atkStartC && u.getOwner() == currentAttackerOwner; });
            auto defIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == defTargetR && u.getC() == defTargetC && u.getOwner() != currentAttackerOwner; });

            if (atkIt != units.end() && defIt != units.end()) {
                if (finalAtkRoll > finalDefRoll) {
                    units.erase(defIt);

                    bool tileEmpty = true;
                    for (auto& u : units) {
                        if (u.getR() == defTargetR && u.getC() == defTargetC && u.getOwner() != currentAttackerOwner) {
                            tileEmpty = false; break;
                        }
                    }

                    if (tileEmpty) {
                        if (isArmyAttack) {
                            for (auto& u : units) {
                                if (u.getR() == atkStartR && u.getC() == atkStartC && u.getOwner() == currentAttackerOwner) {
                                    u.moveTo(defTargetR, defTargetC); u.consumeAP(1);
                                }
                            }
                        }
                        else {
                            auto singleAtk = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == atkStartR && u.getC() == atkStartC && u.getOwner() == currentAttackerOwner; });
                            if (singleAtk != units.end()) { singleAtk->moveTo(defTargetR, defTargetC); singleAtk->consumeAP(1); }
                        }
                        if (currentAttackerOwner == 1) worldMap.revealFog(defTargetR, defTargetC, 1);
                    }
                    else {
                        if (isArmyAttack) {
                            for (auto& u : units) {
                                if (u.getR() == atkStartR && u.getC() == atkStartC && u.getOwner() == currentAttackerOwner) u.consumeAP(1);
                            }
                        }
                        else {
                            auto singleAtk = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == atkStartR && u.getC() == atkStartC && u.getOwner() == currentAttackerOwner; });
                            if (singleAtk != units.end()) singleAtk->consumeAP(1);
                        }
                    }
                }
                else {
                    units.erase(atkIt);
                }
            }
        }

        // --- AI Turn ---
        if (isGameRunning && turnSys.getCurrentPlayer() == 2 && !isRollingDice) {
            if (aiTimer.getElapsedTime().asSeconds() > 0.5f) {
                bool aiMovedThisTick = false;
                int evenDir[6][2] = { {-1,-1}, {-1,0}, {0,-1}, {0,1}, {1,-1}, {1,0} };
                int oddDir[6][2] = { {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,0}, {1,1} };

                if (aiGold >= 100 && aiWood >= 50 && aiFood >= 50) {
                    aiGold -= 100; aiWood -= 50; aiFood -= 50; aiCityLevel++;
                    units.emplace_back("Enemy_Lv" + std::to_string(aiCityLevel), aiBaseR, aiBaseC, 2);
                }

                for (size_t i = 0; i < units.size(); ++i) {
                    if (units[i].getOwner() == 2 && units[i].getCurrentAP() > 0) {
                        HexTile* currentTile = worldMap.getTile(units[i].getR(), units[i].getC());
                        if (currentTile && (currentTile->gold > 0 || currentTile->wood > 0 || currentTile->food > 0)) {
                            aiGold += currentTile->gold; aiWood += currentTile->wood; aiFood += currentTile->food;
                            currentTile->gold = 0; currentTile->wood = 0; currentTile->food = 0;
                        }

                        int targetR = -1, targetC = -1;
                        int minDist = 999999;
                        bool enemyInSight = false;

                        for (auto& enemy : units) {
                            if (enemy.getOwner() == 1) {
                                int dist = getHexDistance(units[i].getR(), units[i].getC(), enemy.getR(), enemy.getC());
                                if (dist <= 5) {
                                    if (dist < minDist) { minDist = dist; targetR = enemy.getR(); targetC = enemy.getC(); enemyInSight = true; }
                                }
                            }
                        }

                        if (!enemyInSight) {
                            minDist = 999999;
                            for (int r = 0; r < 50; r++) {
                                for (int c = 0; c < 50; c++) {
                                    HexTile* t = worldMap.getTile(r, c);
                                    if (t && (t->gold > 0 || t->wood > 0 || t->food > 0)) {
                                        int dist = getHexDistance(units[i].getR(), units[i].getC(), r, c);
                                        if (dist < minDist) { minDist = dist; targetR = r; targetC = c; }
                                    }
                                }
                            }
                        }

                        if (targetR == -1) {
                            targetR = std::max(0, std::min(49, aiBaseR + (std::rand() % 11 - 5)));
                            targetC = std::max(0, std::min(49, aiBaseC + (std::rand() % 11 - 5)));
                        }

                        int bestR = units[i].getR(), bestC = units[i].getC();
                        int bestDist = getHexDistance(units[i].getR(), units[i].getC(), targetR, targetC);

                        for (int dir = 0; dir < 6; ++dir) {
                            int nr = units[i].getR() + (units[i].getR() % 2 == 0 ? evenDir[dir][0] : oddDir[dir][0]);
                            int nc = units[i].getC() + (units[i].getR() % 2 == 0 ? evenDir[dir][1] : oddDir[dir][1]);
                            HexTile* nTile = worldMap.getTile(nr, nc);
                            if (nTile != nullptr) {
                                int d = getHexDistance(nr, nc, targetR, targetC);
                                if (d < bestDist) { bestDist = d; bestR = nr; bestC = nc; }
                            }
                        }

                        if (bestR != units[i].getR() || bestC != units[i].getC()) {
                            auto playerIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == bestR && u.getC() == bestC && u.getOwner() == 1; });
                            if (playerIt != units.end()) {
                                atkStartR = units[i].getR(); atkStartC = units[i].getC();
                                defTargetR = bestR; defTargetC = bestC;
                                currentAttackerOwner = 2;
                                isArmyAttack = true;

                                finalAtkRoll = rollDiceRisk(3);
                                finalDefRoll = rollDiceRisk(2);

                                isRollingDice = true;
                                diceAnimTimer.restart();
                                sndDice.play();

                                worldMap.clearHighlight(); gui.clearSelection(); leadUnit = nullptr; currentStack.clear();
                            }
                            else {
                                units[i].moveTo(bestR, bestC);
                                units[i].consumeAP(1);
                                sndMove.play();
                            }
                        }
                        else {
                            units[i].consumeAP(units[i].getCurrentAP());
                        }

                        aiMovedThisTick = true;
                        break;
                    }
                }

                if (!aiMovedThisTick) {
                    turnSys.endTurn(units);
                    currentTurnNumber++;
                    std::cout << ">>> Switched to Player 1 <<<" << std::endl;
                }
                aiTimer.restart();
            }
        }

        // --- Rendering ---
        if (!isRollingDice) camera.update(window);

        window.setView(camera.getView());

        if (!isRollingDice) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
            worldMap.updateHighlight(mousePos);
        }

        window.clear(sf::Color(20, 20, 30));
        window.setView(camera.getView());
        worldMap.draw(window);

        if (leadUnit != nullptr && turnSys.getCurrentPlayer() == 1) {
            for (auto& u : units) {
                if (u.getOwner() == 2 && worldMap.isValidMove(u.getR(), u.getC())) {
                    HexTile* enemyTile = worldMap.getTile(u.getR(), u.getC());
                    if (enemyTile) {
                        sf::ConvexShape redHex = enemyTile->shape;
                        redHex.setFillColor(sf::Color(255, 0, 0, 150));
                        redHex.setOutlineColor(sf::Color::Red);
                        redHex.setOutlineThickness(3.0f);
                        window.draw(redHex);
                    }
                }
            }
        }

        for (auto& unit : units) {
            if (unit.getOwner() == 1) {
                unit.draw(window);
            }
            else {
                HexTile* tile = worldMap.getTile(unit.getR(), unit.getC());
                if (tile != nullptr && tile->isVisible) {
                    unit.draw(window);
                }
            }
        }
        worldMap.drawCities(window);

        gui.updateTurnInfo(turnSys.getCurrentPlayer(), currentTurnNumber);

        City* myCity = worldMap.getFirstCity();
        if (myCity) gui.updateResourceBar(myCity->getWood(), myCity->getGold(), myCity->getFood());

        window.setView(window.getDefaultView());

        if (worldMap.isGameStarted()) {
            gui.draw(window);
        }
        cityPanel.draw(window);

        if (isRollingDice && hasCombatFont) {
            float elapsed = diceAnimTimer.getElapsedTime().asSeconds();
            sf::RectangleShape darkOverlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
            darkOverlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(darkOverlay);

            if (elapsed < 1.5f) {
                displayAtkRoll = (std::rand() % 6) + 1;
                displayDefRoll = (std::rand() % 6) + 1;
            }
            else {
                displayAtkRoll = finalAtkRoll;
                displayDefRoll = finalDefRoll;
            }

            std::string titleStr = "COMBAT PHASE";
            std::string atkStr = (currentAttackerOwner == 1 ? "PLAYER (Attacker)\nRolls 3 Dice" : "AI (Attacker)\nRolls 3 Dice");
            std::string defStr = (currentAttackerOwner == 1 ? "AI (Defender)\nRolls 2 Dice" : "PLAYER (Defender)\nRolls 2 Dice");

            sf::Text titleText(titleStr, combatFont, 50); titleText.setFillColor(sf::Color::Yellow); titleText.setPosition(window.getSize().x / 2.0f - 200.f, 100.f);
            sf::Text atkSideText(atkStr, combatFont, 30); atkSideText.setFillColor(sf::Color(255, 100, 100)); atkSideText.setPosition(window.getSize().x / 4.0f - 100.f, 300.f);
            sf::Text atkRollText(std::to_string(displayAtkRoll), combatFont, 100); atkRollText.setFillColor(sf::Color::White); atkRollText.setPosition(window.getSize().x / 4.0f, 400.f);
            sf::Text vsText("VS", combatFont, 40); vsText.setFillColor(sf::Color::White); vsText.setPosition(window.getSize().x / 2.0f - 25.f, 420.f);
            sf::Text defSideText(defStr, combatFont, 30); defSideText.setFillColor(sf::Color(100, 150, 255)); defSideText.setPosition(window.getSize().x * 3.0f / 4.0f - 100.f, 300.f);
            sf::Text defRollText(std::to_string(displayDefRoll), combatFont, 100); defRollText.setFillColor(sf::Color::White); defRollText.setPosition(window.getSize().x * 3.0f / 4.0f, 400.f);

            window.draw(titleText); window.draw(atkSideText); window.draw(atkRollText);
            window.draw(vsText); window.draw(defSideText); window.draw(defRollText);

            if (elapsed >= 1.5f) {
                sf::Text resultText("", combatFont, 60);
                if (finalAtkRoll > finalDefRoll) {
                    resultText.setString("ATTACKER WINS!"); resultText.setFillColor(sf::Color::Green);
                }
                else {
                    resultText.setString("DEFENDER WINS!"); resultText.setFillColor(sf::Color::Red);
                }
                resultText.setPosition(window.getSize().x / 2.0f - 220.f, 600.f);
                window.draw(resultText);
            }
        }

        window.display();
    } // <---  วงเล็บปิดของ while(window.isOpen())

    return 0; // <---  ปิด main() ตรงนี้ถูกแล้ว!
}

#if 0
//panel
GameMap gameMap(10, 10);
sf::Font font;
font.loadFromFile("arial.ttf");
sf::RectangleShape panel;
panel.setSize(sf::Vector2f(300, 600));
panel.setFillColor(sf::Color(40, 40, 40));
panel.setPosition(850, 50);
sf::Text panelText;
panelText.setFont(font);
panelText.setCharacterSize(18);
panelText.setFillColor(sf::Color::White);
panelText.setPosition(870, 70);
while (window.isOpen())
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            gameMap.handleMouseClick(mousePos);
        }
    }
    window.clear();
    gameMap.draw(window);
    City* city = gameMap.getSelectedCity();
    if (city != nullptr) {
        window.draw(panel);
        panelText.setString(city->getCityInfo());
        window.draw(panelText);
    }
    window.display();
}
#endif