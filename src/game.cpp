#include "Game.h"
#include <conio.h>
#include <thread>
#include <algorithm>
#include <string>
#include <iostream> // За std::wcout
#include <utility>  // За std::move

// --- Константни стойности за нивата ---
const int ENEMY_MOVE_FREQ_LEVEL_1 = 15; // По-бавно
const int ENEMY_FIRE_FREQ_LEVEL_1 = 25; // Рядко

const int ENEMY_MOVE_FREQ_LEVEL_2 = 10; // По-бързо
const int ENEMY_FIRE_FREQ_LEVEL_2 = 15; // По-често
const int SCORE_FOR_EXTRA_LIFE = 300; // Точки за допълнителен живот

const int ENEMY_MOVE_FREQ_LEVEL_3 = 5;  // Много бързо
const int ENEMY_FIRE_FREQ_LEVEL_3 = 5;  // Постоянно стрелят

// --- Console Setup and Teardown ---
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

    // Опционално: Програмно задаване на шрифта, ако ръчното не се запазва
    /*
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    GetCurrentConsoleFontEx(hConsoleOutput, FALSE, &cfi);
    cfi.dwFontSize.Y = 16;
    wcscpy_s(cfi.FaceName, L"Cascadia Mono");
    SetCurrentConsoleFontEx(hConsoleOutput, FALSE, &cfi);
    */
}

void Game::teardownConsole() {
    SetConsoleActiveScreenBuffer(hConsoleOutput);
    CloseHandle(hBuffer);
}

// --- Конструктор и Деструктор ---
Game::Game()
    : running(true), gameOver(false), score(0), currentLevel(1), lastExtraLifeScore(0),
    player(nullptr), // Ще го създадем в resetGame
    enemyMoveTimer(0), enemyFireTimer(0),
    enemyMoveFrequency(ENEMY_MOVE_FREQ_LEVEL_1), // Първоначална честота за Ниво 1
    enemyFireFrequency(ENEMY_FIRE_FREQ_LEVEL_1), // Първоначална честота за Ниво 1
    rng(std::chrono::system_clock::now().time_since_epoch().count()) {
    setupConsole();
    resetGame(); // Започваме играта, като я рестартираме за ниво 1
}

Game::~Game() {
    // Почистване на динамично заделената памет
    delete player;
    for (Bullet* bullet : playerBullets) { delete bullet; }
    for (Bullet* bullet : enemyBullets) { delete bullet; }
    for (Enemy* enemy : enemies) { delete enemy; }
    playerBullets.clear();
    enemyBullets.clear();
    enemies.clear();

    teardownConsole();
}

