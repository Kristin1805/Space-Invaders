#include "Enemy.h" // Include the Enemy header

// --- Constructors ---
Enemy::Enemy() : GameObject(), direction(1), moveDown(false) {
    // Default constructor: Initializes with GameObject defaults, direction right, no move down.
}

Enemy::Enemy(int x, int y, wchar_t symbol, COLORS color, int direction) // Symbol is wchar_t
    : GameObject(x, y, L'█', color), direction(direction), moveDown(false) {
    // Parameterized constructor: Initializes enemy at (x,y) with a solid block symbol, color, and direction.
    // L'█' (Unicode U+2588) is used as the default enemy symbol.
}

// --- Overridden Virtual Functions ---
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

// --- Enemy-Specific Getters/Setters/Methods ---
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