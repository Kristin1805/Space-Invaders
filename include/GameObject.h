#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <windows.h>
#include <string>
#include <wchar.h>
#include <utility>

static HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

enum COLORS {
    BLACK = 0,
    BLUE = FOREGROUND_BLUE,
    CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN,
    GREEN = FOREGROUND_GREEN,
    RED = FOREGROUND_RED,
    BROWN = FOREGROUND_RED | FOREGROUND_GREEN,
    PURPLE = FOREGROUND_RED | FOREGROUND_BLUE,
    LIGHT_GREY = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
    GREY = 0 | FOREGROUND_INTENSITY,
    LIGHT_BLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    LIGHT_CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    LIGHT_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    LIGHT_RED = FOREGROUND_RED | FOREGROUND_INTENSITY,
    YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    PINK = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    WHITE = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
};

inline void draw_char(wchar_t ch, int y, int x, COLORS foreground_color, COLORS background_color, HANDLE target_buffer) {
    CHAR_INFO ch_info;
    ch_info.Char.UnicodeChar = ch;
    ch_info.Attributes = foreground_color | (background_color << 4);

    COORD buf_size = { 1, 1 };
    COORD buf_coord = { 0, 0 };
    SMALL_RECT screen_pos = {
        (SHORT)x,
        (SHORT)y,
        (SHORT)(x + 1),
        (SHORT)(y + 1)
    };
    ::WriteConsoleOutput(target_buffer, &ch_info, buf_size, buf_coord, &screen_pos);
}

class GameObject {
protected:
    int x, y;
    wchar_t symbol;
    COLORS color;

public:
    GameObject();
    GameObject(int x, int y, wchar_t symbol, COLORS color);
    GameObject(const GameObject& other);
    GameObject(GameObject&& other) noexcept;
    GameObject& operator=(const GameObject& other);
    virtual ~GameObject() = default;

    virtual void update(int screenWidth) = 0;
    virtual void render(HANDLE target_buffer) const = 0;

    int getX() const;
    int getY() const;
    wchar_t getSymbol() const;
    COLORS getColor() const;

    void setX(int x);
    void setY(int y);
    void setSymbol(wchar_t symbol);
    void setColor(COLORS color);
};

#endif // GAMEOBJECT_H