// --- Основен цикъл на играта ---
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
                running = false; // Прекратяваме цикъла, ако играта е свършила
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Кратка пауза за намаляване на натоварването на процесора
    }

    // --- Екран "Край на играта" / Изход ---
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
                playerBullets.push_back(new Bullet(player->getX(), player->getY() - 1, L'|', WHITE, -1)); // Куршум нагоре
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

    // Адаптиране на честотите според текущото ниво
    switch (currentLevel) {
    case 1:
        enemyMoveFrequency = ENEMY_MOVE_FREQ_LEVEL_1;
        enemyFireFrequency = ENEMY_FIRE_FREQ_LEVEL_1;
        break;
    case 2:
        enemyMoveFrequency = ENEMY_MOVE_FREQ_LEVEL_2;
        enemyFireFrequency = ENEMY_FIRE_FREQ_LEVEL_2;
        // Логика за бонус живот на ниво 2
        if (player && player->getScore() >= lastExtraLifeScore + SCORE_FOR_EXTRA_LIFE) {
            player->addLife();
            lastExtraLifeScore = player->getScore(); // Обновяваме прага за следващ живот
        }
        break;
    case 3:
        enemyMoveFrequency = ENEMY_MOVE_FREQ_LEVEL_3;
        enemyFireFrequency = ENEMY_FIRE_FREQ_LEVEL_3;
        break;
    }

    // Обновяваме куршумите на играча
    for (Bullet* bullet : playerBullets) {
        bullet->update(SCREEN_WIDTH);
    }
    // Обновяваме куршумите на врага
    for (Bullet* bullet : enemyBullets) {
        bullet->update(SCREEN_WIDTH);
    }

    // Обновяваме враговете
    bool edgeReached = false;
    // Движение на враговете само при определена честота
    if (++enemyMoveTimer >= enemyMoveFrequency) {
        for (Enemy* enemy : enemies) {
            enemy->update(SCREEN_WIDTH); // Актуализира позицията на врага
            if (enemy->getX() <= 1 || enemy->getX() >= SCREEN_WIDTH - 2) { // Проверка за граница на екрана
                edgeReached = true;
            }
        }
        enemyMoveTimer = 0; // Рестартираме таймера
    }


    // Ако враг достигне края, всички сменят посоката и слизат надолу
    if (edgeReached) {
        for (Enemy* enemy : enemies) {
            enemy->setDirection(enemy->getDirection() * -1); // Смяна на посоката
            enemy->setMoveDown(true); // Задава флаг за слизане надолу
        }
    }

    // Логика за стрелба на враговете
    if (++enemyFireTimer >= enemyFireFrequency) {
        if (!enemies.empty()) {
            std::uniform_int_distribution<> dist(0, enemies.size() - 1);
            int enemyIndex = dist(rng);
            Enemy* firingEnemy = enemies[enemyIndex];
            // Вражески куршум: L'v' или L'▼' (U+25BC)
            enemyBullets.push_back(new Bullet(firingEnemy->getX(), firingEnemy->getY() + 1, L'▼', RED, 1));
        }
        enemyFireTimer = 0; // Рестартираме таймера
    }


    handleCollisions(); // Обработва сблъсъците
    if (player) { 
        score = player->getScore(); // Синхронизира резултата на класа Game с резултата на играча
    }
    cleanupBullets();   // Почиства куршуми извън екрана

    // Проверка за край на играта (животи на играча)
    if (player && player->getLives() <= 0) {
        gameOver = true;
    }

    checkWinCondition(); // Проверява дали нивото е спечелено
}

// --- Рендиране ---
void Game::renderGame() {
    COORD bufferOrigin = { 0, 0 };
    DWORD charsWritten;
    FillConsoleOutputCharacterA(hBuffer, ' ', SCREEN_WIDTH * SCREEN_HEIGHT, bufferOrigin, &charsWritten);
    FillConsoleOutputAttribute(hBuffer, BLACK, SCREEN_WIDTH * SCREEN_HEIGHT, bufferOrigin, &charsWritten);

    drawBorder(); // Рисува границата

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

// --- Спомагателни функции ---

// Рисува низ от символи
void Game::drawString(const std::wstring& str, int y, int x, COLORS foreground_color, COLORS background_color, HANDLE target_buffer) const {
    for (size_t i = 0; i < str.length(); ++i) {
        draw_char(str[i], y, x + (int)i, foreground_color, background_color, target_buffer);
    }
}

// Рисува границата на играта
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

// Създава врагове според текущото ниво
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
                L'█', // Плътен блок
                YELLOW, // Жълт цвят за враговете
                1 // Първоначална посока (1 за надясно)
            ));
        }
    }
}

// Обработка на сблъсъци
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

    // Враг срещу играча (НОВА ЛОГИКА - незабавна смърт)
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [&](Enemy* enemy) {
            if (player && enemy->getX() == player->getX() && enemy->getY() == player->getY()) {
                player->resetLives(); // Играчът губи всички животи
                gameOver = true;      // Играта свършва
                running = false;      // Прекратяваме цикъла
                delete enemy;         // Изтриваме врага
                return true;          // Премахваме врага от списъка
            }
            return false;
        }),
        enemies.end());
}

// Почиства куршуми извън екрана
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

// Проверява дали нивото е спечелено и преминава към следващо
void Game::checkWinCondition() {
    if (enemies.empty()) {
        if (currentLevel < 3) { // Има още нива
            currentLevel++;      // Преминаваме към следващо ниво
            resetForNewLevel();  // Рестартираме за новото ниво
        }
        else { // Всички нива са завършени
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