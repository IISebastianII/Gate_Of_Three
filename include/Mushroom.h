#pragma once

#include "Enemy.h"

class Mushroom : public Enemy
{
public:
    explicit Mushroom(sf::Vector2f feetPosition);

    void update(float deltaTime) override;
    void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders) override;
};
