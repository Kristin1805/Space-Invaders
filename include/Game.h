#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <chrono> 
#include <random>

#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "GameObject.h"

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 30;
const int GAME_SPEED_MS = 100; 


const int PLAYER_FIRE_COOLDOWN_MS = 300; 


class Game {
private:
    bool running;
    bool gameOver;
    int score;
    int currentLevel;
    int lastExtraLifeScore;

    Player* player;
    std::vector<Bullet*> playerBullets;
    std::vector<Bullet*> enemyBullets;
    std::vector<Enemy*> enemies;

    HANDLE hBuffer;
    HANDLE hConsoleOutput;

    std::chrono::steady_clock::time_point lastFrameTime;
    std::chrono::steady_clock::time_point lastPlayerFireTime; 

    int enemyMoveTimer;
    int enemyFireTimer;
    int enemyMoveFrequency;
    int enemyFireFrequency;
    std::mt19937 rng;

    void setupConsole();
    void teardownConsole();
    void processInput();
    void updateGame();
    void renderGame();
    void spawnEnemies();
    void handleCollisions();
    void cleanupBullets();
    void checkWinCondition();
    void drawBorder() const;
    void drawString(const std::wstring& str, int y, int x, COLORS foreground_color, COLORS background_color, HANDLE target_buffer) const;

    void resetForNewLevel();
    void resetGame();

public:
    Game();
    ~Game();
    void run();
};

#endif // GAME_H
