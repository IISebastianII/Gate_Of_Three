#pragma once

#include "Enemy.h"

class Boss : public Enemy
{
public:
    explicit Boss(sf::Vector2f feetPosition);

    void update(float deltaTime) override;
    void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders) override;
};
