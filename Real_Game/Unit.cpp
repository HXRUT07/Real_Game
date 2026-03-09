#include "Unit.h"
#include <cmath>
#include <iostream>

const float HEX_SIZE_UNIT = 30.0f;
const float UNIT_RADIUS = 18.f;

sf::Texture Unit::s_texKnight;
sf::Texture Unit::s_texArmy;
sf::Texture Unit::s_texGoblin;
sf::Texture Unit::s_texGoblinArmy;

bool Unit::s_texturesLoaded = false;

bool Unit::loadTextures()
{
    if (s_texturesLoaded) return true;

    bool ok = true;

    if (!s_texKnight.loadFromFile("blueknight.png"))
    {
        std::cout << "ERROR: blueknight.png not found\n";
        ok = false;
    }

    if (!s_texArmy.loadFromFile("army.png"))
    {
        std::cout << "ERROR: army.png not found\n";
        ok = false;
    }

    if (!s_texGoblin.loadFromFile("redgoblin.png"))
    {
        std::cout << "ERROR: redgoblin.png not found\n";
        ok = false;
    }

    if (!s_texGoblinArmy.loadFromFile("armygoblin.png"))
    {
        std::cout << "ERROR: armygoblin.png not found\n";
        ok = false;
    }

    s_texKnight.setSmooth(true);
    s_texArmy.setSmooth(true);
    s_texGoblin.setSmooth(true);
    s_texGoblinArmy.setSmooth(true);

    s_texturesLoaded = ok;
    return ok;
}

Unit::Unit(std::string name, int startR, int startC, int owner)
{
    m_name = name;
    m_gridR = startR;
    m_gridC = startC;
    m_owner = owner;

    m_maxAP = 2;
    m_currentAP = m_maxAP;

    m_moveRange = 1;
    m_sightRange = 1;

    m_circle.setRadius(UNIT_RADIUS);
    m_circle.setOrigin(UNIT_RADIUS, UNIT_RADIUS);
    m_circle.setOutlineThickness(2.f);

    loadTextures();

    updateScreenPosition();
}

void Unit::moveTo(int r, int c)
{
    m_gridR = r;
    m_gridC = c;

    updateScreenPosition();
}

void Unit::draw(sf::RenderWindow& window, int stackCount)
{
    if (!s_texturesLoaded)
    {
        window.draw(m_circle);
        return;
    }

    sf::Texture* tex = nullptr;

    if (m_owner == 1)
    {
        if (stackCount >= 2)
            tex = &s_texArmy;
        else
            tex = &s_texKnight;
    }
    else
    {
        if (stackCount >= 2)
            tex = &s_texGoblinArmy;
        else
            tex = &s_texGoblin;
    }

    m_circle.setTexture(tex, true);

    sf::Vector2u texSize = tex->getSize();

    if (texSize.x > 0)
    {
        float diameter = UNIT_RADIUS * 2.f;

        float scale = diameter / std::min((float)texSize.x, (float)texSize.y);

        int cropSize = (int)(diameter / scale);

        int offsetX = (texSize.x - cropSize) / 2;
        int offsetY = (texSize.y - cropSize) / 2;

        m_circle.setTextureRect(sf::IntRect(offsetX, offsetY, cropSize, cropSize));
    }

    if (m_owner == 1)
    {
        m_circle.setFillColor(m_currentAP > 0
            ? sf::Color::White
            : sf::Color(180, 180, 180));

        m_circle.setOutlineColor(m_currentAP > 0
            ? sf::Color(255, 215, 0)
            : sf::Color(120, 120, 120));
    }
    else
    {
        m_circle.setFillColor(m_currentAP > 0
            ? sf::Color::White
            : sf::Color(150, 150, 150));

        m_circle.setOutlineColor(sf::Color::Red);
    }

    window.draw(m_circle);
}

bool Unit::isClicked(sf::Vector2f mousePos)
{
    float dx = mousePos.x - m_pos.x;
    float dy = mousePos.y - m_pos.y;

    return (dx * dx + dy * dy) <= (UNIT_RADIUS * UNIT_RADIUS);
}

void Unit::updateScreenPosition()
{
    float width = std::sqrt(3.0f) * HEX_SIZE_UNIT;
    float height = 2.0f * HEX_SIZE_UNIT;

    float horizDist = width;
    float vertDist = height * 0.75f;

    float x = m_gridC * horizDist + 50.0f;
    float y = m_gridR * vertDist + 50.0f;

    if (m_gridR % 2 != 0)
        x += width / 2.0f;

    m_pos = sf::Vector2f(x, y);

    m_circle.setPosition(x, y);
}