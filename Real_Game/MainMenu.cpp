#include "MainMenu.h"
#include <cmath>
#include <algorithm>
#include <cstdio>

namespace Pal {
    const sf::Color PanelFill{ 18, 14, 10,200 };
    const sf::Color PanelBorder{ 160,145,115,170 };
    const sf::Color TitleNormal{ 240,230,200,255 };
    const sf::Color TitleShadow{ 0,  0,  0,160 };
    const sf::Color BtnNormal{ 230,220,195,255 };
    const sf::Color BtnHover{ 255,255,255,255 };
    const sf::Color SepColor{ 160,145,115, 80 };
    const sf::Color BgFallback{ 40, 32, 22,255 };
    const sf::Color Gold{ 255, 215, 80, 255 };  // ทองสว่างขึ้น
    const sf::Color GoldDim{ 140,110, 40,180 };
    const sf::Color TrackBg{ 50, 42, 30,255 };
    const sf::Color TrackFill{ 180,145, 55,255 };
    const sf::Color Subtitle{ 160,145,100,180 };
    const sf::Color CreditName{ 230, 215, 175, 255 };
}

static float lerpF(float a, float b, float t) { return a + (b - a) * t; }
static sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t) {
    auto c = [](float x) {return static_cast<sf::Uint8>(std::clamp(x, 0.f, 255.f)); };
    return{ c(lerpF(a.r,b.r,t)),c(lerpF(a.g,b.g,t)),c(lerpF(a.b,b.b,t)),c(lerpF(a.a,b.a,t)) };
}
static void centerText(sf::Text& t, float cx, float cy) {
    sf::FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
    t.setPosition(cx, cy);
}

// ── Rounded rect (fill + 8-segment border) ───────────────────────
static void drawRR(sf::RenderWindow& win,
    float x, float y, float w, float h, float rad,
    sf::Color fill, sf::Color border)
{
    rad = std::min(rad, std::min(w, h) / 2.f);

    // Fill rectangles
    sf::RectangleShape rH({ w, h - rad * 2 }); rH.setPosition(x, y + rad); rH.setFillColor(fill); win.draw(rH);
    sf::RectangleShape rV({ w - rad * 2, h }); rV.setPosition(x + rad, y); rV.setFillColor(fill); win.draw(rV);

    // มุมโค้ง — ใช้ CircleShape pointCount=16 และ setTextureRect เพื่อตัดเฉพาะ 1/4
    // แต่ SFML ไม่รองรับตัด arc ตรงๆ — ใช้วิธี VertexArray แทน
    const int   SEG = 12;   // ยิ่งมาก ยิ่งเนียน
    const float PI = 3.14159265f;

    // มุมทั้ง 4: (cx, cy, startAngle)
    float corners[4][3] = {
        {x + rad,   y + rad,        PI      },   // บนซ้าย
        {x + w - rad, y + rad,      PI * 1.5f },   // บนขวา
        {x + w - rad, y + h - rad,  0.f     },   // ล่างขวา
        {x + rad,   y + h - rad,    PI * 0.5f }    // ล่างซ้าย
    };

    for (auto& cor : corners)
    {
        // Fill fan
        sf::VertexArray fan(sf::TriangleFan, SEG + 2);
        fan[0] = { {cor[0], cor[1]}, fill };
        for (int j = 0; j <= SEG; j++)
        {
            float a = cor[2] + (PI * 0.5f) * j / SEG;
            fan[j + 1] = { {cor[0] + std::cos(a) * rad, cor[1] + std::sin(a) * rad}, fill };
        }
        win.draw(fan);

        // Border arc
        sf::VertexArray arc(sf::LineStrip, SEG + 1);
        for (int j = 0; j <= SEG; j++)
        {
            float a = cor[2] + (PI * 0.5f) * j / SEG;
            arc[j] = { {cor[0] + std::cos(a) * rad, cor[1] + std::sin(a) * rad}, border };
        }
        win.draw(arc);
    }

    // Border เส้นตรง 4 ด้าน
    sf::VertexArray lines(sf::Lines, 8);
    lines[0] = { {x + rad,      y},          border }; lines[1] = { {x + w - rad, y},        border };
    lines[2] = { {x + w,        y + rad},    border }; lines[3] = { {x + w,     y + h - rad},border };
    lines[4] = { {x + w - rad,  y + h},      border }; lines[5] = { {x + rad,   y + h},      border };
    lines[6] = { {x,            y + h - rad},border }; lines[7] = { {x,      y + rad},       border };
    win.draw(lines);
}

