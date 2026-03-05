#include "MouseUI.h"
#include <random>
#include <array>
#include <string>

MouseUI::MouseUI() {
    hasFont = font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf");

    isPanelVisible = false;
    m_showSidePanel = false;

    // ขยายกรอบเผื่อใส่ข้อมูลเกณฑ์ทหาร
    infoPanel.setSize(sf::Vector2f(270.f, 220.f));
    infoPanel.setFillColor(sf::Color(30, 30, 30, 220));
    infoPanel.setOutlineThickness(2.f);
    infoPanel.setOutlineColor(sf::Color(100, 200, 255));

    if (hasFont) {
        infoContent.setFont(font);
        infoContent.setCharacterSize(20);
        infoContent.setFillColor(sf::Color::White);

        endTurnBtn.setSize(sf::Vector2f(160.f, 50.f));
        endTurnBtn.setFillColor(sf::Color(150, 50, 50));
        endTurnBtn.setOutlineThickness(3.f);
        endTurnBtn.setOutlineColor(sf::Color(255, 200, 0));

        endTurnText.setFont(font);
        endTurnText.setString("END TURN");
        endTurnText.setCharacterSize(24);
        endTurnText.setFillColor(sf::Color::White);
        endTurnText.setStyle(sf::Text::Bold);

        turnCounterText.setFont(font);
        turnCounterText.setCharacterSize(45);
        turnCounterText.setFillColor(sf::Color::White);
        turnCounterText.setOutlineThickness(2.f);
        turnCounterText.setOutlineColor(sf::Color::Black);
        turnCounterText.setPosition(20.f, 20.f);
        turnCounterText.setString("Player 1 - Turn 1");

        // เปรมทำ - ตั้งค่า text แสดงจำนวนทรัพยากร
        m_woodText.setFont(font);
        m_woodText.setCharacterSize(22);
        m_woodText.setFillColor(sf::Color::White);

        m_goldText.setFont(font);
        m_goldText.setCharacterSize(22);
        m_goldText.setFillColor(sf::Color::Yellow);

        m_foodText.setFont(font);
        m_foodText.setCharacterSize(22);
        m_foodText.setFillColor(sf::Color::Green);

        buildingCityBtn.setSize(sf::Vector2f(180.f, 54.f));
        buildingCityBtn.setFillColor(sf::Color(40, 80, 30));
        buildingCityBtn.setOutlineThickness(2.f);
        buildingCityBtn.setOutlineColor(sf::Color(120, 200, 80));

        buildingCityText.setFont(font);
        buildingCityText.setString("BUILDING CITY");
        buildingCityText.setCharacterSize(20);
        buildingCityText.setFillColor(sf::Color(200, 255, 150));
        buildingCityText.setStyle(sf::Text::Bold);

        // --- ปุ่มโหมดกองทัพ ---
        modeBtn.setSize(sf::Vector2f(200.f, 40.f));
        modeBtn.setOutlineThickness(2.f);
        modeBtn.setOutlineColor(sf::Color::White);
        modeText.setFont(font);
        modeText.setCharacterSize(16);
        modeText.setStyle(sf::Text::Bold);
        modeText.setFillColor(sf::Color::White);
    }

    if (woodTex.loadFromFile("wood.png")) {
        woodIcon.setTexture(woodTex);
        woodIcon.setScale(40.f / woodTex.getSize().x, 40.f / woodTex.getSize().y);
    }
    if (goldTex.loadFromFile("gold.png")) {
        goldIcon.setTexture(goldTex);
        goldIcon.setScale(40.f / goldTex.getSize().x, 40.f / goldTex.getSize().y);
    }
    if (foodTex.loadFromFile("food.png")) {
        foodIcon.setTexture(foodTex);
        foodIcon.setScale(40.f / foodTex.getSize().x, 40.f / foodTex.getSize().y);
    }
}

void MouseUI::showResourcePanel(float windowWidth, int gold, int wood, int food, const std::string& terrainName) {
    isPanelVisible = true;
    m_showSidePanel = false;

    // แก้ไขบัคเพื่อนลืมใส่วงเล็บปิด
    m_targetResourceStr =
        terrainName + "\n\n" +   // ← ใช้ชื่อพื้นที่แทน TILE RESOURCES
        "Gold : " + std::to_string(gold) + "\n" +
        "Wood : " + std::to_string(wood) + "\n" +
        "Food : " + std::to_string(food);
}

