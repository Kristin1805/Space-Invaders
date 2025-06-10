#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h" // Include the base GameObject class

// Defines the starting Y-coordinate for the player.
const int PLAYER_Y_START = 25; // Example: row 25

class Player : public GameObject {
private:
    int lives; // Player's remaining lives
    int score; // Player's current score

public:
    // Constructors
    Player(); // Default constructor
    Player(int x); // Parameterized constructor (sets initial X)

    // Overridden virtual functions from GameObject
    void update(int screenWidth) override; // Player-specific update logic
    void render(HANDLE target_buffer) const override; // Player-specific rendering

    // Player-specific methods
    void moveLeft();  // Moves player left
    void moveRight(int screenWidth); // Moves player right within screen bounds
    void loseLife();  // Decreases player lives
    int getLives() const; // Gets current lives
    int getScore() const; // Gets current score
    void addScore(int points); // Adds points to score
};

#endif // PLAYER_H