static MenuButton makeBackBtn(sf::Font& font, float px, float py, float pw, float ph, float H) {
    MenuButton btn;
    btn.label.setFont(font);
    btn.label.setString("< BACK");
    btn.label.setCharacterSize(static_cast<unsigned>(H * 0.032f));
    btn.label.setLetterSpacing(2.f);
    btn.label.setFillColor(Pal::BtnNormal);
    centerText(btn.label, px + pw * 0.5f, py + ph * 0.92f);
    sf::FloatRect lb = btn.label.getGlobalBounds();
    btn.bounds = { lb.left - 20,lb.top - 10,lb.width + 40,lb.height + 20 };
    return btn;
}

// ── Constructor ──────────────────────────────────────────────────
MainMenu::MainMenu(sf::RenderWindow& window,
    const std::string& bgImagePath,
    const std::string& fontPath)
    : m_window(window)
{
    m_W = static_cast<float>(window.getSize().x);
    m_H = static_cast<float>(window.getSize().y);

    if (!m_font.loadFromFile(fontPath))
        for (const auto& fb : { "assets/fonts/CinzelDecorative-Regular.ttf",
                             "assets/fonts/Cinzel-Regular.ttf",
                             "C:/Windows/Fonts/times.ttf" })
            if (m_font.loadFromFile(fb)) break;

    if (m_bgTexture.loadFromFile(bgImagePath)) {
        m_bgLoaded = true;
        m_bgSprite.setTexture(m_bgTexture);
        float s = std::max(m_W / m_bgTexture.getSize().x, m_H / m_bgTexture.getSize().y);
        m_bgSprite.setScale(s, s);
        m_bgSprite.setPosition((m_W - m_bgTexture.getSize().x * s) / 2.f,
            (m_H - m_bgTexture.getSize().y * s) / 2.f);
    }
    else {
        m_bgFallback.setSize({ m_W,m_H });
        m_bgFallback.setFillColor(Pal::BgFallback);
    }
    buildMainUI();
    buildSettingsUI();
    buildCreditsUI();
}

// ── loadVideoFrames ──────────────────────────────────────────────
void MainMenu::loadVideoFrames(const std::string& folder, int count) {
    m_videoFrames.resize(count);
    for (int i = 0; i < count; i++) {
        char fn[512];
        std::snprintf(fn, sizeof(fn), "%s/frame_%04d.png", folder.c_str(), i + 1);
        if (!m_videoFrames[i].loadFromFile(fn)) { m_videoLoaded = false; return; }
    }
    m_videoSprite.setTexture(m_videoFrames[0]);
    float s = std::max(m_W / 960.f, m_H / 540.f);
    m_videoSprite.setScale(s, s);
    m_videoLoaded = true;
}