void MouseUI::showCityResourcePanel(float windowWidth, int gold, int wood, int food) {
    isPanelVisible = true;
    m_showSidePanel = false;

    m_targetResourceStr =
        "--- CITY STOCKPILE ---\n"
        "Gold : " + std::to_string(gold) + "\n"
        "Wood : " + std::to_string(wood) + "\n"
        "Food : " + std::to_string(food) + "\n\n"
        "[1] Swordsman (20G, 50F)\n"
        "[2] Cavalry (50G, 50W, 50F)";
}

void MouseUI::hideInfo() {
    isPanelVisible = false;
}

void MouseUI::setSelectionList(const std::vector<Unit*>& units) {
    m_selectedUnits = units;
    m_showSidePanel = !units.empty();

    if (m_showSidePanel) {
        isPanelVisible = false;
    }
}

void MouseUI::clearSelection() {
    m_selectedUnits.clear();
    m_showSidePanel = false;
}

void MouseUI::update(sf::Vector2f mousePos) {}

void MouseUI::updateTurnInfo(int playerTurn, int turnNumber) {
    if (!hasFont) return;

    std::string pName = (playerTurn == 1) ? "Player 1" : "AI (Player 2)";
    turnCounterText.setString(pName + "   |   Turn: " + std::to_string(turnNumber));

    if (playerTurn == 1) {
        turnCounterText.setFillColor(sf::Color(100, 255, 100));
        endTurnBtn.setFillColor(sf::Color(150, 50, 50));
    }
    else {
        turnCounterText.setFillColor(sf::Color(255, 100, 100));
        endTurnBtn.setFillColor(sf::Color(80, 80, 80));
    }
}

bool MouseUI::isEndTurnButtonClicked(sf::Vector2f mousePos) {
    return endTurnBtn.getGlobalBounds().contains(mousePos);
}

bool MouseUI::isBuildingCityButtonClicked(sf::Vector2f mousePos) {
    return buildingCityBtn.getGlobalBounds().contains(mousePos);
}

void MouseUI::updateResourceBar(int wood, int gold, int food) {
    m_wood = wood;
    m_gold = gold;
    m_food = food;
    m_woodText.setString(std::to_string(wood));
    m_goldText.setString(std::to_string(gold));
    m_foodText.setString(std::to_string(food));
}

bool MouseUI::isModeButtonClicked(sf::Vector2f mousePos) {
    if (!m_showSidePanel) return false;
    return modeBtn.getGlobalBounds().contains(mousePos);
}

int MouseUI::getClickedItemIndex(sf::Vector2f mousePos) {
    if (!m_showSidePanel) return -1;
    for (size_t i = 0; i < itemRects.size(); ++i) {
        if (itemRects[i].contains(mousePos)) return (int)i;
    }
    return -1;
}

