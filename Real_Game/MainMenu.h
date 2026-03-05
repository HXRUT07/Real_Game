#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class MenuState { None, Play, Settings, Credits, Exit };
enum class SubScreen { Main, Settings, Credits };

struct MenuButton
{
    sf::Text      label;
    sf::FloatRect bounds;
    MenuState     action = MenuState::None;
    bool          hovered = false;
    float         hoverAnim = 0.f;
};

struct SettingItem
{
    std::string name;
    int value = 50, minVal = 0, maxVal = 100, step = 10;
    sf::Text           labelText, valueText, arrowLeft, arrowRight;
    sf::RectangleShape trackBar, fillBar, btnLeft, btnRight;
};

class MainMenu
{
public:
    explicit MainMenu(sf::RenderWindow& window,
        const std::string& bgImagePath = "assets/background.png",
        const std::string& fontPath = "assets/fonts/Trajan Pro Regular.ttf");
    ~MainMenu() = default;

    void handleEvent(const sf::Event& event);
    void update(float dt);
    void draw();
    void loadVideoFrames(const std::string& folderPath, int frameCount);

    MenuState getState()   const { return m_state; }
    void      resetState() { m_state = MenuState::None; }

    int getMusicVolume() const { return m_musicVol; }
    int getSfxVolume()   const { return m_sfxVol; }
    int getBrightness()  const { return m_brightness; }

private:
    // build
    void buildMainUI();
    void buildSettingsUI();
    void buildCreditsUI();
    // draw
    void drawBackground();
    void drawMain();
    void drawSettings();
    void drawCredits();
    // event
    void handleMainEvent(const sf::Event&);
    void handleSettingsEvent(const sf::Event&);
    void handleCreditsEvent(const sf::Event&);
    // update
    void updateButtons(float dt, std::vector<MenuButton>& btns);
    void updateSettingItems();

    // ── shared ──────────────────────────────────────────
    sf::RenderWindow& m_window;
    sf::Font          m_font;
    float             m_W = 0, m_H = 0;
    MenuState         m_state = MenuState::None;
    SubScreen         m_subScreen = SubScreen::Main;

    // ── background ──────────────────────────────────────
    sf::Texture        m_bgTexture;
    sf::Sprite         m_bgSprite;
    bool               m_bgLoaded = false;
    sf::RectangleShape m_bgFallback;

    std::vector<sf::Texture> m_videoFrames;
    sf::Sprite               m_videoSprite;
    int   m_currentFrame = 0;
    float m_frameTimer = 0.f;
    const float m_frameDuration = 1.f / 30.f;
    bool  m_videoLoaded = false;

    // ── main menu panel (geometry only, no RectangleShape) ──
    float m_mainPX = 0, m_mainPY = 0, m_mainPW = 0, m_mainPH = 0;
    sf::Text                        m_titleText;
    std::vector<MenuButton>         m_buttons;
    std::vector<sf::RectangleShape> m_separators;

    // ── settings ────────────────────────────────────────
    float m_setPX = 0, m_setPY = 0, m_setPW = 0, m_setPH = 0;
    sf::Text                 m_settingsTitle, m_settingsSubtitle;
    sf::RectangleShape       m_settingsDivider;
    std::vector<SettingItem> m_settingItems;
    MenuButton               m_settingsBackBtn;
    int m_musicVol = 70, m_sfxVol = 80, m_brightness = 100;

    // ── credits ─────────────────────────────────────────
    float m_crePX = 0, m_crePY = 0, m_crePW = 0, m_crePH = 0;
    sf::Text              m_creditsTitle, m_creditsSubtitle;
    sf::RectangleShape    m_creditsDivider;
    std::vector<sf::Text> m_creditsLines;
    MenuButton            m_creditsBackBtn;
};