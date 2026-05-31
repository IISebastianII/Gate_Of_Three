#pragma once

#include "Entity.h"

class Projectile : public Entity
{
public:
    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

private:
    sf::CircleShape debugShape_;
};
