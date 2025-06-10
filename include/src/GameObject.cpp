#include "GameObject.h" // Include the header for GameObject
#include <utility>
// --- Constructors ---
GameObject::GameObject() : x(0), y(0), symbol(L' '), color(WHITE) {
    // Default constructor: initializes to (0,0), a space character, and white color.
}

GameObject::GameObject(int x, int y, wchar_t symbol, COLORS color) // Symbol is wchar_t
    : x(x), y(y), symbol(symbol), color(color) {
    // Parameterized constructor: initializes with provided values.
}

// Copy constructor
GameObject::GameObject(const GameObject& other)
    : x(other.x), y(other.y), symbol(other.symbol), color(other.color) {
}

// Move constructor
GameObject::GameObject(GameObject&& other) noexcept
    : x(other.x), y(other.y), symbol(other.symbol), color(other.color) {
    // In move constructor, 'other' resources might be nullified if they were dynamically allocated.
    // For simple types like int, wchar_t, enum, this is not strictly necessary.
}

// Copy assignment operator
GameObject& GameObject::operator=(const GameObject& other) {
    if (this != &other) { // Avoid self-assignment
        x = other.x;
        y = other.y;
        symbol = other.symbol;
        color = other.color;
    }
    return *this;
}

// --- Getters ---
int GameObject::getX() const {
    return x;
}

int GameObject::getY() const {
    return y;
}

wchar_t GameObject::getSymbol() const { // Returns wchar_t
    return symbol;
}

COLORS GameObject::getColor() const {
    return color;
}

// --- Setters ---
void GameObject::setX(int x) {
    this->x = x;
}

void GameObject::setY(int y) {
    this->y = y;
}

void GameObject::setSymbol(wchar_t symbol) { // Takes wchar_t
    this->symbol = symbol;
}

void GameObject::setColor(COLORS color) {
    this->color = color;
}