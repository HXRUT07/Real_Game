#include "City.h"
#include <sstream>

City::City(int r, int c, sf::Vector2f pos, ResourceYield res)
    : gridR(r), gridC(c), center(pos), baseResource(res)
{
    name = "New City";

    baseIcon.setSize(sf::Vector2f(12.f, 12.f));
    baseIcon.setFillColor(sf::Color::Yellow);
    baseIcon.setOrigin(6.f, 6.f);
    baseIcon.setPosition(center);
}

void City::draw(sf::RenderWindow& window)
{
    window.draw(baseIcon);
}

sf::FloatRect City::getBounds() const
{
    return baseIcon.getGlobalBounds();
}

ResourceYield City::getTotalResource() const
{
    ResourceYield total = baseResource;

    total.wood += 100;
    total.gold += 100;
    total.food += 100;

    return total;
}

std::string City::getCityInfo() const
{
    std::stringstream ss;
    ResourceYield r = getTotalResource();

    ss << "City Name: " << name << "\n\n";
    ss << "Wood: " << r.wood << "\n";
    ss << "Gold: " << r.gold << "\n";
    ss << "Food: " << r.food << "\n\n";
    ss << "Upgraded: " << (upgraded ? "Yes" : "No");

    return ss.str();
}