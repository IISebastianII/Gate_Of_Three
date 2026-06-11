#include "Projectile.h"

void Projectile::update(float deltaTime)
{
    position_ += velocity_ * deltaTime;
    debugShape_.setPosition(position_);
}

void Projectile::draw(sf::RenderTarget& target) const
{
    target.draw(debugShape_);
}
