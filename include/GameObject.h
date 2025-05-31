#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <windows.h>

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

class GameObject {
protected:
    int x, y;
    char symbol;
    COLORS color;

public:
    GameObject();
    GameObject(int x, int y, char symbol, COLORS color);
    GameObject(const GameObject& other);
    GameObject(GameObject&& other) noexcept;
    GameObject& operator=(const GameObject& other);
    virtual ~GameObject() = default;

    virtual void update() = 0;
    virtual void render() const = 0;

    int getX() const;
    int getY() const;
    char getSymbol() const;
    COLORS getColor() const;

    void setX(int x);
    void setY(int y);
    void setSymbol(char symbol);
    void setColor(COLORS color);
};

