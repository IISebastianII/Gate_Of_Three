#pragma once

#include "Enemy.h"

class Boss : public Enemy
{
public:
    void update(float deltaTime) override;
};