// ── buildMainUI ──────────────────────────────────────────────────
void MainMenu::buildMainUI() {
    m_mainPW = m_W * 0.38f; m_mainPH = m_H * 0.62f;
    m_mainPX = m_W - m_mainPW - m_W * 0.04f;
    m_mainPY = (m_H - m_mainPH) / 2.f;

    m_titleText.setFont(m_font);
    m_titleText.setString("SARVIER");
    m_titleText.setCharacterSize(static_cast<unsigned>(m_H * 0.085f));
    m_titleText.setLetterSpacing(1.8f);
    m_titleText.setFillColor(Pal::TitleNormal);
    {
        sf::FloatRect tb = m_titleText.getLocalBounds();
        m_titleText.setPosition(m_mainPX + m_mainPW - tb.width - tb.left,
            m_mainPY - tb.height - tb.top - m_H * 0.04f);
    }

    struct BD { std::string l; MenuState a; };
    const std::vector<BD> defs = {
        {"PLAY",    MenuState::Play},
        {"SETTINGS",MenuState::Settings},
        {"CREDITS", MenuState::Credits},
        {"EXIT",    MenuState::Exit}
    };
    unsigned fs = static_cast<unsigned>(m_H * 0.055f);
    float slotH = m_mainPH / defs.size();
    float cx = m_mainPX + m_mainPW / 2.f;

    m_buttons.clear(); m_separators.clear();
    for (size_t i = 0; i < defs.size(); i++) {
        MenuButton btn;
        btn.action = defs[i].a;
        btn.label.setFont(m_font);
        btn.label.setString(defs[i].l);
        btn.label.setCharacterSize(fs);
        btn.label.setLetterSpacing(2.5f);
        btn.label.setFillColor(Pal::BtnNormal);
        centerText(btn.label, cx, m_mainPY + slotH * i + slotH / 2.f);
        btn.bounds = { m_mainPX,m_mainPY + slotH * i,m_mainPW,slotH };
        m_buttons.push_back(std::move(btn));
        if (i < defs.size() - 1) {
            sf::RectangleShape sep({ m_mainPW * 0.75f,1.f });
            sep.setOrigin(m_mainPW * 0.375f, 0.5f);
            sep.setPosition(cx, m_mainPY + slotH * (i + 1));
            sep.setFillColor(Pal::SepColor);
            m_separators.push_back(sep);
        }
    }
}

