#include "Game.h"
#include <iostream>
#include <utility>
#include <conio.h>
#include <thread>
#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <string>

void Game::setupConsole() {
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); 
    hBuffer = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,       
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        NULL,                               
        CONSOLE_TEXTMODE_BUFFER,            
        NULL                               
    );

    
    COORD bufferSize = { (SHORT)SCREEN_WIDTH, (SHORT)SCREEN_HEIGHT };
    SetConsoleScreenBufferSize(hConsoleOutput, bufferSize);
    SetConsoleScreenBufferSize(hBuffer, bufferSize);

    // Set the console window dimensions
    SMALL_RECT windowSize = { 0, 0, (SHORT)(SCREEN_WIDTH - 1), (SHORT)(SCREEN_HEIGHT - 1) };
    SetConsoleWindowInfo(hConsoleOutput, TRUE, &windowSize);

    // Make the back buffer the active one, so drawing happens off-screen initially
    SetConsoleActiveScreenBuffer(hBuffer);

    // Hide the console cursor for a cleaner look
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hBuffer, &cursorInfo);
    cursorInfo.bVisible = FALSE; // Set cursor to invisible
    SetConsoleCursorInfo(hBuffer, &cursorInfo);

}

void Game::teardownConsole() {
    // Restore original console buffer (if you saved it, otherwise just close back buffer)
    SetConsoleActiveScreenBuffer(hConsoleOutput);
    // Close the back buffer handle
    CloseHandle(hBuffer);
}

//constructor n destructor
Game::Game()
    : running(true), gameOver(false), score(0), player(nullptr), // Initialize members
    enemyMoveTimer(0), enemyFireTimer(0),
    enemyMoveFrequency(10), 
    enemyFireFrequency(10), 
    rng(std::chrono::system_clock::now().time_since_epoch().count()) // Seed random number generator
{
    setupConsole(); // Initialize console on construction
    player = new Player(SCREEN_WIDTH / 2); 
    spawnEnemies(); 
    lastFrameTime = std::chrono::steady_clock::now(); // Initialize timer
}

Game::~Game() {
    // Clean up memory 
    delete player;
    for (Bullet* bullet : playerBullets) {
        delete bullet;
    }
    for (Bullet* bullet : enemyBullets) {
        delete bullet;
    }
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    playerBullets.clear();
    enemyBullets.clear();
    enemies.clear();

    teardownConsole();
}


void Game::run() {
    while (running) {
        // Calculate elapsed time for consistent updates
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime).count();

        if (elapsedTime >= GAME_SPEED_MS) {
            lastFrameTime = currentTime; // Reset timer for next frame

            processInput(); // Handle player input
            updateGame();   // Update game state
            renderGame();   // Draw everything to console

            if (gameOver) {
                running = false; // Exit game loop if game is over
            }
        }
        // Small sleep to prevent busy-waiting and reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    COORD finalPos = { 0, 0 };
    DWORD charsWritten;
    FillConsoleOutputCharacterA(hConsoleOutput, ' ', SCREEN_WIDTH * SCREEN_HEIGHT, finalPos, &charsWritten);
    FillConsoleOutputAttribute(hConsoleOutput, BLACK, SCREEN_WIDTH * SCREEN_HEIGHT, finalPos, &charsWritten);

    SetConsoleCursorPosition(hConsoleOutput, finalPos);

    if (player && player->getLives() <= 0) {
        std::wcout << L"Game Over! Your final score: " << score << std::endl;
    }
    else if (!enemies.empty() && !running) { // Check if game ended due to non-player death
        std::wcout << L"Game Ended. Your final score: " << score << std::endl;
    }
    else {
        std::wcout << L"Congratulations! You won! Final Score: " << score << std::endl;
    }
    std::wcout << L"Press any key to exit." << std::endl;
    _getch(); // Wait for a key press before closing the console
}
//input handling
void Game::processInput() {
    if (_kbhit()) { // Check if a key has been pressed
        int key = _getch(); // Get the pressed key (non-blocking)

        switch (key) {
        case 'a': // Move player left
        case 'A':
            if (player) player->moveLeft();
            break;
        case 'd': // Move player right
        case 'D':
            if (player) player->moveRight(SCREEN_WIDTH);
            break;
        case ' ': // Fire bullet
            if (player) {
                playerBullets.push_back(new Bullet(player->getX(), player->getY() - 1, L'|', WHITE, -1));
            }
            break;
        case 27: // Escape key (ASCII for ESC) to quit
            running = false;
            break;
        }
    }
}

