#include "Game.h"
#include <conio.h>
#include <thread>
#include <algorithm>
#include <string>
#include <iostream>
#include <utility>

//consts
const int ENEMY_MOVE_FREQ_LEVEL_1 = 15;
const int ENEMY_FIRE_FREQ_LEVEL_1 = 25;

const int ENEMY_MOVE_FREQ_LEVEL_2 = 10;
const int ENEMY_FIRE_FREQ_LEVEL_2 = 15;
const int SCORE_FOR_EXTRA_LIFE = 300;

const int ENEMY_MOVE_FREQ_LEVEL_3 = 5;
const int ENEMY_FIRE_FREQ_LEVEL_3 = 5;

//console setup
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

    SMALL_RECT windowSize = { 0, 0, (SHORT)(SCREEN_WIDTH - 1), (SHORT)(SCREEN_HEIGHT - 1) };
    SetConsoleWindowInfo(hConsoleOutput, TRUE, &windowSize);

    SetConsoleActiveScreenBuffer(hBuffer);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hBuffer, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hBuffer, &cursorInfo);
}

void Game::teardownConsole() {
    SetConsoleActiveScreenBuffer(hConsoleOutput);
    CloseHandle(hBuffer);
}

//constructor and destructor
Game::Game()
    : running(true), gameOver(false), score(0), currentLevel(1), lastExtraLifeScore(0),
    player(nullptr),
    enemyMoveTimer(0), enemyFireTimer(0),
    enemyMoveFrequency(ENEMY_MOVE_FREQ_LEVEL_1),
    enemyFireFrequency(ENEMY_FIRE_FREQ_LEVEL_1),
    rng(std::chrono::system_clock::now().time_since_epoch().count()),
    lastPlayerFireTime(std::chrono::steady_clock::now()) { 
    setupConsole();
    resetGame();
}

Game::~Game() {
    delete player;
    for (Bullet* bullet : playerBullets) { delete bullet; }
    for (Bullet* bullet : enemyBullets) { delete bullet; }
    for (Enemy* enemy : enemies) { delete enemy; }
    playerBullets.clear();
    enemyBullets.clear();
    enemies.clear();

    teardownConsole();
}


void Game::run() {
    while (running) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime).count();

        if (elapsedTime >= GAME_SPEED_MS) {
            lastFrameTime = currentTime;

            processInput();
            updateGame();
            renderGame();

            if (gameOver) {
                running = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    COORD finalPos = { 0, 0 };
    DWORD charsWritten;
    FillConsoleOutputCharacterA(hConsoleOutput, ' ', SCREEN_WIDTH * SCREEN_HEIGHT, finalPos, &charsWritten);
    FillConsoleOutputAttribute(hConsoleOutput, BLACK, SCREEN_WIDTH * SCREEN_HEIGHT, finalPos, &charsWritten);
    SetConsoleCursorPosition(hConsoleOutput, finalPos);

    if (player && player->getLives() <= 0) {
        if (currentLevel == 3) {
            std::wcout << L"Играта свърши! Започваш от ниво 1. Твоят резултат: " << score << std::endl;
        }
        else {
            std::wcout << L"Играта свърши! Твоят резултат: " << score << std::endl;
        }
    }
    else {
        std::wcout << L"Поздравления! Премина всички нива! Краен резултат: " << score << std::endl;
    }
    std::wcout << L"Натисни произволен клавиш за изход." << std::endl;
    _getch();
}

//input
void Game::processInput() {
    if (_kbhit()) {
        int key = _getch();
        switch (key) {
        case 'a': case 'A':
            if (player) player->moveLeft();
            break;
        case 'd': case 'D':
            if (player) player->moveRight(SCREEN_WIDTH);
            break;
        case ' ': // Стрелба с интервал
            if (player) {
                auto currentTime = std::chrono::steady_clock::now();
                // Изчисляваме колко време е минало от последния изстрел
                auto elapsedSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastPlayerFireTime).count();

                // Ако е минало достатъчно време (повече от PLAYER_FIRE_COOLDOWN_MS)
                if (elapsedSinceLastShot >= PLAYER_FIRE_COOLDOWN_MS) {
                    playerBullets.push_back(new Bullet(player->getX(), player->getY() - 1, L'|', WHITE, -1));
                    lastPlayerFireTime = currentTime; // Нулираме таймера за следващ изстрел
                }
            }
            break;
        case 27: // ESC ключ за изход
            running = false;
            break;
        }
    }
}