// ── buildSettingsUI ──────────────────────────────────────────────
void MainMenu::buildSettingsUI() {
    m_setPW = m_W * 0.50f; m_setPH = m_H * 0.72f;
    m_setPX = (m_W - m_setPW) / 2.f; m_setPY = (m_H - m_setPH) / 2.f;

    m_settingsTitle.setFont(m_font);
    m_settingsTitle.setString("SETTINGS");
    m_settingsTitle.setCharacterSize(static_cast<unsigned>(m_H * 0.058f));
    m_settingsTitle.setLetterSpacing(3.f);
    m_settingsTitle.setFillColor(Pal::TitleNormal);
    centerText(m_settingsTitle, m_setPX + m_setPW / 2.f, m_setPY + m_H * 0.068f);

    m_settingsDivider.setSize({ m_setPW * 0.7f,1.f });
    m_settingsDivider.setOrigin(m_setPW * 0.35f, 0.5f);
    m_settingsDivider.setPosition(m_setPX + m_setPW / 2.f, m_setPY + m_H * 0.115f);
    m_settingsDivider.setFillColor(Pal::GoldDim);

    m_settingsSubtitle.setFont(m_font);
    m_settingsSubtitle.setString("~ Audio & Display ~");
    m_settingsSubtitle.setCharacterSize(static_cast<unsigned>(m_H * 0.022f));
    m_settingsSubtitle.setStyle(sf::Text::Italic);
    m_settingsSubtitle.setFillColor(Pal::Subtitle);
    centerText(m_settingsSubtitle, m_setPX + m_setPW / 2.f, m_setPY + m_H * 0.135f);

    struct SI { std::string n; int v; };
    std::vector<SI> defs = { {"Music Volume",m_musicVol},{"SFX Volume",m_sfxVol},{"Brightness",m_brightness} };
    m_settingItems.clear();
    float startY = m_setPY + m_H * 0.22f, rowH = m_H * 0.14f;
    float cx = m_setPX + m_setPW / 2.f;
    float btnSz = m_H * 0.05f, trackW = m_setPW * 0.52f, trackH = m_H * 0.018f;

    for (size_t i = 0; i < defs.size(); i++) {
        SettingItem item;
        item.name = defs[i].n; item.value = defs[i].v;
        float rowCy = startY + rowH * i;

        item.labelText.setFont(m_font);
        item.labelText.setString(item.name);
        item.labelText.setCharacterSize(static_cast<unsigned>(m_H * 0.030f));
        item.labelText.setLetterSpacing(1.5f);
        item.labelText.setFillColor(Pal::BtnNormal);
        centerText(item.labelText, cx, rowCy);

        item.trackBar.setSize({ trackW,trackH });
        item.trackBar.setOrigin(trackW / 2.f, trackH / 2.f);
        item.trackBar.setPosition(cx, rowCy + m_H * 0.048f);
        item.trackBar.setFillColor(Pal::TrackBg);
        item.trackBar.setOutlineColor(Pal::GoldDim);
        item.trackBar.setOutlineThickness(1.f);

        float fillW = trackW * item.value / 100.f;
        item.fillBar.setSize({ std::max(fillW,2.f),trackH });
        item.fillBar.setOrigin(0.f, trackH / 2.f);
        item.fillBar.setPosition(cx - trackW / 2.f, rowCy + m_H * 0.048f);
        item.fillBar.setFillColor(Pal::TrackFill);

        float gap = m_W * 0.012f;              // ช่องว่างระหว่างปุ่มกับ track
        float bxL = cx - trackW / 2.f - gap - btnSz / 2.f;
        float bxR = cx + trackW / 2.f + gap + btnSz / 2.f;
        float bcy = rowCy + m_H * 0.048f;

        item.btnLeft.setSize({ btnSz,btnSz });
        item.btnLeft.setOrigin(btnSz / 2.f, btnSz / 2.f);
        item.btnLeft.setPosition(bxL, bcy);
        item.btnLeft.setFillColor({ 40,32,20,200 });
        item.btnLeft.setOutlineColor(Pal::GoldDim);
        item.btnLeft.setOutlineThickness(1.f);

        item.btnRight.setSize({ btnSz,btnSz });
        item.btnRight.setOrigin(btnSz / 2.f, btnSz / 2.f);
        item.btnRight.setPosition(bxR, bcy);
        item.btnRight.setFillColor({ 40,32,20,200 });
        item.btnRight.setOutlineColor(Pal::GoldDim);
        item.btnRight.setOutlineThickness(1.f);

        item.arrowLeft.setFont(m_font);
        item.arrowLeft.setString("<");
        item.arrowLeft.setCharacterSize(static_cast<unsigned>(m_H * 0.028f));
        item.arrowLeft.setFillColor(Pal::Gold);
        centerText(item.arrowLeft, bxL, bcy);

        item.arrowRight.setFont(m_font);
        item.arrowRight.setString(">");
        item.arrowRight.setCharacterSize(static_cast<unsigned>(m_H * 0.028f));
        item.arrowRight.setFillColor(Pal::Gold);
        centerText(item.arrowRight, bxR, bcy);

        item.valueText.setFont(m_font);
        item.valueText.setString(std::to_string(item.value));
        item.valueText.setCharacterSize(static_cast<unsigned>(m_H * 0.026f));
        item.valueText.setFillColor(sf::Color(255, 255, 255, 255)); // ขาวสว่าง
        centerText(item.valueText, cx, rowCy + m_H * 0.048f);

        m_settingItems.push_back(std::move(item));
    }
    m_settingsBackBtn = makeBackBtn(m_font, m_setPX, m_setPY, m_setPW, m_setPH, m_H);
}

