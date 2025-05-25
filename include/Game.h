#ifndef GAME_H
#define GAME_H
#pragma once
#include <list>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
using namespace std;


class Game {
private:
    list<GameObject*> enemies; // списъци от врагове (тип GameObject*)

    void initializeEnemies(); // създава вълна от врагове
    void update();

    void checkCollisions(); // проверява сблъсъци
    void render();

public:
    Game();
    Game(const Game& other);
    Game(Game&& other) noexcept;
    Game& operator=(const Game& other);
    ~Game();

    void run();
};

#endif //GAME_H