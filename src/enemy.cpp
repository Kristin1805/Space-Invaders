#include "Enemy.h" 

//сonstructors
Enemy::Enemy() : GameObject(), direction(1), moveDown(false) {
    
}

Enemy::Enemy(int x, int y, wchar_t symbol, COLORS color, int direction) // Symbol is wchar_t
    : GameObject(x, y, L'█', color), direction(direction), moveDown(false) {
    
}

//Overridden Virtual Functions
void Enemy::update(int screenWidth) {
    if (moveDown) {
        y++; // Move down one row
        moveDown = false; // Reset the flag
    }
    x += direction; // Move horizontally
}

void Enemy::render(HANDLE target_buffer) const {
    // Renders the enemy's symbol at its current position with its color.
    draw_char(symbol, y, x, color, COLORS::BLACK, target_buffer);
}

void Enemy::setDirection(int dir) {
    direction = dir;
}

int Enemy::getDirection() const {
    return direction;
}

void Enemy::setMoveDown(bool down) {
    moveDown = down;
}

bool Enemy::getMoveDown() const {
    return moveDown;
}