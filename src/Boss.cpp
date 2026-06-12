#include "Boss.h"

Boss::Boss(sf::Vector2f feetPosition)
    : Enemy(feetPosition, "Animations/Enemy/crystal_golem", 15, 2, 6, 9)
{
}

void Boss::update(float deltaTime)
{
    Enemy::update(deltaTime);
}

void Boss::update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders)
{
    Enemy::update(deltaTime, player, solidColliders);
}
