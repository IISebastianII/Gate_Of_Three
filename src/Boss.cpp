#include "Boss.h"

void Boss::update(float deltaTime)
{
    Enemy::update(deltaTime);
}

void Boss::update(float deltaTime, Player&, const std::vector<sf::FloatRect>&)
{
    Enemy::update(deltaTime);
}
