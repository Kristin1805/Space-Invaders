#include "GameObject.h" // Include the header for GameObject
#include <utility>
//Constructors
GameObject::GameObject() : x(0), y(0), symbol(L' '), color(WHITE) {}

GameObject::GameObject(int x, int y, wchar_t symbol, COLORS color) 
    : x(x), y(y), symbol(symbol), color(color) {}

GameObject::GameObject(const GameObject& other)
    : x(other.x), y(other.y), symbol(other.symbol), color(other.color) {}

GameObject::GameObject(GameObject&& other) noexcept
    : x(other.x), y(other.y), symbol(other.symbol), color(other.color) {}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this != &other) { 
        x = other.x;
        y = other.y;
        symbol = other.symbol;
        color = other.color;
    }
    return *this;
}

//getters
int GameObject::getX() const {
    return x;
}

int GameObject::getY() const {
    return y;
}

wchar_t GameObject::getSymbol() const {
    return symbol;
}

COLORS GameObject::getColor() const {
    return color;
}

//setters
void GameObject::setX(int x) {
    this->x = x;
}

void GameObject::setY(int y) {
    this->y = y;
}

void GameObject::setSymbol(wchar_t symbol) { 
    this->symbol = symbol;
}

void GameObject::setColor(COLORS color) {
    this->color = color;
}