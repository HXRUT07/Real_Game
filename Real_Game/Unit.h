#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class Unit {
public:
    Unit(std::string name, int startR, int startC, int owner);
    int getOwner() const { return m_owner; }
    void draw(sf::RenderWindow& window, int stackCount = 1);
    void moveTo(int r, int c);
    bool hasAP() const { return m_currentAP > 0; }
    void consumeAP(int cost) { m_currentAP -= cost; }
    void resetAP() { m_currentAP = m_maxAP; }
    int getMoveRange() const { return m_moveRange; }
    int getSightRange() const { return m_sightRange; }
    int getR() const { return m_gridR; }
    int getC() const { return m_gridC; }
    sf::Vector2f getPosition() const { return m_pos; }
    std::string getName() const { return m_name; }
    int getCurrentAP() const { return m_currentAP; }
    int getMaxAP() const { return m_maxAP; }
    bool isClicked(sf::Vector2f mousePos);
    static bool loadTextures();

    // เปรมทำ - ต้องมี move constructor เพราะ RenderTexture ใน unique_ptr
    Unit(Unit&&) = default;
    Unit& operator=(Unit&&) = default;
    Unit(const Unit&) = delete;
    Unit& operator=(const Unit&) = delete;
    // เปรมทำ - จบ

private:
    int m_owner;
    std::string m_name;
    int m_gridR, m_gridC;
    sf::Vector2f m_pos;
    int m_maxAP = 2;
    int m_currentAP = 2;
    int m_moveRange = 1;
    int m_sightRange = 1;

    sf::CircleShape m_circle;

    // เปรมทำ - ใช้ unique_ptr เพื่อให้ move ได้
    std::unique_ptr<sf::RenderTexture> m_hexTex;
    std::unique_ptr<sf::Sprite>        m_hexSprite;
    bool m_spriteReady = false;
    int  m_lastTexIdx = -1;
    void rebuildHexSprite(sf::Texture* tex, int texIdx);
    // เปรมทำ - จบ

    static sf::Texture s_texKnight;
    static sf::Texture s_texArmy;
    static sf::Texture s_texGoblin;
    static sf::Texture s_texGoblinArmy;
    static bool s_texturesLoaded;

    void updateScreenPosition();
};