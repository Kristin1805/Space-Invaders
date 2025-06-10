#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h" // Include the base GameObject class

class Bullet : public GameObject {
private:
    int direction; // -1 for player bullet (up), 1 for enemy bullet (down)

public:
    // Constructors
    Bullet(); // Default constructor
    // Parameterized constructor for creating a bullet (symbol is pre-defined for bullets)
    Bullet(int x, int y, wchar_t symbol, COLORS color, int direction); // <<< symbol is wchar_t

    // Overridden virtual functions from GameObject
    void update(int screenWidth) override; // Bullet-specific update logic (movement)
    void render(HANDLE target_buffer) const override; // Bullet-specific rendering

    // Bullet-specific getter
    int getDirection() const;
};

#endif // BULLET_H
#include "Bullet.h" // Include the Bullet header

// --- Constructors ---
Bullet::Bullet() : GameObject(), direction(-1) {
    // Default constructor: Initializes with GameObject defaults and direction up.
}

Bullet::Bullet(int x, int y, wchar_t symbol, COLORS color, int direction) // Symbol is wchar_t
    : GameObject(x, y, L'•', color), direction(direction) {
    // Parameterized constructor: Initializes bullet at (x,y) with a specific symbol and direction.
    // L'•' (Unicode U+2022) is used as the default bullet symbol.
}

// --- Overridden Virtual Functions ---
void Bullet::update(int screenWidth) {
    // Moves the bullet based on its direction
    y += direction;
}

void Bullet::render(HANDLE target_buffer) const {
    // Renders the bullet's symbol at its current position with its color.
    draw_char(symbol, y, x, color, COLORS::BLACK, target_buffer);
}

// --- Bullet-Specific Getter ---
int Bullet::getDirection() const {
    return direction;
}