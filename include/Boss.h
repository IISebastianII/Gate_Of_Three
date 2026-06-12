#pragma once

#include "Enemy.h"

class Boss : public Enemy
{
public:
    using Enemy::Enemy;

    void update(float deltaTime) override;
};
