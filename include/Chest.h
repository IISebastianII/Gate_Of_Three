#pragma once

#include "GameObject.h"

class Chest : public GameObject
{
public:
    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

private:
    sf::RectangleShape debugShape_;
};
