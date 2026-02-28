#include "MainMenu.h"
#include <cmath>
#include <algorithm>

// ================================================================
//  Color palette  –  ดึงจาก ref: มืด, ขาวครีม, ขอบทอง/เทา
// ================================================================
namespace Pal
{
    // Panel
    const sf::Color PanelFill{ 18,  14,  10, 210 };  // กล่องดำอมน้ำตาลโปร่งแสง
    const sf::Color PanelBorder{ 160, 145, 115, 180 };  // ขอบสีครีมอ่อน

    // Title
    const sf::Color TitleNormal{ 240, 230, 200, 255 };  // ขาวอมครีม
    const sf::Color TitleShadow{ 0,   0,   0, 160 };

    // Button text
    const sf::Color BtnNormal{ 230, 220, 195, 255 };  // ขาวครีม
    const sf::Color BtnHover{ 255, 255, 255, 255 };  // ขาวสว่างเมื่อ hover

    // Separator
    const sf::Color SepColor{ 160, 145, 115,  80 };

    // Fallback BG
    const sf::Color BgFallback{ 40,  32,  22, 255 };
}

// ----------------------------------------------------------------
//  Utility
// ----------------------------------------------------------------
static float lerpF(float a, float b, float t) { return a + (b - a) * t; }

static sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t)
{
    auto c = [](float x) { return static_cast<sf::Uint8>(std::clamp(x, 0.f, 255.f)); };
    return { c(lerpF(a.r, b.r, t)),
             c(lerpF(a.g, b.g, t)),
             c(lerpF(a.b, b.b, t)),
             c(lerpF(a.a, b.a, t)) };
}

// ================================================================
//  Constructor
// ================================================================
MainMenu::MainMenu(sf::RenderWindow& window,
    const std::string& bgImagePath,
    const std::string& fontPath)
    : m_window(window)
{
    m_W = static_cast<float>(window.getSize().x);
    m_H = static_cast<float>(window.getSize().y);

    // ---- Font ------------------------------------------------
    if (!m_font.loadFromFile(fontPath))
    {
        // fallback paths
        for (const auto& fb : {
                "assets/fonts/CinzelDecorative-Regular.ttf",
                "assets/fonts/Cinzel-Regular.ttf",
                "C:/Windows/Fonts/times.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf" })
                if (m_font.loadFromFile(fb)) break;
    }

    // ---- Background ------------------------------------------
    if (m_bgTexture.loadFromFile(bgImagePath))
    {
        m_bgLoaded = true;
        m_bgSprite.setTexture(m_bgTexture);

        // Scale-to-fill: คำนวณ scale ให้คลุมทั้งหน้าจอ
        float scaleX = m_W / static_cast<float>(m_bgTexture.getSize().x);
        float scaleY = m_H / static_cast<float>(m_bgTexture.getSize().y);
        float scale = std::max(scaleX, scaleY);
        m_bgSprite.setScale(scale, scale);

        // Center
        float sw = m_bgTexture.getSize().x * scale;
        float sh = m_bgTexture.getSize().y * scale;
        m_bgSprite.setPosition((m_W - sw) / 2.f, (m_H - sh) / 2.f);
    }
    else
    {
        m_bgFallback.setSize({ m_W, m_H });
        m_bgFallback.setFillColor(Pal::BgFallback);
    }

    buildUI();
}

