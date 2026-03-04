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
    // ตั้งค่า Seed สำหรับการสุ่ม (ใส่ใน Main ทีเดียวจบ)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // กำหนดค่าการลบรอยหยัก (Antialiasing) เพื่อให้ขอบหกเหลี่ยมคมชัดขึ้น (PLAY)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Hexa-Conquest", sf::Style::Fullscreen, settings);

    // จำกัดเฟรมเรตหน่อย เครื่องจะได้ไม่ทำงานหนักเกินไปตอน Fullscreen
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
        menu.loadVideoFrames("assets/frames", 240); // <-- เพิ่มบรรทัดนี้
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

    //----Map system----//(Yu)
    // 1. สร้าง Map แค่บรรทัดเดียว!
    GameMap worldMap(50, 50);

    // 2. สร้าง Object กล้อง
    GameCamera camera(window.getSize().x, window.getSize().y);
    MouseUI gui; //(PLAY)

    //----Unit System----//
    std::vector<Unit> units;       // เก็บยูนิตทั้งหมด
    // จองพื้นที่ล่วงหน้า 1,000 ตัว ป้องกันบัค C++ ย้ายที่อยู่หน่วยความจำ (Memory Dangling)
    units.reserve(1000);

    Unit* selectedUnit = nullptr;
    bool isGameRunning = false; // ตัวแปรเช็คว่าจบช่วงเลือกจุดเกิดหรือยัง
    int unitNameCounter = 1; // ตัวนับสำหรับตั้งชื่อ Unit อัตโนมัติ
    TurnManager turnSys(2); // สร้างระบบเทิร์นสำหรับ 2 ผู้เล่น

    // --- ตัวแปรเก็บจำนวนเทิร์นที่ผ่านไป ---
    int currentTurnNumber = 1;

    // --- เรียกใช้ Class ที่เราแยกไปเขียน ---
    CombatManager combatSys;
    if (hasCombatFont) combatSys.setFont(combatFont);
    AIManager aiSys;

    City* activeCityUI = nullptr; // ตัวแปรจดจำเมืองที่เรากำลังเปิดดูอยู่ (เพื่อเอาไว้สร้างทหาร)

    while (window.isOpen()) {

        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        // ลูป pollEvent จะจบแค่ตรงที่เช็คปุ่ม/เมาส์ เท่านั้น
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // กด Escape เพื่อออกจากเกมได้ (สำคัญมากตอนเทส Fullscreen ไม่งั้นออกยาก)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

            // 3. ส่ง Event ให้กล้องจัดการ (คลิก/ปล่อย/หมุนล้อ)
            // ล็อกกล้องตอนทอยเต๋า
            if (!combatSys.isCombatActive()) {
                camera.handleEvent(event, window);
            }

            // -----------------------------------------------------------------------
            // ส่วนตรวจสอบการคลิกซ้าย: เลือกจุดเกิด (Spawn) และ ควบคุมยูนิต (Gameplay)
            // -----------------------------------------------------------------------
            // อนุญาตให้ผู้เล่นคลิกเมาส์สั่งการได้ เฉพาะตอนเป็นตาของ Player 1 เท่านั้น
            if (event.type == sf::Event::MouseButtonPressed && turnSys.getCurrentPlayer() == 1 && !combatSys.isCombatActive()) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, camera.getView());
                sf::Vector2f uiPos = window.mapPixelToCoords(pixelPos, window.getDefaultView()); // ตำแหน่งสำหรับ UI

                if (event.mouseButton.button == sf::Mouse::Right) {
                    // 1. ยกเลิกการเลือกทหารและไฮไลท์
                    gui.clearSelection();
                    selectedUnit = nullptr;
                    worldMap.clearHighlight();
                    activeCityUI = nullptr; // เคลียร์เมืองที่เลือกอยู่

                    // 2. ดึงข้อมูลทรัพยากรจากช่องที่คลิก
                    int r = 0, c = 0;
                    if (worldMap.getGridCoords(worldPos, r, c)) {
                        City* clickedCity = worldMap.getCityAt(r, c);
                        if (clickedCity != nullptr) {
                            activeCityUI = clickedCity; // จำไว้ว่าคลิกเมืองนี้อยู่
                            gui.showCityResourcePanel((float)window.getSize().x, clickedCity->getGold(), clickedCity->getWood(), clickedCity->getFood());
                        }
                        else {
                            HexTile* clickedTile = worldMap.getTile(r, c);
                            // กฎ: ต้องมีช่องนี้อยู่จริง และ "ต้องเคยสำรวจแล้ว (isExplored)" เท่านั้น
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

                    // --- เช็คก่อนเลยว่าคลิกโดนปุ่ม "END TURN" หรือเปล่า? ---
                    if (gui.isEndTurnButtonClicked(uiPos) && isGameRunning) {
                        sndClick.play();
                        turnSys.endTurn(units);
                        gui.clearSelection();
                        selectedUnit = nullptr;
                        worldMap.clearHighlight();
                        activeCityUI = nullptr; // ปิดหน้าต่างเมือง
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                        continue;
                    }

                    // คลิกซ้าย: ซ่อน Info Panel เดิมก่อน
                    gui.hideInfo();
                    activeCityUI = nullptr; // คลิกลงดินก็ปิดหน้าต่างเมืองซะ

                    // --- PHASE 1: เลือกจุดเกิด ---
                    if (!isGameRunning) {
                        // 3. ส่งให้ GameMap จัดการเลือกจุดเกิด
                        worldMap.handleMouseClick(worldPos);

                        // เช็คว่า Map เริ่มเกมสำเร็จหรือยัง?
                        if (worldMap.isGameStarted()) {
                            isGameRunning = true;

                            // *** Spawn ทหารตัวแรกตรงจุดที่คลิก ***
                            int spawnR = 0, spawnC = 0;
                            if (worldMap.getGridCoords(worldPos, spawnR, spawnC)) {
                                sndMove.play();
                                // ใส่เลข 1 ด้านหลังเพื่อให้ตัวนี้เป็นของ Player 1
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
                    // --- PHASE 2: ควบคุมทหาร (Gameplay) ---
                    else {
                        int r = 0, c = 0;
                        if (worldMap.getGridCoords(worldPos, r, c)) {
                            HexTile* clickedTile = worldMap.getTile(r, c);

                            // ถ้ายิงคลิกไปโดนหมอกดำ (มองไม่เห็น) ให้ตัดจบการทำงาน
                            if (clickedTile == nullptr || !clickedTile->isVisible) {
                                gui.clearSelection(); selectedUnit = nullptr; worldMap.clearHighlight();
                            }
                            else {
                                // 1. หา Unit ทั้งหมดที่อยู่ในช่องนี้ (Stacking)
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
                                        combatSys.initiateCombat(selectedUnit->getR(), selectedUnit->getC(), r, c, 1, sndDice, false);
                                        gui.clearSelection();
                                        worldMap.clearHighlight();
                                        selectedUnit = nullptr;
                                    }
                                    else {
                                        // 1. ย้ายตำแหน่ง
                                        selectedUnit->moveTo(r, c);
                                        // 2. หัก AP
                                        selectedUnit->consumeAP(1);
                                        // 3. เปิดหมอก (ระยะ 1 ช่อง)
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
                                        // กฎ: ต้องมี AP และ "ต้องเป็นของ Player ปัจจุบันเท่านั้น!"
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

            // =========================================================================
            // ระบบเกณฑ์ทหารผ่านคีย์บอร์ด (เมื่อหน้าต่างเมืองเปิดอยู่)
            // =========================================================================
            if (event.type == sf::Event::KeyPressed && !combatSys.isCombatActive()) {
                // R: รีเซ็ต AP ของทุก Unit (จำลองการจบ Turn)
                if (event.key.code == sf::Keyboard::R) {
                    for (auto& u : units) u.resetAP();
                }

                // ถ้าคลิกเปิดหน้าต่างเมืองค้างไว้อยู่...
                if (activeCityUI != nullptr && turnSys.getCurrentPlayer() == 1) {

                    // กดแป้น 1 สร้าง พลดาบ (Swordsman)
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                        if (activeCityUI->spendResources(20, 0, 50)) {
                            units.emplace_back("Swordsman", activeCityUI->getR(), activeCityUI->getC(), 1);
                            sndMove.play();
                            gui.showCityResourcePanel((float)window.getSize().x, activeCityUI->getGold(), activeCityUI->getWood(), activeCityUI->getFood());
                        }
                    }
                    // กดแป้น 2 สร้าง ทหารม้า (Cavalry)
                    else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                        if (activeCityUI->spendResources(50, 50, 50)) {
                            units.emplace_back("Cavalry", activeCityUI->getR(), activeCityUI->getC(), 1);
                            sndMove.play();
                            gui.showCityResourcePanel((float)window.getSize().x, activeCityUI->getGold(), activeCityUI->getWood(), activeCityUI->getFood());
                        }
                    }
                }
            }

            // ใช้ KeyReleased (ปล่อยนิ้ว) และแก้จาก Enter เป็น Return
            if (event.type == sf::Event::KeyReleased && !combatSys.isCombatActive()) {
                if (event.key.code == sf::Keyboard::Return && isGameRunning) {
                    // กดจบเทิร์นได้เฉพาะตอนที่เป็นตาของเราเท่านั้น
                    if (turnSys.getCurrentPlayer() == 1) {
                        sndClick.play();
                        turnSys.endTurn(units);
                        gui.clearSelection(); selectedUnit = nullptr; worldMap.clearHighlight();
                        activeCityUI = nullptr; // ปิดหน้าต่างเมือง
                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                    }
                }
            }
        }

        // -----------------------------------------------------------------------
        // ระบบสมอง AI (จะทำงานทันทีเมื่อเป็นตาของ Player 2)
        // -----------------------------------------------------------------------
        if (isGameRunning && turnSys.getCurrentPlayer() == 2) {
            if (aiSys.processTurn(units, worldMap, turnSys, combatSys, sndMove, sndDice)) {
                // วนกลับมาตาเราปุ๊บ ก็นับเป็นเทิร์นใหม่ทันที
                currentTurnNumber++;
                std::cout << ">>> Switched to Player 1 <<<" << std::endl;
            }
        }

        // 4. อัปเดตกล้อง (คำนวณการเลื่อน)
        if (!combatSys.isCombatActive()) camera.update(window);

        // 5. นำ View จากกล้องมาใส่ window ก่อนจะทำอย่างอื่น
        window.setView(camera.getView());

        // --- Logic การตรวจสอบ Highlight ---
        if (!combatSys.isCombatActive()) {
            // จุดสำคัญ: ต้องส่ง view ของ camera เข้าไปใน mapPixelToCoords ด้วย
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
            worldMap.updateHighlight(mousePos);
        }

        window.clear(sf::Color(20, 20, 30)); // พื้นหลังสีน้ำเงินเข้มๆ เหมือนอวกาศ

        // สั่งวาด Map แค่บรรทัดเดียว!
        window.setView(camera.getView()); // ใช้ View กล้องวาดแมพ
        worldMap.draw(window);

        // ไฮไลท์เป้าหมายสีแดง
        if (selectedUnit != nullptr && turnSys.getCurrentPlayer() == 1) {
            for (auto& u : units) {
                // ถ้าเป็นของศัตรู และอยู่ในช่องที่เดินไปถึงได้ (isPath = true)
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

        // -----------------------------------------------------------------------
        // ระบบพรางตาศัตรู (วาดเฉพาะตัวที่อยู่ในระยะมองเห็น)
        // -----------------------------------------------------------------------
        for (auto& unit : units) {
            if (unit.getOwner() == 1) {
                // ตัวเรา (Player 1) วาดเสมอ
                unit.draw(window);
            }
            else {
                // ศัตรู (Player 2) เช็คให้ชัวร์ว่าช่องนั้นสว่างอยู่จริงๆ
                HexTile* tile = worldMap.getTile(unit.getR(), unit.getC());
                if (tile != nullptr && tile->isVisible) {
                    unit.draw(window); // วาดก็ต่อเมื่ออยู่ในไฟสว่าง (isVisible)
                }
            }
        }
        worldMap.drawCities(window);

        window.setView(window.getDefaultView()); // คืนค่า View ปกติเพื่อวาด UI ทับข้างบนสุด

        // --- ส่งข้อมูลให้ UI อัปเดตเลขเทิร์นก่อนวาด ---
        gui.updateTurnInfo(turnSys.getCurrentPlayer(), currentTurnNumber);

        // เปรมทำ - ส่งข้อมูลทรัพยากรของเมืองไปแสดงมุมขวาบน
        City* myCity = worldMap.getFirstCity();
        // เช็คเผื่อว่า getFirstCity มีตัวตน (ขึ้นอยู่กับเวอร์ชัน GameMap ที่คุณมี)
        // if (myCity) gui.updateResourceBar(myCity->getWood(), myCity->getGold(), myCity->getFood());
        // เปรมทำ - จบ

        gui.draw(window);

        // --- อัปเดตและวาดฉากลูกเต๋าของ CombatManager ทับล่างสุด ---
        combatSys.updateAndDraw(window, units, worldMap, sndDice, sndHit);

        window.display();
    } // <---  วงเล็บปิดของ while(window.isOpen())

    return 0; // <--- ย้าย return 0 มาไว้จุดล่างสุดนอกลูป

    // =========================================================================
    // โค้ดด้านล่างนี้ถูกเก็บรักษาไว้ 100% ตามคำขอครับ 
    // (ใส่ #if 0 ครอบไว้เพื่อไม่ให้คอมพิวเตอร์งงตอนรันโปรแกรมครับ)
    // =========================================================================
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
}

// ไม่ต้องเอาคอมเมนต์ออก
