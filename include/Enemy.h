#ifndef ENEMY_H
#define ENEMY_H
#pragma once
#include "GameObject.h"

class Enemy : public GameObject {
private:
    int direction;      // +1 за надясно, -1 за наляво
    bool moveDown;      // дали врагът трябва да слезе надолу

public:
    Enemy();  // конструктор по подразбиране
    Enemy(int x, int y, char symbol, COLORS color, int direction); // конструктор с параметри
    Enemy(const Enemy& other);  // копиращ конструктор
    Enemy(Enemy&& other) noexcept; // преместващ конструктор
    Enemy& operator=(const Enemy& other); // присвояване

    ~Enemy(); // Унищожава

    void update(int screenWidth) override;  // движение по хоризонтала + слизане надолу
    void render() const override; // "нарисува" енеми на екрана

    int getDirection() const;  // гетър за посоката.
    void setDirection(int dir); // сетър за посоката
    void setMoveDown(bool value); // сетър за moveDown
};

#endif //ENEMY_H