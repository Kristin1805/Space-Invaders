#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"

const int PLAYER_Y_START = 25; 
const int PLAYER_INITIAL_LIVES = 3; 

class Player : public GameObject {
private:
    int lives; 
    int score; 

public:
    Player(); 
    Player(int x); 

    void update(int screenWidth) override; 
    void render(HANDLE target_buffer) const override; 

    void moveLeft();
    void moveRight(int screenWidth);
    void loseLife(); // Намалява животите на играча
    void addLife();  
    void resetLives(); 
    int getLives() const;
    int getScore() const;
    void addScore(int points);
};

#endif // PLAYER_H