#include "Player.h"

Player::Player()
    : GameObject(0, PLAYER_Y_START, L'▲', LIGHT_GREEN), lives(PLAYER_INITIAL_LIVES), score(0) {
}

Player::Player(int x)
    : GameObject(x, PLAYER_Y_START, L'▲', LIGHT_GREEN), lives(PLAYER_INITIAL_LIVES), score(0) {
}

//Предефинирани виртуални функции
void Player::update(int screenWidth) {}

void Player::render(HANDLE target_buffer) const {
    draw_char(symbol, y, x, color, COLORS::BLACK, target_buffer);
}

void Player::moveLeft() {
    if (x > 1) { 
        x--;
    }
}

void Player::moveRight(int screenWidth) {
    if (x < screenWidth - 2) { 
        x++;
    }
}

void Player::loseLife() {
    if (lives > 0) { 
        lives--;
    }
}

void Player::addLife() { 
    lives++;
}

void Player::resetLives() { 
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