#include "Cloud.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

// เปรมทำ - ระบบก้อนเมฆ pixel
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

    // เปรมทำ - สุ่มชนิดเมฆตามธรรมชาติ
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
        // Stratus - เมฆแผ่นยาวแนวนอน บางๆ
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
        // Cirrus - เมฆเส้นๆ บางมาก อยู่สูง
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
    // เปรมทำ - จบ
}

void Cloud::update(float dt) {
    if (movingRight)
        position.x += speed * dt;
    else
        position.x -= speed * dt;
}

bool Cloud::isOffscreen() const {
    if (movingRight) return position.x > screenWidth + 250.f;
    else             return position.x < -250.f;
}

void Cloud::draw(sf::RenderWindow& window, float zoomLevel) {
    // เปรมทำ - CloudSystem จัดการ zoom แล้ว วาดตรงๆ ได้เลย
    for (const auto& p : puffs) {
        sf::RectangleShape rect;
        float size = p.radius;
        rect.setSize(sf::Vector2f(size, size));
        rect.setOrigin(size / 2.f, size / 2.f);
        rect.setPosition(position.x + p.offset.x, position.y + p.offset.y);
        rect.setFillColor(sf::Color(220, 230, 255, (sf::Uint8)alpha));
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
    // เปรมทำ - สร้างเมฆสูงสุด 30 ก้อนไว้ก่อน
    for (int i = 0; i < 30; ++i) {
        clouds.emplace_back(screenW, screenH);
    }
}

void CloudSystem::update(float dt) {
    for (auto& c : clouds) {
        c.update(dt);
        if (c.isOffscreen()) {
            c.reset(false);
        }
    }
}

void CloudSystem::draw(sf::RenderWindow& window, float zoomLevel) {
    // เปรมทำ - คำนวณจำนวนเมฆตามระดับซูม
    // zoomLevel <= 1.0 = ไม่โชว์
    // zoomLevel 1.0-2.0 = 0-8 ก้อน
    // zoomLevel 2.0-4.0 = 8-20 ก้อน
    // zoomLevel 4.0+    = 20-30 ก้อน หนาแน่นมาก

    int cloudsToShow = 0;

    if (zoomLevel <= 1.0f) {
        cloudsToShow = 0;
    }
    else if (zoomLevel <= 2.0f) {
        float t = (zoomLevel - 1.0f) / 1.0f; // 0->1
        cloudsToShow = (int)(t * 8.f);
    }
    else if (zoomLevel <= 4.0f) {
        float t = (zoomLevel - 2.0f) / 2.0f; // 0->1
        cloudsToShow = 8 + (int)(t * 12.f);
    }
    else {
        float t = std::min(1.f, (zoomLevel - 4.0f) / 2.0f); // 0->1
        cloudsToShow = 20 + (int)(t * 10.f);
    }

    cloudsToShow = std::min(cloudsToShow, (int)clouds.size());

    // วาดเฉพาะจำนวนที่คำนวณได้
    for (int i = 0; i < cloudsToShow; ++i) {
        clouds[i].draw(window, zoomLevel);
    }
    // เปรมทำ - จบ
}