#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h" 

class Enemy : public GameObject {
private:
    int direction; 
    bool moveDown; 

public:
    Enemy(); 
    Enemy(int x, int y, wchar_t symbol, COLORS color, int direction); 


    void update(int screenWidth) override; 
    void render(HANDLE target_buffer) const override; 

    void setDirection(int dir); 

    int getDirection() const;
    void setMoveDown(bool down); 
    bool getMoveDown() const;
    virtual int getScoreValue() const = 0;
};

#endif // ENEMY_H