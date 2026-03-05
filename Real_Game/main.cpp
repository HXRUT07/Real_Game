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
#include "UpkeepManager.h" // <--- ระบบเสบียงความหิว
#include "CombatManager.h" // <--- ระบบต่อสู้ลูกเต๋า
#include "BuildMenu.h"
#include "AIManager.h"    // <--- ระบบสมอง AI

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

    MouseUI gui; //(PLAY)
    CityPanel cityPanel(window.getSize().x, window.getSize().y);

    // --- ระบบต่อสู้ ---
    CombatManager combatSys;
    if (hasCombatFont) combatSys.setFont(combatFont);

    BuildMenu buildMenu(window.getSize().x, window.getSize().y);

    //----Unit System----//
    std::vector<Unit> units;       // เก็บยูนิตทั้งหมด

    // จองพื้นที่ล่วงหน้า 1,000 ตัว ป้องกันบัค C++ ย้ายที่อยู่หน่วยความจำ (Memory Dangling)
    units.reserve(1000);

    Unit* selectedUnit = nullptr;  // ตัวที่กำลังเลือกอยู่
    bool isGameRunning = false;    // ตัวแปรเช็คว่าจบช่วงเลือกจุดเกิดหรือยัง
    int unitNameCounter = 1;       // ตัวนับสำหรับตั้งชื่อ Unit อัตโนมัติ
    TurnManager turnSys(2); // สร้างระบบเทิร์นสำหรับ 2 ผู้เล่น

    // --- ตัวแปรเก็บจำนวนเทิร์นที่ผ่านไป ---
    int currentTurnNumber = 1;

    // --- ตัวแปรสำหรับระบบกองทัพ (Army) ---
    std::vector<Unit*> currentStack;
    Unit* leadUnit = nullptr;

    City* activeCityUI = nullptr; // ตัวแปรจดจำเมืองที่เรากำลังเปิดดูอยู่ (เพื่อเอาไว้สร้างทหาร)

    // --- ตัวแปรสมอง AI ---
    AIManager aiManager;

    while (window.isOpen()) {
        sf::Vector2f mousePosScreen = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Event event;

        // [จุดแก้ไข] ลูป pollEvent จะจบแค่ตรงที่เช็คปุ่ม/เมาส์ เท่านั้น
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // กด Escape เพื่อออกจากเกมได้ (สำคัญมากตอนเทส Fullscreen ไม่งั้นออกยาก)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

            cityPanel.handleEvent(event);
            buildMenu.handleEvent(event);

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

                // เช็คคลิก UI แยกทัพด้านขวาก่อน
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
                        // ล้างไฮไลท์เก่าก่อนแสดงระยะเดินของตัวที่เลือกแยก (Individual Movement)
                        worldMap.clearHighlight();
                        worldMap.calculateValidMoves(leadUnit->getR(), leadUnit->getC(), leadUnit->getMoveRange());
                        sndClick.play();
                        continue;
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Right) {
                    // 1. ยกเลิกการเลือกทหารและไฮไลท์
                    gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                    activeCityUI = nullptr; // เคลียร์เมืองที่เลือกอยู่

                    // 2. ดึงข้อมูลทรัพยากรจากช่องที่คลิก
                    int r = 0, c = 0;
                    if (worldMap.getGridCoords(worldPos, r, c)) {

                        // --- [อัปเดตระบบใหม่] เช็คก่อนว่ามีเมืองอยู่ตรงนี้ไหม ---
                        City* clickedCity = worldMap.getCityAt(r, c);
                        if (clickedCity != nullptr) {
                            // ถ้ามีเมือง ให้โชว์หน้าต่างคลังหลวงของเมือง (CITY STOCKPILE)
                            activeCityUI = clickedCity;
                            cityPanel.setCity(clickedCity);
                            gui.hideInfo();
                        }
                        else {
                            cityPanel.clear();
                            // ถ้าไม่มีเมือง ค่อยไปดึงข้อมูลทรัพยากรบนพื้นดินปกติ
                            HexTile* clickedTile = worldMap.getTile(r, c);

                            // กฎ: ต้องมีช่องนี้อยู่จริง และ "ต้องเคยสำรวจแล้ว (isExplored)" เท่านั้น
                            if (clickedTile != nullptr && clickedTile->isExplored) {
                                // โชว์ข้อมูลทรัพยากรของช่องนั้น
                                gui.showResourcePanel((float)window.getSize().x,
                                    clickedTile->gold, clickedTile->wood, clickedTile->food);
                            }
                            else {
                                // ถ้าคลิกขวาใส่หมอกดำๆ ให้ปิดหน้าต่างทิ้ง
                                gui.hideInfo();
                            }
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Left) {

                    // ---  เช็คก่อนเลยว่าคลิกโดนปุ่ม "END TURN" หรือเปล่า? ---
                    if (gui.isEndTurnButtonClicked(uiPos) && isGameRunning) {
                        sndClick.play();
                        turnSys.endTurn(units);

                        // --- [NEW] ประมวลผลความหิวของ AI ทันทีที่ผู้เล่นกด Enter จบเทิร์น ---
                        UpkeepManager::processAIUpkeep(units, aiManager.getAIFood());

                        // เคลียร์ UI ที่เลือกค้างไว้
                        gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                        activeCityUI = nullptr;

                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                        continue; // ข้ามการทำงานด้านล่างไปเลย เพราะคลิกปุ่มไปแล้ว
                    }

                    // คลิกซ้าย: ซ่อน Info Panel เดิมก่อน
                    // BUILD CITY button -> เปิด/ปิด BuildMenu
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

                    // --- PHASE 1: เลือกจุดเกิด ---
                    if (!worldMap.isGameStarted()) {
                        // 3. ส่งให้ GameMap จัดการเลือกจุดเกิด
                        worldMap.handleMouseClick(worldPos);

                        // เช็คว่า Map เริ่มเกมสำเร็จหรือยัง?
                        if (worldMap.isGameStarted()) {
                            isGameRunning = true;

                            // *** Spawn ทหารตัวแรกตรงจุดที่คลิก ***
                            int spawnR = 0, spawnC = 0;
                            if (worldMap.getGridCoords(worldPos, spawnR, spawnC)) {
                                sndMove.play();
                                // ใช้ทหารชื่อ Swordsman เพื่อไม่ให้บัคภาพล่องหน
                                units.emplace_back("Swordsman", spawnR, spawnC, 1);

                                // --- ยัด Starter Pack ให้เมืองหลวงทันที! ---
                                City* myCity = worldMap.getFirstCity();
                                if (myCity != nullptr) {
                                    ResourceYield starter = worldMap.getStarterPackValues();
                                    myCity->addGold(starter.gold);
                                    myCity->addWood(starter.wood);
                                    myCity->addFood(starter.food);
                                    std::cout << "Received Starter Pack: " << starter.gold << "G " << starter.wood << "W " << starter.food << "F!" << std::endl;
                                }

                                // สุ่มหาจุดเกิดให้ AI ให้ไกล 15-30 ช่อง
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
                                units.emplace_back("Enemy", enemyR, enemyC, 2);
                            }
                        }
                    }
                    // --- PHASE 2: ควบคุมทหาร (Gameplay) ---
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
                                // 1. หา Unit ทั้งหมดที่อยู่ในช่องนี้ (Stacking)
                                std::vector<Unit*> stackInTile;
                                bool hasOurUnit = false;
                                for (auto& u : units) {
                                    if (u.getR() == r && u.getC() == c) {
                                        stackInTile.push_back(&u);
                                        if (u.getOwner() == turnSys.getCurrentPlayer()) hasOurUnit = true;
                                    }
                                }

                                // 1. กรณีที่มีตัวละครถูกเลือกอยู่แล้ว (กำลังจะเดิน หรือ โจมตี)
                                if (leadUnit != nullptr && worldMap.isValidMove(r, c)) {
                                    auto enemyIt = std::find_if(units.begin(), units.end(), [&](Unit& u) { return u.getR() == r && u.getC() == c && u.getOwner() != turnSys.getCurrentPlayer(); });

                                    if (enemyIt != units.end()) {
                                        // สั่งโจมตี! เรียกใช้ CombatManager
                                        combatSys.initiateCombat(leadUnit->getR(), leadUnit->getC(), r, c, 1, sndDice, gui.isArmyMode());

                                        gui.clearSelection(); worldMap.clearHighlight();
                                        leadUnit = nullptr; currentStack.clear();
                                    }
                                    else {
                                        // เดิน!
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
                                // 2. กรณีที่คลิกใส่ช่องที่มีตัวละครของเรา (เพื่อเลือกตัวละครใหม่) -- *จุดแก้บักโชว์ระยะเดินผิดตัว*
                                else if (!stackInTile.empty() && hasOurUnit) {
                                    // ล้างไฮไลท์เก่าทิ้งก่อนทุกครั้งที่เลือกใหม่ เพื่อป้องกันเงาระยะเดินตัวเก่าค้าง
                                    worldMap.clearHighlight();

                                    currentStack = stackInTile;
                                    gui.setSelectionList(currentStack);
                                    gui.setArmyMode(true); // Default เป็น Army Mode เสมอเวลาคลิกเลือกก้อนทหาร
                                    gui.setSelectedIndex(0);

                                    leadUnit = nullptr;
                                    for (auto* u : currentStack) {
                                        // หาตัวแรกที่มี AP เพื่อตั้งเป็น Leader และคำนวณระยะเดินจากตัวนั้นจริงๆ
                                        if (u->hasAP() && u->getOwner() == turnSys.getCurrentPlayer()) { leadUnit = u; break; }
                                    }

                                    // โชว์ระยะเดินเฉพาะของตัวที่ถูกเลือกเป็น Leader (Respect individual MoveRange)
                                    if (leadUnit) worldMap.calculateValidMoves(leadUnit->getR(), leadUnit->getC(), leadUnit->getMoveRange());
                                    else worldMap.clearHighlight(); // ถ้าหมด AP ทั้งกอง ก็ไม่ต้องโชว์ระยะ
                                }
                                // 3. กรณีคลิกพื้นว่างเปล่า โดยไม่ได้เลือกใครไว้
                                else {
                                    gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                                }
                            }
                        }
                    }
                }
            }

            // =======================================================================
            // [DEBUG / TEST] ระบบเทสคีย์บอร์ด (ปุ่มเสกทหารแบบ RISK)
            // =======================================================================
            if (event.type == sf::Event::KeyPressed && !combatSys.isCombatActive()) {

                // R: รีเซ็ต AP ของทุก Unit (จำลองการจบ Turn)
                if (event.key.code == sf::Keyboard::R) {
                    for (auto& u : units) u.resetAP();
                    std::cout << "Next Turn: All AP Reset" << std::endl;
                }

                // [ปุ่มเสกทหารสำหรับ TEST] กด '+' หรือ '=' เสกทหารฟรีๆ ที่เมืองหลวง!
                if (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Equal) {
                    City* myCity = worldMap.getFirstCity();
                    if (myCity != nullptr) {
                        units.emplace_back("Swordsman", myCity->getR(), myCity->getC(), 1);
                        sndMove.play();
                        std::cout << "[TEST] Spawned 1 Swordsman at Capital!\n";
                    }
                }

                // [NEW] ระบบเสกทหารตรงจุดที่เมาส์ชี้ (Debug Spawn) เอาไว้เทส
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
                            units.emplace_back("Enemy", r, c, 2);
                            sndMove.play();
                        }
                    }
                }

                // ปุ่มเกณฑ์ทหารจากเมือง
                if (activeCityUI != nullptr && turnSys.getCurrentPlayer() == 1) {
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                        if (activeCityUI->getGold() >= 20 && activeCityUI->getFood() >= 50) {
                            activeCityUI->addGold(-20);
                            activeCityUI->addFood(-50);
                            units.emplace_back("Swordsman", activeCityUI->getR(), activeCityUI->getC(), 1);
                            sndMove.play();
                        }
                    }
                    // แพ็กเกจใหญ่ ซื้อทีละ 5 ตัว!
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

            //  ใช้ KeyReleased (ปล่อยนิ้ว) และแก้จาก Enter เป็น Return
            if (event.type == sf::Event::KeyReleased && !combatSys.isCombatActive()) {
                if (event.key.code == sf::Keyboard::Return && isGameRunning) {
                    // กดจบเทิร์นได้เฉพาะตอนที่เป็นตาของเราเท่านั้น
                    if (turnSys.getCurrentPlayer() == 1) {
                        sndClick.play();
                        turnSys.endTurn(units); // เรียกสลับเทิร์นและรีเซ็ต AP

                        UpkeepManager::processAIUpkeep(units, aiManager.getAIFood());

                        // เคลียร์ UI ที่เลือกค้างไว้
                        gui.clearSelection(); leadUnit = nullptr; currentStack.clear(); worldMap.clearHighlight();
                        activeCityUI = nullptr;

                        std::cout << ">>> Switched to AI (Player 2) <<<" << std::endl;
                    }
                }
            }
        } // <---  วงเล็บปิดของ while(window.pollEvent)

        // -----------------------------------------------------------------------
        // ระบบสมอง AI นักล่า - แบบหน่วงเวลาให้เห็นมันเดินทีละก้าว!
        // -----------------------------------------------------------------------
        if (isGameRunning && turnSys.getCurrentPlayer() == 2 && !combatSys.isCombatActive()) {
            bool aiEndedTurn = aiManager.processTurn(units, worldMap, turnSys, combatSys, sndMove, sndDice);
            if (aiEndedTurn) {
                currentTurnNumber++;
                std::cout << ">>> Switched to Player 1 <<<" << std::endl;
                // --- ประมวลผลความหิวของผู้เล่นทันทีที่ AI จบเทิร์น ---
                UpkeepManager::processPlayerUpkeep(units, worldMap.getFirstCity());
            }
        }

        // --- Rendering ---
        // 4. อัปเดตกล้อง (คำนวณการเลื่อน)
        if (!combatSys.isCombatActive()) camera.update(window);

        // 5. นำ View จากกล้องมาใส่ window ก่อนจะทำอย่างอื่น
        window.setView(camera.getView());

        // --- Logic การตรวจสอบ Highlight ---
        // จุดสำคัญ: ต้องส่ง view ของ camera เข้าไปใน mapPixelToCoords ด้วย
        if (!combatSys.isCombatActive()) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera.getView());
            worldMap.updateHighlight(mousePos);
        }

        if (isGameRunning) {
            worldMap.updateVision(units, 1); // เลข 1 คือให้เปิดไฟให้ Player 1 (เรา)
        }

        // พื้นหลังสีน้ำเงินเข้มๆ เหมือนอวกสร้าง
        window.clear(sf::Color(20, 20, 30));

        // สั่งวาด Map แค่บรรทัดเดียว!
        window.setView(camera.getView()); // ใช้ View กล้องวาดแมพ
        worldMap.draw(window);

        // ไฮไลท์เป้าหมายสีแดง
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

        // -----------------------------------------------------------------------
        //  ระบบพรางตาศัตรู (วาดเฉพาะตัวที่อยู่ในระยะมองเห็น)
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

        // --- ส่งข้อมูลให้ UI อัปเดตเลขเทิร์นก่อนวาด ---
        gui.updateTurnInfo(turnSys.getCurrentPlayer(), currentTurnNumber);

        // เปรมทำ - ส่งข้อมูลทรัพยากรของเมืองไปแสดงมุมขวาบน
        City* myCity = worldMap.getFirstCity();
        if (myCity) gui.updateResourceBar(myCity->getWood(), myCity->getGold(), myCity->getFood());
        // เปรมทำ - จบ

        // คืนค่า View ปกติเพื่อวาด UI ทับข้างบนสุด
        window.setView(window.getDefaultView());

        if (worldMap.isGameStarted()) {
            gui.draw(window);
        }
        cityPanel.draw(window);
        buildMenu.draw(window);

        cityPanel.draw(window);

        // ให้ CombatManager จัดการวาดและลบ Unit ให้เสร็จสรรพ
        combatSys.updateAndDraw(window, units, worldMap, sndDice, sndHit);

        window.display();
    } // <---  วงเล็บปิดของ while(window.isOpen())

    return 0; // <---  ย้าย return 0 มาไว้จุดล่างสุดนอกลูป
}