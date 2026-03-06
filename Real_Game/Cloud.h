#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

struct CloudPuff {
    sf::Vector2f offset;
    float radius = 0.f;
};

class Cloud {
private:
    sf::Vector2f position;
    float speed = 0.f;
    float alpha = 0.f;
    bool movingRight = false;
    float screenWidth = 0.f;
    float screenHeight = 0.f;
    std::vector<CloudPuff> puffs;

public:
    Cloud(float screenW, float screenH);
    void reset(bool spawnOffscreen);
    void update(float dt);
    void draw(sf::RenderWindow& window, float zoomLevel);
    bool isOffscreen() const;
};

class CloudSystem {
private:
    std::vector<Cloud> clouds;
    float screenWidth = 0.f;
    float screenHeight = 0.f;

public:
    CloudSystem(float screenW, float screenH, int count = 6);
    void update(float dt);
    void draw(sf::RenderWindow& window, float zoomLevel);
};