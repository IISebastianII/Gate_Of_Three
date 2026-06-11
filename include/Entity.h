#pragma once

#include "GameObject.h"

class Entity : public GameObject
{
public:
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f position);

    sf::FloatRect getBounds() const override;

protected:
    sf::Vector2f position_ = {0.f, 0.f};
    sf::Vector2f velocity_ = {0.f, 0.f};
    sf::Vector2f boundsSize_ = {0.f, 0.f};
};
