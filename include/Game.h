#ifndef GAME_H
#define GAME_H

#include <vector>    
#include <string>    
#include <chrono>    
#include <random>  
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

// Define screen dimensions (constants for console size)
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 30;

const int GAME_SPEED_MS = 100;

class Game {
private:
    // Game state variables
    bool running;
    bool gameOver; 
    int score;

    // Game objects (pointers to manage dynamically allocated objects)
    Player* player;                // Pointer to the single player object
    std::vector<Bullet*> playerBullets; // List of player-fired bullets
    std::vector<Bullet*> enemyBullets;  // List of enemy-fired bullets
    std::vector<Enemy*> enemies;       // List of active enemies

    // Console handles for double buffering
    HANDLE hBuffer;          
    HANDLE hConsoleOutput;   

    // Timing for game updates
    std::chrono::steady_clock::time_point lastFrameTime; // Time of the last frame update
    int enemyMoveTimer;      // Timer for enemy movement frequency
    int enemyFireTimer;      // Timer for enemy firing frequency
    int enemyMoveFrequency;  // How often enemies move (e.g., every 500ms)
    int enemyFireFrequency;  // How often enemies fire (e.g., every 1000ms)
    std::mt19937 rng;        // Random number generator for enemy firing

    // Private helper methods
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

public:
    // Constructor and Destructor
    Game();
    ~Game();

    // Main game loop
    void run();
};

#endif // GAME_H