// ── buildCreditsUI ───────────────────────────────────────────────
void MainMenu::buildCreditsUI() {
    m_crePW = m_W * 0.52f; m_crePH = m_H * 0.90f;
    m_crePX = (m_W - m_crePW) / 2.f; m_crePY = (m_H - m_crePH) / 2.f;

    m_creditsTitle.setFont(m_font);
    m_creditsTitle.setString("CREDITS");
    m_creditsTitle.setCharacterSize(static_cast<unsigned>(m_H * 0.058f));
    m_creditsTitle.setLetterSpacing(3.f);
    m_creditsTitle.setFillColor(Pal::TitleNormal);
    centerText(m_creditsTitle, m_crePX + m_crePW / 2.f, m_crePY + m_H * 0.068f);

    m_creditsDivider.setSize({ m_crePW * 0.7f,1.f });
    m_creditsDivider.setOrigin(m_crePW * 0.35f, 0.5f);
    m_creditsDivider.setPosition(m_crePX + m_crePW / 2.f, m_crePY + m_H * 0.115f);
    m_creditsDivider.setFillColor(Pal::GoldDim);

    m_creditsSubtitle.setFont(m_font);
    m_creditsSubtitle.setString("~ Those who built this world ~");
    m_creditsSubtitle.setCharacterSize(static_cast<unsigned>(m_H * 0.022f));
    m_creditsSubtitle.setStyle(sf::Text::Italic);
    m_creditsSubtitle.setFillColor(Pal::Subtitle);
    centerText(m_creditsSubtitle, m_crePX + m_crePW / 2.f, m_crePY + m_H * 0.135f);

    struct CL { std::string t; bool role; };
    const std::vector<CL> lines = {
        {"",true},
        {"PROJECT TEAM",true},{"",true},
                              {"Nithit Boonlert 680610763",false},{"",true},
                              {"Nipasiri Saephoei 680610764",false},{"",true},
                              {"Paniti Lertvijitsub 680610766",false},{"",true},
                              {"Piyarad thammachaisu 680610769",false},{"",true},
                              {"Ratchaphum Sootthasil 680610787",false},{"",true},
        {"SPECIAL THANKS",true},{"",true},
        {"Everyone who played",false},{"and gave feedback",false},{"Thank you for playing this game",false},{"",false},
    };
    m_creditsLines.clear();
    float cx = m_crePX + m_crePW / 2.f;
    float sy = m_crePY + m_H * 0.160f, lh = m_H * 0.034f;
    for (size_t i = 0; i < lines.size(); i++) {
        sf::Text t;
        t.setFont(m_font);
        t.setString(lines[i].t);
        if (lines[i].role) { t.setCharacterSize(static_cast<unsigned>(m_H * 0.026f)); t.setLetterSpacing(2.5f); t.setFillColor(Pal::Gold); }
        else { t.setCharacterSize(static_cast<unsigned>(m_H * 0.025f)); t.setLetterSpacing(2.5f); t.setFillColor(Pal::CreditName); }
        centerText(t, cx, sy + lh * i);
        m_creditsLines.push_back(std::move(t));
    }
    m_creditsBackBtn = makeBackBtn(m_font, m_crePX, m_crePY, m_crePW, m_crePH, m_H);
}

