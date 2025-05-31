#include "Enemy.h"
#include <iostream>
using namespace std;


Enemy::Enemy() : GameObject(), direction(1), moveDown(false) {} // Създава нов враг празния конструктор на GameObject
// Задаваме direction = 1 (движи се надолу)

Enemy::Enemy(int x, int y, char symbol, COLORS color, int direction)
    : GameObject(x, y, symbol, color), direction(direction), moveDown(false) {} // Създава враг с конкретни данни , конструктора на
// GameObject с x, y, symbol, color.

Enemy::Enemy(const Enemy& other) : GameObject(other), direction(other.direction), moveDown(other.moveDown) {} // sъздава копие на друг враг

Enemy::Enemy(Enemy&& other) noexcept : GameObject(move(other)), direction(other.direction), moveDown(other.moveDown) {}

Enemy& Enemy::operator=(const Enemy& other) { // Присвояване враг1 = враг2
    if (this != &other) { // Проверяваме дали не са един и същи
        GameObject::operator=(other); // трябва да копираме всичко от GameObject
        direction = other.direction; // трябва да копираме direction (собствено поле на Enemy)
        moveDown = other.moveDown;
    }
    return *this;
}

Enemy::~Enemy() {} //  Унищожава

void Enemy::update(int screenWidth) {
    if (moveDown) {
        y += 1;         // Слизаме надолу
        moveDown = false; // Ресетваме флага
    } else {
        x += direction; // Движим се наляво/надясно
        if (x <= 0 || x >= screenWidth - 1) {
            moveDown = true; // При ръб, задаваме да слезе надолу
            direction *= -1; // Смяна на посоката
        }
    }
}

void Enemy::render() const { // рисува врага на екрана с draw_char.
    draw_char(symbol, y, x, color, BLACK);
}

int Enemy::getDirection() const { return direction; }
void Enemy::setDirection(int dir) { direction = dir; }
void Enemy::setMoveDown(bool value) { moveDown = value; }