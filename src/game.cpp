#include "Game.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

Game::Game() : score(0), level(1), running(true) {
    initializeEnemies(); // Когато се създава нова игра се инициализират енемис
}

void Game::initializeEnemies() {
    for (int i = 0; i < 5; ++i) { // Създаваме 5 врага, разположени на разстояние 5 по x-координата
        enemies.push_back(new Enemy(10 + i * 5, 2, 'W', RED, 1)); //  Създава нов враг (Enemy),
        // разположен на определена позиция, и го добавя в списъка
    }
}

void Game::update() {
    // обновяваме състоянието на всички обекти (тоест трябва и за плейър и за булетс)
    for (auto* e : enemies) {
        // Обновява позицията на всеки враг в enemies с метод update().
        e->update();
    }
    checkCollisions(); // проверява дали има сблъсък между куршум и враг
}

void Game::checkCollisions() {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) { // обхожда всеки куршум в списъка  bullets
        bool hit = false;
        for (auto enemy = enemies.begin(); enemy != enemies.end();) { // обхожда всички врагове.
            if ((*bullet)->getX() == (*enemy)->getX() && (*bullet)->getY() == (*enemy)->getY()) {
                // дали координатите съвпадат — т.е. куршумът и врагът са на едно и също място.
                delete *enemy; // убиваме врага
                enemy = enemies.erase(enemy); // премахваме елемента от списъка.
                hit = true;
                score += 10;
                break;
            } else {
                ++enemy; // няма сблъсък, просто минаваме към следващия враг
            }
        }

        if (hit) {
            delete *bulletIt; // махаме куршума
            bulletIt = bullets.erase(bulletIt); // махаме го от списъка
        } else {
            ++bulletIt; // Ако няма удар, просто отиваме към следващия куршум
        }
    }
}