// ── handleEvent ──────────────────────────────────────────────────
void MainMenu::handleEvent(const sf::Event& e) {
    switch (m_subScreen) {
    case SubScreen::Main:     handleMainEvent(e);     break;
    case SubScreen::Settings: handleSettingsEvent(e); break;
    case SubScreen::Credits:  handleCreditsEvent(e);  break;
    }
}
void MainMenu::handleMainEvent(const sf::Event& e) {
    if (e.type == sf::Event::MouseMoved) {
        sf::Vector2f m((float)e.mouseMove.x, (float)e.mouseMove.y);
        for (auto& b : m_buttons) b.hovered = b.bounds.contains(m);
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f m((float)e.mouseButton.x, (float)e.mouseButton.y);
        for (auto& b : m_buttons) {
            if (!b.bounds.contains(m)) continue;
            if (b.action == MenuState::Settings) { m_subScreen = SubScreen::Settings; return; }
            if (b.action == MenuState::Credits) { m_subScreen = SubScreen::Credits; return; }
            m_state = b.action; return;
        }
    }
    if (e.type == sf::Event::KeyReleased && e.key.code == sf::Keyboard::Escape)
        m_state = MenuState::Exit;
}
void MainMenu::handleSettingsEvent(const sf::Event& e) {
    if (e.type == sf::Event::MouseMoved) {
        sf::Vector2f m((float)e.mouseMove.x, (float)e.mouseMove.y);
        m_settingsBackBtn.hovered = m_settingsBackBtn.bounds.contains(m);
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f m((float)e.mouseButton.x, (float)e.mouseButton.y);
        if (m_settingsBackBtn.bounds.contains(m)) { m_subScreen = SubScreen::Main; return; }
        for (size_t i = 0; i < m_settingItems.size(); i++) {
            auto& item = m_settingItems[i];
            if (item.btnLeft.getGlobalBounds().contains(m)) {
                item.value = std::max(item.minVal, item.value - item.step);
                updateSettingItems();
                if (i == 0)m_musicVol = item.value;
                if (i == 1)m_sfxVol = item.value;
                if (i == 2)m_brightness = item.value;
                return;
            }
            if (item.btnRight.getGlobalBounds().contains(m)) {
                item.value = std::min(item.maxVal, item.value + item.step);
                updateSettingItems();
                if (i == 0)m_musicVol = item.value;
                if (i == 1)m_sfxVol = item.value;
                if (i == 2)m_brightness = item.value;
                return;
            }
        }
    }
    if (e.type == sf::Event::KeyReleased && e.key.code == sf::Keyboard::Escape)
        m_subScreen = SubScreen::Main;
}
void MainMenu::handleCreditsEvent(const sf::Event& e) {
    if (e.type == sf::Event::MouseMoved) {
        sf::Vector2f m((float)e.mouseMove.x, (float)e.mouseMove.y);
        m_creditsBackBtn.hovered = m_creditsBackBtn.bounds.contains(m);
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f m((float)e.mouseButton.x, (float)e.mouseButton.y);
        if (m_creditsBackBtn.bounds.contains(m)) { m_subScreen = SubScreen::Main; return; }
    }
    if (e.type == sf::Event::KeyReleased && e.key.code == sf::Keyboard::Escape)
        m_subScreen = SubScreen::Main;
}

