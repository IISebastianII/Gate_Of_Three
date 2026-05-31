#include "Enemy.h"

void Enemy::update(float)
{
}

void Enemy::draw(sf::RenderTarget& target) const
{
    target.draw(debugShape_);
}