void Game::updateGame() {
    if (gameOver) return;

    switch (currentLevel) {
    case 1:
        enemyMoveFrequency = ENEMY_MOVE_FREQ_LEVEL_1;
        enemyFireFrequency = ENEMY_FIRE_FREQ_LEVEL_1;
        break;
    case 2:
        enemyMoveFrequency = ENEMY_MOVE_FREQ_LEVEL_2;
        enemyFireFrequency = ENEMY_FIRE_FREQ_LEVEL_2;
        if (player && player->getScore() >= lastExtraLifeScore + SCORE_FOR_EXTRA_LIFE) {
            player->addLife();
            lastExtraLifeScore = player->getScore();
        }
        break;
    case 3:
        enemyMoveFrequency = ENEMY_MOVE_FREQ_LEVEL_3;
        enemyFireFrequency = ENEMY_FIRE_FREQ_LEVEL_3;
        break;
    }

    for (Bullet* bullet : playerBullets) {
        bullet->update(SCREEN_WIDTH);
    }
    for (Bullet* bullet : enemyBullets) {
        bullet->update(SCREEN_WIDTH);
    }

    bool edgeReached = false;
    if (++enemyMoveTimer >= enemyMoveFrequency) {
        for (Enemy* enemy : enemies) {
            enemy->update(SCREEN_WIDTH);
            if (enemy->getX() <= 1 || enemy->getX() >= SCREEN_WIDTH - 2) {
                edgeReached = true;
            }
        }
        enemyMoveTimer = 0;
    }

    if (edgeReached) {
        for (Enemy* enemy : enemies) {
            enemy->setDirection(enemy->getDirection() * -1);
            enemy->setMoveDown(true);
        }
    }

    if (++enemyFireTimer >= enemyFireFrequency) {
        if (!enemies.empty()) {
            std::uniform_int_distribution<> dist(0, enemies.size() - 1);
            int enemyIndex = dist(rng);
            Enemy* firingEnemy = enemies[enemyIndex];
            enemyBullets.push_back(new Bullet(firingEnemy->getX(), firingEnemy->getY() + 1, L'▼', RED, 1));
        }
        enemyFireTimer = 0;
    }

    handleCollisions();
    if (player) { 
        score = player->getScore(); 
    }
    cleanupBullets();

    if (player && player->getLives() <= 0) {
        gameOver = true;
    }

    checkWinCondition();
}

void Game::renderGame() {
    COORD bufferOrigin = { 0, 0 };
    DWORD charsWritten;
    FillConsoleOutputCharacterA(hBuffer, ' ', SCREEN_WIDTH * SCREEN_HEIGHT, bufferOrigin, &charsWritten);
    FillConsoleOutputAttribute(hBuffer, BLACK, SCREEN_WIDTH * SCREEN_HEIGHT, bufferOrigin, &charsWritten);

    drawBorder(); 

    // Рисува резултат и животи
    std::wstring scoreStr = L"Резултат: " + std::to_wstring(score);
    std::wstring livesStr = L"Животи: " + std::to_wstring(player ? player->getLives() : 0);
    std::wstring levelStr = L"Ниво: " + std::to_wstring(currentLevel);

    drawString(scoreStr, 0, 0, WHITE, BLACK, hBuffer);
    drawString(livesStr, 0, SCREEN_WIDTH - (int)livesStr.length() - 1, WHITE, BLACK, hBuffer);
    // Позициониране на нивото (напр. по средата на горния ред)
    drawString(levelStr, 0, SCREEN_WIDTH / 2 - (int)levelStr.length() / 2, WHITE, BLACK, hBuffer);


    // Рендира играча, куршумите и враговете
    if (player) { player->render(hBuffer); }
    for (Bullet* bullet : playerBullets) { bullet->render(hBuffer); }
    for (Bullet* bullet : enemyBullets) { bullet->render(hBuffer); }
    for (Enemy* enemy : enemies) { enemy->render(hBuffer); }

    SetConsoleActiveScreenBuffer(hBuffer); // Сменя буферите
}

void Game::drawString(const std::wstring& str, int y, int x, COLORS foreground_color, COLORS background_color, HANDLE target_buffer) const {
    for (size_t i = 0; i < str.length(); ++i) {
        draw_char(str[i], y, x + (int)i, foreground_color, background_color, target_buffer);
    }
}

void Game::drawBorder() const {
    const wchar_t HORIZONTAL_LINE = L'─';
    const wchar_t VERTICAL_LINE = L'│';
    const wchar_t TOP_LEFT_CORNER = L'┌';
    const wchar_t TOP_RIGHT_CORNER = L'┐';
    const wchar_t BOTTOM_LEFT_CORNER = L'└';
    const wchar_t BOTTOM_RIGHT_CORNER = L'┘';

    for (int x = 1; x < SCREEN_WIDTH - 1; ++x) {
        draw_char(HORIZONTAL_LINE, 1, x, LIGHT_GREY, COLORS::BLACK, hBuffer);
        draw_char(HORIZONTAL_LINE, SCREEN_HEIGHT - 1, x, LIGHT_GREY, COLORS::BLACK, hBuffer);
    }

    for (int y = 2; y < SCREEN_HEIGHT - 1; ++y) {
        draw_char(VERTICAL_LINE, y, 0, LIGHT_GREY, COLORS::BLACK, hBuffer);
        draw_char(VERTICAL_LINE, y, SCREEN_WIDTH - 1, LIGHT_GREY, COLORS::BLACK, hBuffer);
    }

    draw_char(TOP_LEFT_CORNER, 1, 0, LIGHT_GREY, COLORS::BLACK, hBuffer);
    draw_char(TOP_RIGHT_CORNER, 1, SCREEN_WIDTH - 1, LIGHT_GREY, COLORS::BLACK, hBuffer);
    draw_char(BOTTOM_LEFT_CORNER, SCREEN_HEIGHT - 1, 0, LIGHT_GREY, COLORS::BLACK, hBuffer);
    draw_char(BOTTOM_RIGHT_CORNER, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, LIGHT_GREY, COLORS::BLACK, hBuffer);
}

