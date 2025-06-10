#include "Player.h"

Player::Player()
    : GameObject(0, PLAYER_Y_START, L'▲', LIGHT_GREEN), lives(PLAYER_INITIAL_LIVES), score(0) {
}

Player::Player(int x)
    : GameObject(x, PLAYER_Y_START, L'▲', LIGHT_GREEN), lives(PLAYER_INITIAL_LIVES), score(0) {
}

// --- Предефинирани виртуални функции ---
void Player::update(int screenWidth) {
    // Тази функция може да се използва за вътрешни таймери/състояния на играча
}

void Player::render(HANDLE target_buffer) const {
    draw_char(symbol, y, x, color, COLORS::BLACK, target_buffer);
}

// --- Специфични методи за играча ---
void Player::moveLeft() {
    if (x > 1) { // Проверка за лява граница
        x--;
    }
}

void Player::moveRight(int screenWidth) {
    if (x < screenWidth - 2) { // Проверка за дясна граница
        x++;
    }
}

void Player::loseLife() {
    if (lives > 0) { // Уверяваме се, че не губим живот под 0
        lives--;
    }
}

void Player::addLife() { // <<< НОВО: Добавя живот (с лимит по желание)
    lives++;
    // Може да добавите лимит на животите, напр. if (lives < MAX_LIVES) lives++;
}

void Player::resetLives() { // <<< НОВО: Връща животите до начална стойност
    lives = PLAYER_INITIAL_LIVES;
}

int Player::getLives() const {
    return lives;
}

int Player::getScore() const {
    return score;
}

void Player::addScore(int points) {
    score += points;
}