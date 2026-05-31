#pragma once

#include "Entity.h"

class Enemy : public Entity
{
public:
    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

protected:
    sf::RectangleShape debugShape_;
};