void Game::updateGame() {
    if (gameOver) return;

    // Update player bullets
    for (Bullet* bullet : playerBullets) {
        bullet->update(SCREEN_WIDTH);
    }

    // Update enemy bullets
    for (Bullet* bullet : enemyBullets) {
        bullet->update(SCREEN_WIDTH);
    }

    // Update enemies
    bool edgeReached = false;
    for (Enemy* enemy : enemies) {
        enemy->update(SCREEN_WIDTH);
        if (enemy->getX() <= 0 || enemy->getX() >= SCREEN_WIDTH - 1) {
            edgeReached = true;
        }
    }

    // If any enemy reached an edge, reverse all enemy directions and make them move down
    if (edgeReached) {
        for (Enemy* enemy : enemies) {
            enemy->setDirection(enemy->getDirection() * -1); // Reverse direction
            enemy->setMoveDown(true); // Flag to move down next update
        }
    }

    enemyFireTimer++;
    if (enemyFireTimer >= enemyFireFrequency) {
        if (!enemies.empty()) {
            std::uniform_int_distribution<> dist(0, enemies.size() - 1);
            int enemyIndex = dist(rng);
            Enemy* firingEnemy = enemies[enemyIndex];
            enemyBullets.push_back(new Bullet(firingEnemy->getX(), firingEnemy->getY() + 1, L'v', RED, 1)); 
        }
        enemyFireTimer = 0; 
    }

    handleCollisions();

    cleanupBullets();

    if (player && player->getLives() <= 0) {
        gameOver = true;
    }

    checkWinCondition();
}

void Game::renderGame() {
    COORD bufferOrigin = { 0, 0 };
    DWORD charsWritten;
    // FillConsoleOutputCharacterA uses ' ' (space)
    FillConsoleOutputCharacterA(hBuffer, ' ', SCREEN_WIDTH * SCREEN_HEIGHT, bufferOrigin, &charsWritten);
    FillConsoleOutputAttribute(hBuffer, BLACK, SCREEN_WIDTH * SCREEN_HEIGHT, bufferOrigin, &charsWritten);

    drawBorder(); 

    // Draw score and lives
    std::wstring scoreStr = L"Score: " + std::to_wstring(score);
    std::wstring livesStr = L"Lives: " + std::to_wstring(player ? player->getLives() : 0);

    drawString(scoreStr, 0, 0, WHITE, BLACK, hBuffer);
    drawString(livesStr, 0, SCREEN_WIDTH - (int)livesStr.length() - 1, WHITE, BLACK, hBuffer);

    // Render player
    if (player) {
        player->render(hBuffer);
    }

    // Render bullets
    for (Bullet* bullet : playerBullets) {
        bullet->render(hBuffer);
    }
    for (Bullet* bullet : enemyBullets) {
        bullet->render(hBuffer);
    }

    // Render enemies
    for (Enemy* enemy : enemies) {
        enemy->render(hBuffer);
    }

    // Swap buffers: display the back buffer on the screen
    SetConsoleActiveScreenBuffer(hBuffer);
}

//helper functions
void Game::drawString(const std::wstring& str, int y, int x, COLORS foreground_color, COLORS background_color, HANDLE target_buffer) const {
    for (size_t i = 0; i < str.length(); ++i) {
        draw_char(str[i], y, x + (int)i, foreground_color, background_color, target_buffer);
    }
}

