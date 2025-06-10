#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h" 

class Bullet : public GameObject {
private:
    int direction; 

public:
    Bullet(); 
    Bullet(int x, int y, wchar_t symbol, COLORS color, int direction); 

    void update(int screenWidth) override; 
    void render(HANDLE target_buffer) const override; 

    int getDirection() const;
};

#endif // BULLET_H