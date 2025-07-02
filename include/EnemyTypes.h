#ifndef ENEMY_TYPES_H
#define ENEMY_TYPES_H

#include "Enemy.h"

class EnemyType1 : public Enemy {
public:
    EnemyType1(int x, int y);
    int getScoreValue() const override;
};

class EnemyType2 : public Enemy {
public:
    EnemyType2(int x, int y);
    int getScoreValue() const override;
};

class EnemyType3 : public Enemy {
public:
    EnemyType3(int x, int y);
    int getScoreValue() const override;
};

class EnemyType4 : public Enemy {
public:
    EnemyType4(int x, int y);
    int getScoreValue() const override;
};

#endif