// ── updateSettingItems ───────────────────────────────────────────
void MainMenu::updateSettingItems() {
    for (auto& item : m_settingItems) {
        float tw = item.trackBar.getSize().x, th = item.trackBar.getSize().y;
        float fw = tw * item.value / 100.f;
        item.fillBar.setSize({ std::max(fw,2.f),th });
        item.valueText.setString(std::to_string(item.value));
        sf::FloatRect tb = item.trackBar.getGlobalBounds();
        centerText(item.valueText, tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    }
}

// ── updateButtons ────────────────────────────────────────────────
void MainMenu::updateButtons(float dt, std::vector<MenuButton>& btns) {
    for (auto& btn : btns) {
        float tgt = btn.hovered ? 1.f : 0.f;
        btn.hoverAnim = lerpF(btn.hoverAnim, tgt, dt * 10.f);
        btn.label.setFillColor(lerpColor(Pal::BtnNormal, Pal::BtnHover, btn.hoverAnim));
        float s = 1.f + 0.04f * btn.hoverAnim;
        btn.label.setScale(s, s);
    }
}

// ── update ───────────────────────────────────────────────────────
void MainMenu::update(float dt) {
    if (m_videoLoaded && !m_videoFrames.empty()) {
        m_frameTimer += dt;
        if (m_frameTimer >= m_frameDuration) {
            m_frameTimer = 0.f;
            m_currentFrame = (m_currentFrame + 1) % (int)m_videoFrames.size();
            m_videoSprite.setTexture(m_videoFrames[m_currentFrame]);
        }
    }
    switch (m_subScreen) {
    case SubScreen::Main: updateButtons(dt, m_buttons); break;
    case SubScreen::Settings: {
        float tgt = m_settingsBackBtn.hovered ? 1.f : 0.f;
        m_settingsBackBtn.hoverAnim = lerpF(m_settingsBackBtn.hoverAnim, tgt, dt * 10.f);
        m_settingsBackBtn.label.setFillColor(lerpColor(Pal::BtnNormal, Pal::BtnHover, m_settingsBackBtn.hoverAnim));
        m_settingsBackBtn.label.setScale(1.f + 0.04f * m_settingsBackBtn.hoverAnim, 1.f + 0.04f * m_settingsBackBtn.hoverAnim);
        break;
    }
    case SubScreen::Credits: {
        float tgt = m_creditsBackBtn.hovered ? 1.f : 0.f;
        m_creditsBackBtn.hoverAnim = lerpF(m_creditsBackBtn.hoverAnim, tgt, dt * 10.f);
        m_creditsBackBtn.label.setFillColor(lerpColor(Pal::BtnNormal, Pal::BtnHover, m_creditsBackBtn.hoverAnim));
        m_creditsBackBtn.label.setScale(1.f + 0.04f * m_creditsBackBtn.hoverAnim, 1.f + 0.04f * m_creditsBackBtn.hoverAnim);
        break;
    }
    }
}

// ── draw ─────────────────────────────────────────────────────────
void MainMenu::drawBackground() {
    if (m_videoLoaded)     m_window.draw(m_videoSprite);
    else if (m_bgLoaded)   m_window.draw(m_bgSprite);
    else                  m_window.draw(m_bgFallback);
}

void MainMenu::drawMain() {
    drawRR(m_window, m_mainPX, m_mainPY, m_mainPW, m_mainPH, 16.f, Pal::PanelFill, Pal::PanelBorder);
    for (const auto& s : m_separators) m_window.draw(s);
    for (const auto& b : m_buttons)    m_window.draw(b.label);
    sf::Text sh = m_titleText; sh.setFillColor(Pal::TitleShadow); sh.move(3.f, 3.f);
    m_window.draw(sh);
    m_window.draw(m_titleText);
}

void MainMenu::drawSettings() {
    drawRR(m_window, m_setPX, m_setPY, m_setPW, m_setPH, 16.f, Pal::PanelFill, Pal::PanelBorder);
    m_window.draw(m_settingsDivider);
    sf::Text sh = m_settingsTitle; sh.setFillColor(Pal::TitleShadow); sh.move(2.f, 2.f);
    m_window.draw(sh); m_window.draw(m_settingsTitle); m_window.draw(m_settingsSubtitle);
    for (const auto& item : m_settingItems) {
        m_window.draw(item.labelText); m_window.draw(item.trackBar);
        m_window.draw(item.fillBar);   m_window.draw(item.btnLeft);
        m_window.draw(item.btnRight);  m_window.draw(item.arrowLeft);
        m_window.draw(item.arrowRight); m_window.draw(item.valueText);
    }
    sf::RectangleShape sep({ m_setPW * 0.75f,1.f });
    sep.setOrigin(m_setPW * 0.375f, 0.5f);
    sep.setPosition(m_setPX + m_setPW / 2.f, m_settingsBackBtn.bounds.top - m_H * 0.035f);
    sep.setFillColor(Pal::SepColor); m_window.draw(sep);
    m_window.draw(m_settingsBackBtn.label);
}

void MainMenu::drawCredits() {
    drawRR(m_window, m_crePX, m_crePY, m_crePW, m_crePH, 16.f, Pal::PanelFill, Pal::PanelBorder);
    m_window.draw(m_creditsDivider);
    sf::Text sh = m_creditsTitle; sh.setFillColor(Pal::TitleShadow); sh.move(2.f, 2.f);
    m_window.draw(sh); m_window.draw(m_creditsTitle); m_window.draw(m_creditsSubtitle);
    for (const auto& l : m_creditsLines) m_window.draw(l);
    sf::RectangleShape sep({ m_crePW * 0.75f,1.f });
    sep.setOrigin(m_crePW * 0.375f, 0.5f);
    sep.setPosition(m_crePX + m_crePW / 2.f, m_creditsBackBtn.bounds.top - m_H * 0.035f);
    sep.setFillColor(Pal::SepColor); m_window.draw(sep);
    m_window.draw(m_creditsBackBtn.label);
}

void MainMenu::draw() {
    drawBackground();
    switch (m_subScreen) {
    case SubScreen::Main:     drawMain();     break;
    case SubScreen::Settings: drawSettings(); break;
    case SubScreen::Credits:  drawCredits();  break;
    }
}