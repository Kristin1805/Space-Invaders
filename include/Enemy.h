#ifndef ENEMY_H
#define ENEMY_H
#pragma once
#include "GameObject.h"

class Enemy : public GameObject {
private:
    int direction;

public:
    Enemy();  // конструктор по подразбиране
    Enemy(int x, int y, char symbol, COLORS color, int direction); // конструктор с параметри
    Enemy(const Enemy& other);  // копиращ конструктор
    Enemy(Enemy&& other) noexcept; // преместващ конструктор
    Enemy& operator=(const Enemy& other); // присвояване

    ~Enemy(); // деструктор

    void update() override; // променя позицията на врага
    void render() const override; // "нарисува" енеми на екрана

    int getDirection() const;  // гетър за посоката.
    void setDirection(int dir); // сетър за посоката
};

#endif //ENEMY_H
