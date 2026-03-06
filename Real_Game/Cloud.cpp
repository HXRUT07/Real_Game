#include "Cloud.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

static float randFloat(float lo, float hi) {
    return lo + (hi - lo) * (std::rand() / (float)RAND_MAX);
}

Cloud::Cloud(float screenW, float screenH)
    : screenWidth(screenW), screenHeight(screenH)
{
    reset(true);
}

void Cloud::reset(bool spawnOffscreen) {
    movingRight = (std::rand() % 2 == 0);
    speed = randFloat(8.f, 22.f);
    position.y = randFloat(30.f, screenHeight - 50.f);

    if (spawnOffscreen) {
        position.x = randFloat(-200.f, screenWidth + 200.f);
    }
    else {
        if (movingRight) position.x = -300.f;
        else             position.x = screenWidth + 300.f;
    }

    alpha = randFloat(35.f, 75.f);
    puffs.clear();

    int cloudType = std::rand() % 3;

    if (cloudType == 0) {
        // Cumulus - ก้อนใหญ่ฟูๆ
        int puffCount = 6 + std::rand() % 5;
        for (int i = 0; i < puffCount; ++i) {
            CloudPuff p;
            p.offset.x = randFloat(-60.f, 60.f);
            p.offset.y = randFloat(-20.f, 20.f);
            float distFromCenter = std::abs(p.offset.x) / 60.f;
            p.radius = randFloat(24.f, 40.f) * (1.f - distFromCenter * 0.5f);
            p.offset.x = std::floor(p.offset.x / 4.f) * 4.f;
            p.offset.y = std::floor(p.offset.y / 4.f) * 4.f;
            p.radius = std::floor(p.radius / 4.f) * 4.f;
            if (p.radius < 8.f) p.radius = 8.f;
            puffs.push_back(p);
        }
    }
    else if (cloudType == 1) {
        // Stratus - เมฆแผ่นยาวแนวนอน
        int puffCount = 8 + std::rand() % 6;
        for (int i = 0; i < puffCount; ++i) {
            CloudPuff p;
            p.offset.x = randFloat(-100.f, 100.f);
            p.offset.y = randFloat(-8.f, 8.f);
            p.radius = randFloat(10.f, 20.f);
            p.offset.x = std::floor(p.offset.x / 4.f) * 4.f;
            p.offset.y = std::floor(p.offset.y / 4.f) * 4.f;
            p.radius = std::floor(p.radius / 4.f) * 4.f;
            if (p.radius < 8.f) p.radius = 8.f;
            puffs.push_back(p);
        }
        alpha *= 0.7f;
    }
    else {
        // Cirrus - เมฆเส้นๆ บางมาก
        int puffCount = 10 + std::rand() % 8;
        for (int i = 0; i < puffCount; ++i) {
            CloudPuff p;
            p.offset.x = randFloat(-120.f, 120.f);
            p.offset.y = randFloat(-5.f, 5.f);
            p.radius = randFloat(8.f, 14.f);
            p.offset.x = std::floor(p.offset.x / 4.f) * 4.f;
            p.offset.y = std::floor(p.offset.y / 4.f) * 4.f;
            p.radius = std::floor(p.radius / 4.f) * 4.f;
            if (p.radius < 8.f) p.radius = 8.f;
            puffs.push_back(p);
        }
        alpha *= 0.5f;
    }
}

void Cloud::update(float dt) {
    if (movingRight) position.x += speed * dt;
    else             position.x -= speed * dt;
}

bool Cloud::isOffscreen() const {
    if (movingRight) return position.x > screenWidth + 250.f;
    else             return position.x < -250.f;
}

void Cloud::draw(sf::RenderWindow& window, float zoomLevel) {
    // เปรมทำ - ยิ่งซูมออก puff ยิ่งใหญ่และทึบขึ้น เหมือนมองจากเครื่องบิน
    float zoomScale = 1.f;
    float zoomAlpha = 1.f;

    if (zoomLevel <= 1.5f) {
        zoomScale = 1.f;
        zoomAlpha = 0.6f;
    }
    else if (zoomLevel <= 3.f) {
        float t = (zoomLevel - 1.5f) / 1.5f; // 0->1
        zoomScale = 1.f + t * 2.5f;           // 1x -> 3.5x
        zoomAlpha = 0.6f + t * 0.4f;          // 0.6 -> 1.0
    }
    else {
        float t = std::min(1.f, (zoomLevel - 3.f) / 3.f); // 0->1
        zoomScale = 3.5f + t * 4.f;           // 3.5x -> 7.5x ใหญ่มากเหมือนเมฆจากเครื่องบิน
        zoomAlpha = 1.f;
    }

    for (const auto& p : puffs) {
        sf::RectangleShape rect;
        float size = p.radius * zoomScale;
        rect.setSize(sf::Vector2f(size, size));
        rect.setOrigin(size / 2.f, size / 2.f);
        rect.setPosition(position.x + p.offset.x, position.y + p.offset.y);

        sf::Uint8 a = (sf::Uint8)std::min(255.f, alpha * zoomAlpha * 2.5f);
        rect.setFillColor(sf::Color(220, 230, 255, a));
        window.draw(rect);
    }
    // เปรมทำ - จบ
}

// --------------------------------------------------------
// CloudSystem
// --------------------------------------------------------
CloudSystem::CloudSystem(float screenW, float screenH, int count)
    : screenWidth(screenW), screenHeight(screenH)
{
    // เปรมทำ - สร้างเมฆ 45 ก้อน (1.5x จากเดิม 30)
    for (int i = 0; i < 45; ++i) {
        clouds.emplace_back(screenW, screenH);
    }
}

void CloudSystem::update(float dt) {
    for (auto& c : clouds) {
        c.update(dt);
        if (c.isOffscreen()) c.reset(false);
    }
}

void CloudSystem::draw(sf::RenderWindow& window, float zoomLevel) {
    // เปรมทำ - คำนวณจำนวนเมฆที่โชว์ตาม zoom
    // ยิ่งซูมออก ยิ่งโชว์เมฆเยอะขึ้น จนบังมิดเหมือนมองจากเครื่องบิน
    int cloudsToShow = 0;

    if (zoomLevel <= 1.0f) {
        cloudsToShow = 0;
    }
    else if (zoomLevel <= 2.0f) {
        float t = (zoomLevel - 1.0f) / 1.0f;
        cloudsToShow = (int)(t * 10.f);        // 0->10
    }
    else if (zoomLevel <= 3.5f) {
        float t = (zoomLevel - 2.0f) / 1.5f;
        cloudsToShow = 10 + (int)(t * 20.f);   // 10->30
    }
    else {
        float t = std::min(1.f, (zoomLevel - 3.5f) / 2.0f);
        cloudsToShow = 30 + (int)(t * 15.f);   // 30->45 บังมิด
    }

    cloudsToShow = std::min(cloudsToShow, (int)clouds.size());

    for (int i = 0; i < cloudsToShow; ++i) {
        clouds[i].draw(window, zoomLevel);
    }
    // เปรมทำ - จบ
}