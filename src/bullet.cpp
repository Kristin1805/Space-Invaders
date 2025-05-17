#include "Bullet.h"
#include <windows.h>

extern HANDLE hConsoleOutput;

void draw_char(char ch, int y, int x, COLORS foreground_color, COLORS background_color) {
    CHAR_INFO ch_info;
    ch_info.Char.AsciiChar = ch;
    ch_info.Attributes = foreground_color | (background_color << 4);

    COORD buf_size = {1, 1};
    COORD buf_coord = {0, 0};
    SMALL_RECT screen_pos = {x, y, x+1, y+1};
    ::WriteConsoleOutput(hConsoleOutput, &ch_info, buf_size, buf_coord, &screen_pos);
}

Bullet::Bullet() : GameObject(), direction(-1) {}

Bullet::Bullet(int x, int y, char symbol, COLORS color, int direction)
    : GameObject(x, y, symbol, color), direction(direction) {}

void Bullet::update() {
    y += direction;
}

void Bullet::render() const {
    draw_char(symbol, y, x, color, COLORS::BLACK);
}

int Bullet::getDirection() const {
    return direction;
}

void Bullet::setDirection(int dir) {
    direction = dir;
}
