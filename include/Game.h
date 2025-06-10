#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <chrono>
#include <random>

#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "GameObject.h" // Уверяваме се, че е включен за draw_char

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 30;
const int GAME_SPEED_MS = 100; // Пример: 100 милисекунди за по-бавна игра

class Game {
private:
    bool running;
    bool gameOver;
    int score;
    int currentLevel; // <<< НОВО: Текущо ниво на играта
    int lastExtraLifeScore; // <<< НОВО: Точки, при които последно е получен допълнителен живот

    Player* player;
    std::vector<Bullet*> playerBullets;
    std::vector<Bullet*> enemyBullets;
    std::vector<Enemy*> enemies;

    HANDLE hBuffer;
    HANDLE hConsoleOutput;

    std::chrono::steady_clock::time_point lastFrameTime;
    int enemyMoveTimer;
    int enemyFireTimer;
    int enemyMoveFrequency; // <<< НОВО: Честота на движение на враговете (зависи от нивото)
    int enemyFireFrequency; // <<< НОВО: Честота на стрелба на враговете (зависи от нивото)
    std::mt19937 rng;

    // Помощни методи
    void setupConsole();
    void teardownConsole();
    void processInput();
    void updateGame();
    void renderGame();
    void spawnEnemies(); // <<< НОВО: Вече ще се адаптира към нивото
    void handleCollisions();
    void cleanupBullets();
    void checkWinCondition(); // <<< НОВО: Проверява дали нивото е спечелено
    void drawBorder() const;
    void drawString(const std::wstring& str, int y, int x, COLORS foreground_color, COLORS background_color, HANDLE target_buffer) const;

    void resetForNewLevel(); // <<< НОВО: Подготвя играта за ново ниво
    void resetGame();        // <<< НОВО: Рестартира играта от ниво 1

public:
    Game();
    ~Game();
    void run();
};

#endif // GAME_H