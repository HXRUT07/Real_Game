#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// -------------------------------------------------------
//  MenuState : ผลลัพธ์ที่ MainMenu ส่งกลับให้ main.cpp
// -------------------------------------------------------
enum class MenuState
{
    None,       // ยังอยู่ที่เมนู (ไม่ได้กดอะไร)
    Play,       // กด PLAY
    Settings,   // กด SETTINGS
    Credits,    // กด CREDITS
    Exit        // กด EXIT
};

// -------------------------------------------------------
//  MenuButton — ข้อมูลปุ่มแต่ละอัน
// -------------------------------------------------------
struct MenuButton
{
    sf::Text      label;
    sf::FloatRect bounds;       // bounding box สำหรับ hit-test
    MenuState     action = MenuState::None;
    bool          hovered = false;
    float         hoverAnim = 0.f;   // 0.0 → 1.0  (ใช้ lerp)
};

// -------------------------------------------------------
//  MainMenu
// -------------------------------------------------------
class MainMenu
{
public:
    // ============================================================
    //  Constructor
    //  window      – หน้าต่างหลักที่ main.cpp สร้างไว้
    //  bgImagePath – path รูปพื้นหลัง  (เช่น "assets/bg.png")
    //  fontPath    – path ฟอนต์ .ttf   (เช่น "assets/fonts/Trajan.ttf")
    //
    //  หากโหลดรูปไม่ได้ จะใช้พื้นหลังสีทึบแทน
    //  หากโหลด font ไม่ได้ จะ fallback เป็น default
    // ============================================================
    explicit MainMenu(sf::RenderWindow& window,
        const std::string& bgImagePath = "assets/background.png",
        const std::string& fontPath = "assets/fonts/Trajan Pro Regular.ttf");

    ~MainMenu() = default;

    // ส่ง SFML Event เข้ามา (เรียกใน event-loop ของ main.cpp)
    void handleEvent(const sf::Event& event);

    // อัปเดต animation  –  dt คือ delta time (วินาที)
    void update(float dt);

    // วาดทุกอย่างลง window
    void draw();

    // ผลลัพธ์ที่ผู้ใช้เลือก — ตรวจหลัง update() ทุกเฟรม
    MenuState getState() const { return m_state; }

    // รีเซ็ตกลับ None เมื่อต้องการแสดงเมนูอีกครั้ง
    void resetState() { m_state = MenuState::None; }

private:
    void buildUI();
    void rebuildButtonBounds();

    // --- refs & owned ---
    sf::RenderWindow& m_window;
    sf::Font          m_font;

    // Background
    sf::Texture        m_bgTexture;
    sf::Sprite         m_bgSprite;
    bool               m_bgLoaded = false;
    sf::RectangleShape m_bgFallback;   // สีทึบถ้าโหลดไม่ได้

    // Dark panel (กล่องด้านขวา)
    sf::RectangleShape m_panel;
    sf::RectangleShape m_panelBorder;

    // Title text  (ชื่อเกมมุมบนขวา)
    sf::Text m_titleText;

    // Buttons
    std::vector<MenuButton> m_buttons;

    // Thin separator lines ระหว่างปุ่ม
    std::vector<sf::RectangleShape> m_separators;

    // State
    MenuState m_state = MenuState::None;

    // Cached size
    float m_W = 0.f, m_H = 0.f;
};