// ================================================================
//  buildUI  –  สร้าง panel + title + buttons ตาม ref
// ================================================================
void MainMenu::buildUI()
{
    // ============================================================
    //  Panel  (กล่องด้านขวา  ~38% ของความกว้างหน้าจอ)
    // ============================================================
    const float panelW = m_W * 0.38f;
    const float panelH = m_H * 0.62f;
    const float panelX = m_W - panelW - m_W * 0.04f;   // ชิดขวา เว้น margin
    const float panelY = (m_H - panelH) / 2.f;

    m_panel.setSize({ panelW, panelH });
    m_panel.setPosition(panelX, panelY);
    m_panel.setFillColor(Pal::PanelFill);

    m_panelBorder.setSize({ panelW, panelH });
    m_panelBorder.setPosition(panelX, panelY);
    m_panelBorder.setFillColor(sf::Color::Transparent);
    m_panelBorder.setOutlineColor(Pal::PanelBorder);
    m_panelBorder.setOutlineThickness(1.5f);

    // ============================================================
    //  Title  (ชื่อเกมมุมบนขวา — เหนือ panel)
    // ============================================================
    m_titleText.setFont(m_font);
    m_titleText.setString("SARVIER");        // <-- เปลี่ยนชื่อเกมได้ที่นี่
    m_titleText.setCharacterSize(static_cast<unsigned>(m_H * 0.085f));
    m_titleText.setLetterSpacing(1.8f);
    m_titleText.setFillColor(Pal::TitleNormal);

    {
        sf::FloatRect tb = m_titleText.getLocalBounds();
        // จัดให้อยู่เหนือ panel ชิดขวา
        float tx = panelX + panelW - tb.width - tb.left;
        float ty = panelY - tb.height - tb.top - m_H * 0.04f;
        m_titleText.setPosition(tx, ty);
    }

    // ============================================================
    //  Buttons  (4 ปุ่มในแนวตั้ง กระจายใน panel)
    // ============================================================
    struct BtnDef { std::string label; MenuState action; };
    const std::vector<BtnDef> defs = {
        { "PLAY",     MenuState::Play     },
        { "SETTINGS", MenuState::Settings },
        { "CREDITS",  MenuState::Credits  },
        { "EXIT",     MenuState::Exit     }
    };

    const unsigned fontSize = static_cast<unsigned>(m_H * 0.055f);
    const float    slotH = panelH / static_cast<float>(defs.size());
    const float    panelCx = panelX + panelW / 2.f;

    m_buttons.clear();
    m_separators.clear();

    for (size_t i = 0; i < defs.size(); ++i)
    {
        MenuButton btn;
        btn.action = defs[i].action;

        btn.label.setFont(m_font);
        btn.label.setString(defs[i].label);
        btn.label.setCharacterSize(fontSize);
        btn.label.setLetterSpacing(2.5f);
        btn.label.setFillColor(Pal::BtnNormal);

        // ตำแหน่ง: กึ่งกลาง slot แนวตั้ง
        float slotCy = panelY + slotH * i + slotH / 2.f;
        sf::FloatRect lb = btn.label.getLocalBounds();
        btn.label.setOrigin(lb.left + lb.width / 2.f,
            lb.top + lb.height / 2.f);
        btn.label.setPosition(panelCx, slotCy);

        // hit-test bounds (ทั้งแนว slot)
        btn.bounds = { panelX, panelY + slotH * i, panelW, slotH };

        m_buttons.push_back(std::move(btn));

        // separator ระหว่างปุ่ม (ไม่ต้องมีหลังปุ่มสุดท้าย)
        if (i < defs.size() - 1)
        {
            sf::RectangleShape sep({ panelW * 0.75f, 1.f });
            sep.setOrigin(panelW * 0.75f / 2.f, 0.5f);
            sep.setPosition(panelCx, panelY + slotH * (i + 1));
            sep.setFillColor(Pal::SepColor);
            m_separators.push_back(sep);
        }
    }
}

// ================================================================
//  handleEvent
// ================================================================
void MainMenu::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse(static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y));
        for (auto& btn : m_buttons)
            btn.hovered = btn.bounds.contains(mouse);
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse(static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y));
        for (auto& btn : m_buttons)
            if (btn.bounds.contains(mouse))
            {
                m_state = btn.action;
                return;
            }
    }

    // ESC = Exit
    if (event.type == sf::Event::KeyReleased &&
        event.key.code == sf::Keyboard::Escape)
        m_state = MenuState::Exit;
}

// ================================================================
//  update
// ================================================================
void MainMenu::update(float dt)
{
    for (auto& btn : m_buttons)
    {
        float target = btn.hovered ? 1.f : 0.f;
        btn.hoverAnim = lerpF(btn.hoverAnim, target, dt * 10.f);

        // ตัวอักษร: สีขาวสว่าง + ขยายนิดเมื่อ hover
        btn.label.setFillColor(lerpColor(Pal::BtnNormal, Pal::BtnHover, btn.hoverAnim));

        float s = 1.f + 0.04f * btn.hoverAnim;
        btn.label.setScale(s, s);
    }
}

// ================================================================
//  draw
// ================================================================
void MainMenu::draw()
{
    // 1. Background
    if (m_bgLoaded)
        m_window.draw(m_bgSprite);
    else
        m_window.draw(m_bgFallback);

    // 2. Dark panel
    m_window.draw(m_panel);
    m_window.draw(m_panelBorder);

    // 3. Separators
    for (const auto& sep : m_separators)
        m_window.draw(sep);

    // 4. Button labels
    for (const auto& btn : m_buttons)
        m_window.draw(btn.label);

    // 5. Title  (วาดทับบน panel เพื่อให้ชัด)
    //    วาด shadow ก่อน
    {
        sf::Text shadow = m_titleText;
        shadow.setFillColor(Pal::TitleShadow);
        shadow.move(3.f, 3.f);
        m_window.draw(shadow);
    }
    m_window.draw(m_titleText);
}