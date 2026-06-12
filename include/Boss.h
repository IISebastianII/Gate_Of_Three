#pragma once

#include "Enemy.h"

// Boss variant configured with more health, range and larger animations.
class Boss : public Enemy
{
public:
    explicit Boss(sf::Vector2f feetPosition);

    void update(float deltaTime) override;
    void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders) override;
};
