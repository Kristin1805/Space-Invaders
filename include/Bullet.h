#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

class Bullet : public GameObject {
private:
    int direction;

public:
    Bullet();
    Bullet(int x, int y, char symbol, COLORS color, int direction);

    void update() override;
    void render() const override;

    int getDirection() const;
    void setDirection(int direction);
};

#endif // BULLET_H

