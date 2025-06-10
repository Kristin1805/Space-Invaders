#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h" 

class Bullet : public GameObject {
private:
    int direction; // -1 for player bullet (up), 1 for enemy bullet (down)

public:
    // Constructors
    Bullet(); 
    Bullet(int x, int y, wchar_t symbol, COLORS color, int direction); 

    // Overridden virtual functions from GameObject
    void update(int screenWidth) override; 
    void render(HANDLE target_buffer) const override; 

    int getDirection() const;
};

#endif // BULLET_H
#include "Bullet.h" 

// --- Constructors ---
Bullet::Bullet() : GameObject(), direction(-1) {}

Bullet::Bullet(int x, int y, wchar_t symbol, COLORS color, int direction) 
    : GameObject(x, y, L'•', color), direction(direction) {}

// --- Overridden Virtual Functions ---
void Bullet::update(int screenWidth) {
    y += direction;
}

void Bullet::render(HANDLE target_buffer) const {
    draw_char(symbol, y, x, color, COLORS::BLACK, target_buffer);
}

int Bullet::getDirection() const {
    return direction;
}