void MouseUI::draw(sf::RenderWindow& window) {
    if (!hasFont) return;

    float screenW = (float)window.getSize().x;
    float screenH = (float)window.getSize().y;

    // ── Roman-style Player Card (top-left) ───────────────
    {
        bool isP1 = (turnCounterText.getFillColor() == sf::Color(100, 255, 100));
        sf::Color cPlayer = isP1 ? sf::Color(100, 220, 100, 255) : sf::Color(220, 90, 90, 255);
        sf::Color cGold = sf::Color(200, 170, 90, 255);
        sf::Color cBg = sf::Color(12, 10, 6, 220);
        sf::Color cRim = sf::Color(160, 130, 45, 200);

        float sx = 16.f, sy = 16.f;
        float medR = 34.f;
        float cardW = 260.f, cardH = medR * 2 + 16.f;
        float medCX = sx + medR + 12.f, medCY = sy + cardH / 2.f;
        float rad = 8.f;

        // พื้นหลัง card
        sf::RectangleShape bgH({ cardW, cardH - rad * 2 }); bgH.setPosition(sx, sy + rad); bgH.setFillColor(cBg); window.draw(bgH);
        sf::RectangleShape bgV({ cardW - rad * 2, cardH }); bgV.setPosition(sx + rad, sy); bgV.setFillColor(cBg); window.draw(bgV);
        float cx4[4] = { sx, sx + cardW - rad * 2, sx, sx + cardW - rad * 2 };
        float cy4[4] = { sy, sy, sy + cardH - rad * 2, sy + cardH - rad * 2 };
        for (int i = 0; i < 4; i++) { sf::CircleShape c(rad); c.setPosition(cx4[i], cy4[i]); c.setFillColor(cBg); window.draw(c); }

        // ขอบโค้งมน
        sf::VertexArray ol(sf::LineStrip, 9);
        ol[0] = { {sx + rad,        sy},                cRim };
        ol[1] = { {sx + cardW - rad,sy},                cRim };
        ol[2] = { {sx + cardW,      sy + rad},          cRim };
        ol[3] = { {sx + cardW,      sy + cardH - rad},  cRim };
        ol[4] = { {sx + cardW - rad,sy + cardH},        cRim };
        ol[5] = { {sx + rad,        sy + cardH},        cRim };
        ol[6] = { {sx,              sy + cardH - rad},  cRim };
        ol[7] = { {sx,              sy + rad},          cRim };
        ol[8] = { {sx + rad,        sy},                cRim };
        window.draw(ol);

        // Medallion วงกลม
        sf::CircleShape outerRim(medR);
        outerRim.setOrigin(medR, medR); outerRim.setPosition(medCX, medCY);
        outerRim.setFillColor(cRim); window.draw(outerRim);

        sf::CircleShape medBody(medR * 0.84f);
        medBody.setOrigin(medR * 0.84f, medR * 0.84f); medBody.setPosition(medCX, medCY);
        medBody.setFillColor(cBg); window.draw(medBody);

        sf::CircleShape playerRing(medR * 0.84f);
        playerRing.setOrigin(medR * 0.84f, medR * 0.84f); playerRing.setPosition(medCX, medCY);
        playerRing.setFillColor(sf::Color::Transparent);
        playerRing.setOutlineThickness(2.5f);
        playerRing.setOutlineColor(cPlayer); window.draw(playerRing);

        // ไอคอนใน medallion
        sf::Text flag(isP1 ? "P1" : "AI", font, (unsigned)(medR * 0.60f));
        flag.setFillColor(cPlayer); flag.setStyle(sf::Text::Bold);
        sf::FloatRect fb = flag.getLocalBounds();
        flag.setOrigin(fb.left + fb.width / 2.f, fb.top + fb.height / 2.f);
        flag.setPosition(medCX, medCY); window.draw(flag);

        // ข้อความทางขวา
        float textX = medCX + medR + 12.f;

        // ชื่อผู้เล่น
        sf::Text pText(isP1 ? "PLAYER 1" : "AI PLAYER", font, 20);
        pText.setFillColor(cPlayer); pText.setStyle(sf::Text::Bold); pText.setLetterSpacing(1.8f);
        sf::FloatRect pb = pText.getLocalBounds(); pText.setOrigin(pb.left, pb.top);
        pText.setPosition(textX, medCY - cardH * 0.28f); window.draw(pText);

        // เส้นคั่น
        float lineW = cardW - (textX - sx) - 10.f;
        sf::RectangleShape div({ lineW, 1.f });
        div.setPosition(textX, medCY - 1.f);
        div.setFillColor(sf::Color(160, 130, 45, 120)); window.draw(div);

        // เลข Turn
        std::string full = turnCounterText.getString();
        std::string turnStr = "TURN  1";
        size_t pos = full.find("Turn: ");
        if (pos != std::string::npos) turnStr = "TURN  " + full.substr(pos + 6);
        sf::Text tText(turnStr, font, 16);
        tText.setFillColor(cGold); tText.setLetterSpacing(2.f);
        sf::FloatRect tb = tText.getLocalBounds(); tText.setOrigin(tb.left, tb.top);
        tText.setPosition(textX, medCY + cardH * 0.08f); window.draw(tText);
    }

    // ── END TURN circular button (bottom-right) ───────────
    {
        float cx = screenW - 70.f;
        float cy = screenH - 80.f;
        float R = 46.f;

        bool isP1 = (endTurnBtn.getFillColor() != sf::Color(80, 80, 80));
        sf::Color cRim = isP1 ? sf::Color(200, 165, 55, 220) : sf::Color(100, 100, 100, 180);
        sf::Color cBody = isP1 ? sf::Color(40, 32, 20, 235) : sf::Color(60, 60, 60, 220);
        sf::Color cCore = isP1 ? sf::Color(25, 20, 12, 225) : sf::Color(40, 40, 40, 210);

        sf::CircleShape rim(R);
        rim.setOrigin(R, R); rim.setPosition(cx, cy);
        rim.setFillColor(cRim); window.draw(rim);

        sf::CircleShape body(R * 0.84f);
        body.setOrigin(R * 0.84f, R * 0.84f); body.setPosition(cx, cy);
        body.setFillColor(cBody); window.draw(body);

        sf::CircleShape ring(R * 0.84f);
        ring.setOrigin(R * 0.84f, R * 0.84f); ring.setPosition(cx, cy);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(1.5f);
        ring.setOutlineColor(sf::Color(180, 145, 50, 160)); window.draw(ring);

        sf::CircleShape core(R * 0.60f);
        core.setOrigin(R * 0.60f, R * 0.60f); core.setPosition(cx, cy);
        core.setFillColor(cCore);
        core.setOutlineThickness(1.f);
        core.setOutlineColor(cRim); window.draw(core);

        sf::Text icon(">", font, (unsigned)(R * 0.60f * 0.9f));
        icon.setFillColor(cRim); icon.setStyle(sf::Text::Bold);
        sf::FloatRect ib = icon.getLocalBounds();
        icon.setOrigin(ib.left + ib.width / 2.f, ib.top + ib.height / 2.f);
        icon.setPosition(cx, cy - R * 0.60f * 0.05f); window.draw(icon);

        sf::Text lbl(isP1 ? "END TURN" : "AI TURN", font, (unsigned)(R * 0.34f));
        lbl.setFillColor(isP1 ? sf::Color(230, 210, 160, 255) : sf::Color(130, 130, 130, 255));
        lbl.setStyle(sf::Text::Bold); lbl.setLetterSpacing(1.5f);
        sf::FloatRect lb = lbl.getLocalBounds();
        lbl.setOrigin(lb.left + lb.width / 2.f, lb.top);
        lbl.setPosition(cx, cy + R + 6.f); window.draw(lbl);

        // อัปเดต bounds สำหรับ click detection (ใช้แทน endTurnBtn เดิม)
        endTurnBtn.setPosition(cx - R, cy - R);
        endTurnBtn.setSize({ R * 2, R * 2 });
    }

    // เปรมทำ - วาด resource bar มุมขวาบน (Roman style แนวตั้ง)
    {
        float iconSize = 32.f;
        float rowH = iconSize + 10.f;
        float panW = 148.f, panH = rowH * 3 + 18.f;
        float margin = 16.f, rad = 8.f;
        float panX = screenW - panW - margin, panY = margin;

        sf::Color cBg = sf::Color(12, 10, 6, 220);
        sf::Color cRim = sf::Color(160, 130, 45, 200);

        // พื้นหลัง rounded
        sf::RectangleShape bgH({ panW, panH - rad * 2 }); bgH.setPosition(panX, panY + rad); bgH.setFillColor(cBg); window.draw(bgH);
        sf::RectangleShape bgV({ panW - rad * 2, panH }); bgV.setPosition(panX + rad, panY); bgV.setFillColor(cBg); window.draw(bgV);
        float cx4[4] = { panX, panX + panW - rad * 2, panX, panX + panW - rad * 2 };
        float cy4[4] = { panY, panY, panY + panH - rad * 2, panY + panH - rad * 2 };
        for (int i = 0; i < 4; i++) { sf::CircleShape c(rad); c.setPosition(cx4[i], cy4[i]); c.setFillColor(cBg); window.draw(c); }

        // ขอบโค้งมน
        sf::VertexArray ol(sf::LineStrip, 9);
        ol[0] = { {panX + rad,       panY},           cRim };
        ol[1] = { {panX + panW - rad, panY},           cRim };
        ol[2] = { {panX + panW,       panY + rad},     cRim };
        ol[3] = { {panX + panW,       panY + panH - rad}, cRim };
        ol[4] = { {panX + panW - rad, panY + panH},    cRim };
        ol[5] = { {panX + rad,        panY + panH},    cRim };
        ol[6] = { {panX,              panY + panH - rad}, cRim };
        ol[7] = { {panX,              panY + rad},     cRim };
        ol[8] = { {panX + rad,        panY},           cRim };
        window.draw(ol);
        // Building City Button (ซ้ายล่าง)
        buildingCityBtn.setPosition(20.f, screenH - 80.f);
        buildingCityText.setPosition(30.f, screenH - 68.f);

        // เปลี่ยนสีปุ่มตอน active mode
        if (m_buildingCityMode) {
            buildingCityBtn.setFillColor(sf::Color(100, 160, 40));
            buildingCityBtn.setOutlineColor(sf::Color(180, 255, 100));
            buildingCityText.setFillColor(sf::Color::White);
        }
        else {
            buildingCityBtn.setFillColor(sf::Color(40, 80, 30));
            buildingCityBtn.setOutlineColor(sf::Color(120, 200, 80));
            buildingCityText.setFillColor(sf::Color(200, 255, 150));
        }
        window.draw(buildingCityBtn);
        window.draw(buildingCityText);

        // เปรมทำ - วาด resource bar มุมขวาบน (ใช้ตัวแปร iconSize และ panX จากบล็อคด้านบน)
    // เปรมทำ - วาด resource bar มุมขวาบน
    float gap = 90.f;
    float startX = screenW - (gap * 3) - 20.f;
    float iconY = 15.f;
    float textY = iconY + iconSize + 2.f;

        float iconX = panX + 10.f, textX = iconX + iconSize + 10.f, startY = panY + 8.f;

        // Wood
        if (woodTex.getSize().x > 0) woodIcon.setScale(iconSize / woodTex.getSize().x, iconSize / woodTex.getSize().y);
        woodIcon.setPosition(iconX, startY + rowH * 0 + 4.f);
        m_woodText.setPosition(textX, startY + rowH * 0 + 6.f);
        window.draw(woodIcon); window.draw(m_woodText);

        // Gold
        if (goldTex.getSize().x > 0) goldIcon.setScale(iconSize / goldTex.getSize().x, iconSize / goldTex.getSize().y);
        goldIcon.setPosition(iconX, startY + rowH * 1 + 4.f);
        m_goldText.setPosition(textX, startY + rowH * 1 + 6.f);
        window.draw(goldIcon); window.draw(m_goldText);

        // Food
        if (foodTex.getSize().x > 0) foodIcon.setScale(iconSize / foodTex.getSize().x, iconSize / foodTex.getSize().y);
        foodIcon.setPosition(iconX, startY + rowH * 2 + 4.f);
        m_foodText.setPosition(textX, startY + rowH * 2 + 6.f);
        window.draw(foodIcon); window.draw(m_foodText);
    }

    // วาด Info Panel ตอนคลิกขวา
    if (isPanelVisible) {
        float panelW = 220.f;
        float panelH = 160.f;
        float margin = 16.f;
        float rad = 8.f;
        float panX = screenW - panelW - margin;
        float panY = margin + 220.f;   // ← เลื่อนลงมาใต้ resource bar

        sf::Color cBg = sf::Color(12, 10, 6, 220);
        sf::Color cRim = sf::Color(160, 130, 45, 200);

        // พื้นหลัง rounded
        sf::RectangleShape bgH({ panelW, panelH - rad * 2 }); bgH.setPosition(panX, panY + rad); bgH.setFillColor(cBg); window.draw(bgH);
        sf::RectangleShape bgV({ panelW - rad * 2, panelH }); bgV.setPosition(panX + rad, panY); bgV.setFillColor(cBg); window.draw(bgV);
        float cx4[4] = { panX, panX + panelW - rad * 2, panX, panX + panelW - rad * 2 };
        float cy4[4] = { panY, panY, panY + panelH - rad * 2, panY + panelH - rad * 2 };
        for (int i = 0; i < 4; i++) { sf::CircleShape c(rad); c.setPosition(cx4[i], cy4[i]); c.setFillColor(cBg); window.draw(c); }

        // ขอบโค้งมนสีทอง
        sf::VertexArray ol(sf::LineStrip, 9);
        ol[0] = { {panX + rad,        panY},                cRim };
        ol[1] = { {panX + panelW - rad, panY},              cRim };
        ol[2] = { {panX + panelW,      panY + rad},         cRim };
        ol[3] = { {panX + panelW,      panY + panelH - rad},cRim };
        ol[4] = { {panX + panelW - rad, panY + panelH},     cRim };
        ol[5] = { {panX + rad,         panY + panelH},      cRim };
        ol[6] = { {panX,               panY + panelH - rad},cRim };
        ol[7] = { {panX,               panY + rad},         cRim };
        ol[8] = { {panX + rad,         panY},               cRim };
        window.draw(ol);

        // เส้นหัวข้อ
        sf::RectangleShape divLine({ panelW - 20.f, 1.f });
        divLine.setPosition(panX + 10.f, panY + 32.f);
        divLine.setFillColor(sf::Color(160, 130, 45, 120));
        window.draw(divLine);

        // header - ออกมาแสดงตรงกลางบน (PLAY)
        std::string headerStr = "";
        std::string bodyStr = m_targetResourceStr;
        size_t newline = m_targetResourceStr.find('\n');
        if (newline != std::string::npos) {
            headerStr = m_targetResourceStr.substr(0, newline);   // บรรทัดแรก
            bodyStr = m_targetResourceStr.substr(newline + 1);  // บรรทัดที่เหลือ
        }

        // header — จัดกลางบนกรอบ (PLAY)
        sf::Text headerText(headerStr, font, 15);
        headerText.setFillColor(sf::Color(200, 170, 80, 255));   // ← สีทองหัวข้อ
        headerText.setStyle(sf::Text::Bold);
        headerText.setLetterSpacing(1.5f);
        sf::FloatRect hb = headerText.getLocalBounds();
        headerText.setOrigin(hb.left + hb.width / 2.f, hb.top); // ← จัดกลาง
        headerText.setPosition(panX + panelW / 2.f, panY + 10.f); // ← กึ่งกลาง X, ชิดบน
        window.draw(headerText);

        // body — แสดงทรัพยากรด้านล่างเส้นคั่น (PLAY)
        sf::Text bodyText(bodyStr, font, 15);
        bodyText.setFillColor(sf::Color(200, 185, 140, 255));  // ← สีข้อความ resource
        bodyText.setStyle(sf::Text::Bold);
        bodyText.setPosition(panX + 14.f, panY + 38.f);        // ← ใต้เส้นคั่น
        window.draw(bodyText);

    }

    if (m_showSidePanel) {
        float panelW = 220.0f; // แก้บัคเพื่อนพิมพ์ผิดเป็น panelWidth

        sf::RectangleShape bg(sf::Vector2f(panelW, screenH));
        bg.setPosition(screenW - panelW, 0);
        bg.setFillColor(sf::Color(20, 20, 20, 200));
        bg.setOutlineColor(sf::Color(100, 100, 100));
        bg.setOutlineThickness(-2.0f);
        window.draw(bg);

        sf::Text title("Selected Units", font, 22);
        title.setPosition(screenW - panelW + 15, 100);
        title.setFillColor(sf::Color::Yellow);
        title.setStyle(sf::Text::Bold);
        window.draw(title);

        sf::RectangleShape line(sf::Vector2f(panelW - 30, 2));
        line.setPosition(screenW - panelW + 15, 135);
        line.setFillColor(sf::Color(100, 100, 100));
        window.draw(line);

        itemRects.clear();
        float startY = 150.0f;
        for (size_t i = 0; i < m_selectedUnits.size(); ++i) {
            auto* u = m_selectedUnits[i];

            sf::FloatRect clickBounds(screenW - panelW + 10.f, startY - 5.f, 200.f, 50.f);
            itemRects.push_back(clickBounds);

            if (!m_isArmyMode && (int)i == m_selectedIndex) {
                sf::RectangleShape highlightBg(sf::Vector2f(200.f, 50.f));
                highlightBg.setPosition(clickBounds.left, clickBounds.top);
                highlightBg.setFillColor(sf::Color(150, 150, 0, 100));
                window.draw(highlightBg);
            }

            sf::Text nameText(u->getName(), font, 18);
            nameText.setPosition(screenW - panelW + 20, startY);

            sf::Text subText("AP: " + std::to_string(u->getCurrentAP()) + " / " + std::to_string(u->getMaxAP()), font, 14);
            subText.setPosition(screenW - panelW + 20, startY + 22);

            if (u->hasAP()) {
                nameText.setFillColor(sf::Color::White);
                subText.setFillColor(sf::Color::Green);
            }
            else {
                nameText.setFillColor(sf::Color(150, 150, 150));
                subText.setFillColor(sf::Color::Cyan);
            }

            window.draw(nameText); window.draw(subText);
            startY += 60.0f;
        }

        modeBtn.setPosition(screenW - panelW + 10.f, screenH - 140.f);
        if (m_isArmyMode) {
            modeBtn.setFillColor(sf::Color(50, 150, 50));
            modeText.setString("MODE: ARMY (ALL)");
        }
        else {
            modeBtn.setFillColor(sf::Color(200, 100, 50));
            modeText.setString("MODE: SPLIT (1)");
        }
        modeText.setPosition(modeBtn.getPosition().x + 20.f, modeBtn.getPosition().y + 10.f);
        window.draw(modeBtn); window.draw(modeText);
    }
}