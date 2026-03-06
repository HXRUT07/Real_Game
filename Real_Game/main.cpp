#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>   
#include <iostream> 
#include <string>   
#include <algorithm> 

#include "GameMap.h"     
#include "MouseUI.h"     
#include "GameCamera.h"  
#include "Unit.h"        
#include "ResourceManage.h" 
#include "TurnManager.h" 
#include "MainMenu.h"
#include "CityPanel.h"
#include "UpkeepManager.h"
#include "CombatManager.h"
#include "BuildMenu.h"
#include "AIManager.h"
#include "Cloud.h"

int getHexDistance(int r1, int c1, int r2, int c2) {
    int ac1 = c1 - (r1 - (r1 & 1)) / 2;
    int ac2 = c2 - (r2 - (r2 & 1)) / 2;
    return (std::abs(r1 - r2) + std::abs(ac1 - ac2) + std::abs((r1 - r2) + (ac1 - ac2))) / 2;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    sf::Font combatFont;
    bool hasCombatFont = combatFont.loadFromFile("Roboto-VariableFont_wdth,wght.ttf");
    if (!hasCombatFont) hasCombatFont = combatFont.loadFromFile("arial.ttf");
    if (!hasCombatFont) hasCombatFont = combatFont.loadFromFile("assets/fonts/Trajan Pro Regular.ttf");

    sf::SoundBuffer bufMove, bufDice, bufHit, bufClick;
    bufMove.loadFromFile("assets/sounds/move.wav");
    bufDice.loadFromFile("assets/sounds/dice.wav");
    bufHit.loadFromFile("assets/sounds/hit.wav");
    bufClick.loadFromFile("assets/sounds/click.wav");

    sf::Sound sndMove(bufMove);
    sf::Sound sndDice(bufDice); sndDice.setLoop(true);
    sf::Sound sndHit(bufHit);
    sf::Sound sndClick(bufClick);

    {
        MainMenu menu(window, "assets/background.png", "assets/fonts/Trajan Pro Regular.ttf");
        menu.loadVideoFrames("assets/frames", 240);
        sf::Clock menuClock;

        while (window.isOpen()) {
            float dt = menuClock.restart().asSeconds();
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
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

    GameMap worldMap(50, 50);
    GameCamera camera(window.getSize().x, window.getSize().y);
    MouseUI gui;
    CityPanel cityPanel(window.getSize().x, window.getSize().y);

    CombatManager combatSys;
    if (hasCombatFont) combatSys.setFont(combatFont);

    BuildMenu buildMenu(window.getSize().x, window.getSize().y);

    // เปรมทำ - ระบบก้อนเมฆ
    CloudSystem cloudSystem((float)window.getSize().x, (float)window.getSize().y, 6);
    sf::Clock cloudClock;
    // เปรมทำ - จบ

    std::vector<Unit> units;
    units.reserve(1000);

    buildMenu.setUnits(&units);

    Unit* selectedUnit = nullptr;  // ตัวที่กำลังเลือกอยู่
    bool isGameRunning = false;    // ตัวแปรเช็คว่าจบช่วงเลือกจุดเกิดหรือยัง
    int unitNameCounter = 1;       // ตัวนับสำหรับตั้งชื่อ Unit อัตโนมัติ
    TurnManager turnSys(2); // สร้างระบบเทิร์นสำหรับ 2 ผู้เล่น
    
    int currentTurnNumber = 1;

    std::vector<Unit*> currentStack;
    Unit* leadUnit = nullptr;
    City* activeCityUI = nullptr;

    AIManager aiManager;

    while (window.isOpen()) {
        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

            cityPanel.handleEvent(event);
            buildMenu.handleEvent(event);

            if (!combatSys.isCombatActive()) {
                camera.handleEvent(event, window);
            }

            if (event.type == sf::Event::MouseButtonPressed && turnSys.getCurrentPlayer() == 1 && !combatSys.isCombatActive()) {
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
                        worldMap.clearHighlight();
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
                            activeCityUI = clickedCity;
                            cityPanel.setCity(clickedCity);
                            gui.hideInfo();
                        }
                        else {
                            cityPanel.clear();
                            HexTile* clickedTile = worldMap.getTile(r, c);
                            if (clickedTile != nullptr && clickedTile->isExplored) {
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
                        UpkeepManager::processAIUpkeep(units, aiManager.getAIFood());
                        gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                        activeCityUI = nullptr;
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                        continue;
                    }

                    if (gui.isBuildingCityButtonClicked(uiPos) && isGameRunning) {
                        if (buildMenu.isOpen()) {
                            buildMenu.clear();
                        }
                        else {
                            City* myCity = worldMap.getFirstCity();
                            if (myCity) buildMenu.setCity(myCity);
                        }
                        sndClick.play();
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
                                units.emplace_back("Swordsman", spawnR, spawnC, 1);

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

                                aiManager.initBase(enemyR, enemyC);
                                units.emplace_back("Goblin", enemyR, enemyC, 2);
                            }
                        }
                    }
                    else {
                        int r = 0, c = 0;
                        if (worldMap.getGridCoords(worldPos, r, c)) {

                            if (gui.isBuildingCityMode()) {
                                HexTile* ft = worldMap.getTile(r, c);
                                if (ft && ft->isVisible && worldMap.getCityAt(r, c) == nullptr) {
                                    worldMap.foundCity(r, c);
                                    gui.setBuildingCityMode(false);
                                    std::cout << "New city built at " << r << "," << c << std::endl;
                                }
                                continue;
                            }

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
                                        combatSys.initiateCombat(leadUnit->getR(), leadUnit->getC(), r, c, 1, sndDice, gui.isArmyMode());
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
                                    worldMap.clearHighlight();
                                    currentStack = stackInTile;
                                    gui.setSelectionList(currentStack);
                                    gui.setArmyMode(true);
                                    gui.setSelectedIndex(0);

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

            if (event.type == sf::Event::KeyPressed && !combatSys.isCombatActive()) {
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

                if (event.key.code == sf::Keyboard::Z || event.key.code == sf::Keyboard::X || event.key.code == sf::Keyboard::C) {
                    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());
                    int r = 0, c = 0;
                    if (worldMap.getGridCoords(worldPos, r, c)) {
                        if (event.key.code == sf::Keyboard::Z) {
                            units.emplace_back("Swordsman", r, c, 1);
                            sndMove.play();
                        }
                        else if (event.key.code == sf::Keyboard::X) {
                            for (int i = 0; i < 5; i++) units.emplace_back("Swordsman", r, c, 1);
                            sndMove.play();
                        }
                        else if (event.key.code == sf::Keyboard::C) {
                            units.emplace_back("Goblin", r, c, 2);
                            sndMove.play();
                        }
                    }
                }

                if (activeCityUI != nullptr && turnSys.getCurrentPlayer() == 1) {
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                        if (activeCityUI->getGold() >= 20 && activeCityUI->getFood() >= 50) {
                            activeCityUI->addGold(-20);
                            activeCityUI->addFood(-50);
                            units.emplace_back("Swordsman", activeCityUI->getR(), activeCityUI->getC(), 1);
                            sndMove.play();
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                        if (activeCityUI->getGold() >= 100 && activeCityUI->getFood() >= 250) {
                            activeCityUI->addGold(-100);
                            activeCityUI->addFood(-250);
                            for (int i = 0; i < 5; i++) units.emplace_back("Swordsman", activeCityUI->getR(), activeCityUI->getC(), 1);
                            sndMove.play();
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyReleased && !combatSys.isCombatActive()) {
                if (event.key.code == sf::Keyboard::Return && isGameRunning) {
                    if (turnSys.getCurrentPlayer() == 1) {
                        sndClick.play();
                        turnSys.endTurn(units);
                        UpkeepManager::processAIUpkeep(units, aiManager.getAIFood());
                        gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                        activeCityUI = nullptr;
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                    }
                }
            }
        }

        if (isGameRunning && turnSys.getCurrentPlayer() == 2 && !combatSys.isCombatActive()) {
            bool aiEndedTurn = aiManager.processTurn(units, worldMap, turnSys, combatSys, sndMove, sndDice);
            if (aiEndedTurn) {
                currentTurnNumber++;
                std::cout << ">>> Switched to Player 1 <<<" << std::endl;
                UpkeepManager::processPlayerUpkeep(units, worldMap.getFirstCity());
            }
        }

        if (!combatSys.isCombatActive()) camera.update(window);
        window.setView(camera.getView());

        if (!combatSys.isCombatActive()) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
            worldMap.updateHighlight(mousePos);
        }

        if (isGameRunning) {
            worldMap.updateVision(units, 1);
        }

        // เปรมทำ - อัปเดตเมฆ
        float cloudDt = cloudClock.restart().asSeconds();
        cloudSystem.update(cloudDt);
        // เปรมทำ - จบ

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
            unit.draw(window);
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
        buildMenu.draw(window);

        if (buildMenu.didRecruit()) {
            sndMove.play();
            buildMenu.clearRecruit();
        }
       
        combatSys.updateAndDraw(window, units, worldMap, sndDice, sndHit);

        // เปรมทำ - วาดเมฆทับหน้าสุด
        window.setView(window.getDefaultView());
        cloudSystem.draw(window, camera.getZoomLevel());
        // เปรมทำ - จบ

        window.display();
    }

    return 0;
}