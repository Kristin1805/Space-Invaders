#include "EnemyTypes.h"

EnemyType1::EnemyType1(int x, int y)
    : Enemy(x, y, L'☺', LIGHT_RED, 1) {}

int EnemyType1::getScoreValue() const { return 10; }

EnemyType2::EnemyType2(int x, int y)
    : Enemy(x, y, L'☻', YELLOW, 1) {}

int EnemyType2::getScoreValue() const { return 20; }

EnemyType3::EnemyType3(int x, int y)
    : Enemy(x, y, L'♦', LIGHT_CYAN, 1) {}

int EnemyType3::getScoreValue() const { return 30; }

EnemyType4::EnemyType4(int x, int y)
    : Enemy(x, y, L'♣', LIGHT_GREEN, 1) {}

int EnemyType4::getScoreValue() const { return 40; }
