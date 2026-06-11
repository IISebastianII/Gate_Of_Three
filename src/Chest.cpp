#include "Chest.h"

void Chest::update(float)
{
}

void Chest::draw(sf::RenderTarget& target) const
{
    target.draw(debugShape_);
}

sf::FloatRect Chest::getBounds() const
{
    return debugShape_.getGlobalBounds();
}