void Game::spawnEnemies() {
    // Изчистваме съществуващи врагове, ако има такива
    for (Enemy* enemy : enemies) { delete enemy; }
    enemies.clear();

    const int ENEMY_ROWS = 3; // По-малко редове, за да не пълнят екрана веднага
    const int ENEMY_COLS = 8;
    const int START_X = 5;
    const int START_Y_LEVEL_1 = 3; // По-високо за ниво 1
    const int START_Y_LEVEL_2 = 4; // Малко по-ниско за ниво 2
    const int START_Y_LEVEL_3 = 5; // Още по-ниско за ниво 3
    const int X_SPACING = 3;
    const int Y_SPACING = 2;

    int current_start_y = START_Y_LEVEL_1;
    if (currentLevel == 2) current_start_y = START_Y_LEVEL_2;
    else if (currentLevel == 3) current_start_y = START_Y_LEVEL_3;

    for (int r = 0; r < ENEMY_ROWS; ++r) {
        for (int c = 0; c < ENEMY_COLS; ++c) {
            enemies.push_back(new Enemy(
                START_X + c * X_SPACING,
                current_start_y + r * Y_SPACING,
                L'█', 
                YELLOW, 
                1 
            ));
        }
    }
}

void Game::handleCollisions() {
    // Куршум на играча срещу враг
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
        [&](Bullet* pBullet) {
            bool collided = false;
            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [&](Enemy* enemy) {
                    if (pBullet->getX() == enemy->getX() && pBullet->getY() == enemy->getY()) {
                        player->addScore(10); // Увеличава резултата на играча
                        collided = true;
                        delete enemy;
                        return true;
                    }
                    return false;
                }),
                enemies.end());
            return collided;
        }),
        playerBullets.end());

    // Куршум на врага срещу играча
    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [&](Bullet* eBullet) {
            if (player && eBullet->getX() == player->getX() && eBullet->getY() == player->getY()) {
                player->loseLife(); // Играчът губи живот
                delete eBullet;
                return true;
            }
            return false;
        }),
        enemyBullets.end());

    // Враг срещу играча 
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [&](Enemy* enemy) {
            if (player && enemy->getX() == player->getX() && enemy->getY() == player->getY()) {
                player->resetLives(); 
                gameOver = true;     
                running = false;      
                delete enemy;
                return true;
            }
            return false;
        }),
        enemies.end());
}

void Game::cleanupBullets() {
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
        [](Bullet* bullet) {
            if (bullet->getY() < 1) { delete bullet; return true; }
            return false;
        }), playerBullets.end());

    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [](Bullet* bullet) {
            if (bullet->getY() > SCREEN_HEIGHT - 1) { delete bullet; return true; }
            return false;
        }), enemyBullets.end());
}

void Game::checkWinCondition() {
    if (enemies.empty()) {
        if (currentLevel < 3) { 
            currentLevel++;      
            resetForNewLevel(); 
        }
        else { 
            gameOver = true;
            running = false;
        }
    }
}

// Подготвя играта за ново ниво (или за рестарт)
void Game::resetForNewLevel() {
    // Изчистваме всички куршуми
    for (Bullet* bullet : playerBullets) { delete bullet; }
    playerBullets.clear();
    for (Bullet* bullet : enemyBullets) { delete bullet; }
    enemyBullets.clear();

    // Препозиционираме играча
    if (player) {
        player->setX(SCREEN_WIDTH / 2);
        // Player->setY(PLAYER_Y_START) already in constructor
    }

    spawnEnemies(); // Създаваме нови врагове за новото ниво
    // Честотите ще бъдат зададени в updateGame() при следващия цикъл

    // Може да добавите кратко съобщение "Ниво X"
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

// Рестартира цялата игра от ниво 1
void Game::resetGame() {
    score = 0;
    currentLevel = 1;
    lastExtraLifeScore = 0;
    gameOver = false;
    running = true;

    // Създаваме нов играч или рестартираме съществуващия
    if (player) {
        delete player; // Изтриваме стария играч
    }
    player = new Player(SCREEN_WIDTH / 2); // Създаваме нов играч (resetLives е в конструктора)

    resetForNewLevel(); // Използваме тази функция за да почисти и създаде врагове
}