void Game::drawBorder() const {
    // Using Unicode line drawing characters (L'...')
    const wchar_t HORIZONTAL_LINE = L'─'; // U+2500
    const wchar_t VERTICAL_LINE = L'│';   // U+2502
    const wchar_t TOP_LEFT_CORNER = L'┌'; // U+250C
    const wchar_t TOP_RIGHT_CORNER = L'┐'; // U+2510
    const wchar_t BOTTOM_LEFT_CORNER = L'└'; // U+2514
    const wchar_t BOTTOM_RIGHT_CORNER = L'┘'; // U+2518

    // Draw top and bottom horizontal lines
    for (int x = 1; x < SCREEN_WIDTH - 1; ++x) { // Exclude corners
        draw_char(HORIZONTAL_LINE, 1, x, LIGHT_GREY, COLORS::BLACK, hBuffer); // Line below score
        draw_char(HORIZONTAL_LINE, SCREEN_HEIGHT - 1, x, LIGHT_GREY, COLORS::BLACK, hBuffer); // Bottom line
    }

    // Draw left and right vertical lines
    for (int y = 2; y < SCREEN_HEIGHT - 1; ++y) { // Exclude top row and bottom row for horizontal lines
        draw_char(VERTICAL_LINE, y, 0, LIGHT_GREY, COLORS::BLACK, hBuffer);
        draw_char(VERTICAL_LINE, y, SCREEN_WIDTH - 1, LIGHT_GREY, COLORS::BLACK, hBuffer);
    }

    // Draw corners
    draw_char(TOP_LEFT_CORNER, 1, 0, LIGHT_GREY, COLORS::BLACK, hBuffer);
    draw_char(TOP_RIGHT_CORNER, 1, SCREEN_WIDTH - 1, LIGHT_GREY, COLORS::BLACK, hBuffer);
    draw_char(BOTTOM_LEFT_CORNER, SCREEN_HEIGHT - 1, 0, LIGHT_GREY, COLORS::BLACK, hBuffer);
    draw_char(BOTTOM_RIGHT_CORNER, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, LIGHT_GREY, COLORS::BLACK, hBuffer);
}

void Game::spawnEnemies() {
    const int ENEMY_ROWS = 5;
    const int ENEMY_COLS = 10;
    const int START_X = 5;
    const int START_Y = 3;
    const int X_SPACING = 3;
    const int Y_SPACING = 2;

    for (int r = 0; r < ENEMY_ROWS; ++r) {
        for (int c = 0; c < ENEMY_COLS; ++c) {
            enemies.push_back(new Enemy(
                START_X + c * X_SPACING,
                START_Y + r * Y_SPACING,
                L'█', // Solid block symbol
                YELLOW, // Yellow color for enemies
                1 // Initial direction (1 for right)
            ));
        }
    }
}

void Game::handleCollisions() {
    // Player bullet vs Enemy
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
        [&](Bullet* pBullet) {
            bool collided = false;
            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [&](Enemy* enemy) {
                    if (pBullet->getX() == enemy->getX() && pBullet->getY() == enemy->getY()) {
                        score += 10; // Increase score
                        collided = true; // Mark player bullet for removal
                        delete enemy; // Delete the enemy object
                        return true; // Remove enemy from list
                    }
                    return false;
                }),
                enemies.end());
            return collided; // Remove player bullet if it collided
        }),
        playerBullets.end());

    // Enemy bullet vs Player
    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [&](Bullet* eBullet) {
            if (player && eBullet->getX() == player->getX() && eBullet->getY() == player->getY()) {
                player->loseLife(); // Player loses a life
                delete eBullet; // Delete the enemy bullet
                return true; // Remove enemy bullet from list
            }
            return false;
        }),
        enemyBullets.end());
}

void Game::cleanupBullets() {
    // Remove player bullets that are off-screen
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
        [](Bullet* bullet) {
            if (bullet->getY() < 1) { // Off top of screen
                delete bullet;
                return true;
            }
            return false;
        }),
        playerBullets.end());

    // Remove enemy bullets that are off-screen
    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [](Bullet* bullet) {
            if (bullet->getY() > SCREEN_HEIGHT - 1) { // Off bottom of screen
                delete bullet;
                return true;
            }
            return false;
        }),
        enemyBullets.end());
}

void Game::checkWinCondition() {
    if (enemies.empty()) {
        gameOver = true; 
        running = false; 
    }
}