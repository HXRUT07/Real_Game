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
#include "CombatManager.h" 
#include "AIManager.h"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    // โหลดฟอนต์
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
    std::vector<Unit> units;
    units.reserve(1000);

    Unit* selectedUnit = nullptr;
    bool isGameRunning = false;
    int unitNameCounter = 1;
    TurnManager turnSys(2);

    int currentTurnNumber = 1;

    // ---  เรียกใช้ Class ที่เราแยกไปเขียน ---
    CombatManager combatSys;
    if (hasCombatFont) combatSys.setFont(combatFont);
    AIManager aiSys;

    while (window.isOpen()) {

        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

            // ล็อกกล้องตอนทอยเต๋า
            if (!combatSys.isCombatActive()) {
                camera.handleEvent(event, window);
            }

            // ระบบคลิกเมาส์
            if (event.type == sf::Event::MouseButtonPressed && turnSys.getCurrentPlayer() == 1 && !combatSys.isCombatActive()) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());
                sf::Vector2f uiPos = window.mapPixelToCoords(pixelPos, window.getDefaultView());

                if (event.mouseButton.button == sf::Mouse::Right) {
                    gui.clearSelection();
                    selectedUnit = nullptr;
                    worldMap.clearHighlight();

                    int r = 0, c = 0;
                    if (worldMap.getGridCoords(worldPos, r, c)) {
                        City* clickedCity = worldMap.getCityAt(r, c);
                        if (clickedCity != nullptr) {
                            gui.showCityResourcePanel((float)window.getSize().x, clickedCity->getGold(), clickedCity->getWood(), clickedCity->getFood());
                        }
                        else {
                            HexTile* clickedTile = worldMap.getTile(r, c);
                            if (clickedTile != nullptr && clickedTile->isExplored) {
                                gui.showResourcePanel((float)window.getSize().x, clickedTile->gold, clickedTile->wood, clickedTile->food);
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
                        gui.clearSelection();
                        selectedUnit = nullptr;
                        worldMap.clearHighlight();
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                        continue;
                    }

                    gui.hideInfo();

                    if (!isGameRunning) {
                        worldMap.handleMouseClick(worldPos);
                        if (worldMap.isGameStarted()) {
                            isGameRunning = true;
                            int spawnR = 0, spawnC = 0;
                            if (worldMap.getGridCoords(worldPos, spawnR, spawnC)) {
                                sndMove.play();
                                units.emplace_back("Commander", spawnR, spawnC, 1);

                                // สุ่มหาจุดเกิดให้ AI ให้ไกล 15-30 ช่อง
                                int enemyR = spawnR, enemyC = spawnC;
                                auto hexDist = [](int r1, int c1, int r2, int c2) {
                                    int ac1 = c1 - (r1 - (r1 & 1)) / 2; int ac2 = c2 - (r2 - (r2 & 1)) / 2;
                                    return (std::abs(r1 - r2) + std::abs(ac1 - ac2) + std::abs((r1 - r2) + (ac1 - ac2))) / 2;
                                    };

                                int attempts = 0;
                                while (attempts < 1000) {
                                    int r = 5 + std::rand() % 40; int c = 5 + std::rand() % 40;
                                    if (hexDist(spawnR, spawnC, r, c) >= 15 && hexDist(spawnR, spawnC, r, c) <= 30) {
                                        enemyR = r; enemyC = c; break;
                                    }
                                    attempts++;
                                }
                                if (attempts >= 1000) { enemyR = std::min(44, spawnR + 15); enemyC = std::min(44, spawnC + 15); }

                                aiSys.initBase(enemyR, enemyC); // ส่งตำแหน่งไปให้ AI Manager
                                units.emplace_back("Enemy", enemyR, enemyC, 2);
                            }
                        }
                    }
                    else {
                        int r = 0, c = 0;
                        if (worldMap.getGridCoords(worldPos, r, c)) {
                            HexTile* clickedTile = worldMap.getTile(r, c);

                            if (clickedTile == nullptr || !clickedTile->isVisible) {
                                gui.clearSelection(); selectedUnit = nullptr; worldMap.clearHighlight();
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

                                if (selectedUnit != nullptr && worldMap.isValidMove(r, c)) {
                                    auto enemyIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == r && u.getC() == c && u.getOwner() != turnSys.getCurrentPlayer(); });

                                    if (enemyIt != units.end()) {
                                        // เราสั่งโจมตี ส่งต่อให้ CombatManager จัดการ!
                                        combatSys.initiateCombat(selectedUnit->getR(), selectedUnit->getC(), r, c, 1, sndDice);
                                        gui.clearSelection();
                                        worldMap.clearHighlight();
                                        selectedUnit = nullptr;
                                    }
                                    else {
                                        selectedUnit->moveTo(r, c);
                                        selectedUnit->consumeAP(1);
                                        worldMap.revealFog(r, c, 1);
                                        sndMove.play();

                                        worldMap.clearHighlight();
                                        gui.clearSelection();
                                        selectedUnit = nullptr;
                                    }
                                }
                                else if (!stackInTile.empty() && hasOurUnit) {
                                    gui.setSelectionList(stackInTile);
                                    selectedUnit = nullptr;
                                    for (auto* u : stackInTile) {
                                        if (u->hasAP() && u->getOwner() == turnSys.getCurrentPlayer()) { selectedUnit = u; break; }
                                    }
                                    if (selectedUnit) worldMap.calculateValidMoves(selectedUnit->getR(), selectedUnit->getC(), selectedUnit->getMoveRange());
                                    else worldMap.clearHighlight();
                                }
                                else {
                                    gui.clearSelection(); selectedUnit = nullptr; worldMap.clearHighlight();
                                }
                            }
                        }
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed && !combatSys.isCombatActive()) {
                if (event.key.code == sf::Keyboard::R) {
                    for (auto& u : units) u.resetAP();
                }
            }

            if (event.type == sf::Event::KeyReleased && !combatSys.isCombatActive()) {
                if (event.key.code == sf::Keyboard::Return && isGameRunning) {
                    if (turnSys.getCurrentPlayer() == 1) {
                        sndClick.play();
                        turnSys.endTurn(units);
                        gui.clearSelection(); selectedUnit = nullptr; worldMap.clearHighlight();
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                    }
                }
            }
        }

        // -----------------------------------------------------------------------
        // อัปเดตสมอง AI (ถ้าจบเทิร์น AIManager จะคืนค่า True)
        // -----------------------------------------------------------------------
        if (isGameRunning && turnSys.getCurrentPlayer() == 2) {
            if (aiSys.processTurn(units, worldMap, turnSys, combatSys, sndMove, sndDice)) {
                currentTurnNumber++;
                std::cout << ">>> Switched to Player 1 <<<" << std::endl;
            }
        }

        if (!combatSys.isCombatActive()) camera.update(window);
        window.setView(camera.getView());

        if (!combatSys.isCombatActive()) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
            worldMap.updateHighlight(mousePos);
        }

        window.clear(sf::Color(20, 20, 30));
        window.setView(camera.getView());
        worldMap.draw(window);

        // ไฮไลท์เป้าหมายสีแดง
        if (selectedUnit != nullptr && turnSys.getCurrentPlayer() == 1) {
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

        for (auto& unit : units) unit.draw(window);
        worldMap.drawCities(window);

        window.setView(window.getDefaultView());
        gui.updateTurnInfo(turnSys.getCurrentPlayer(), currentTurnNumber);

        City* myCity = worldMap.getFirstCity();
        if (myCity) gui.updateResourceBar(myCity->getWood(), myCity->getGold(), myCity->getFood());
        gui.draw(window);

        // --- อัปเดตและวาดฉากลูกเต๋าของ CombatManager ทับล่างสุด ---
        combatSys.updateAndDraw(window, units, worldMap, sndDice, sndHit);

        window.display();
    }

    return 0;
}