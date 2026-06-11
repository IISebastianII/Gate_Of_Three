#include "Entity.h"

sf::Vector2f Entity::getPosition() const
{
    return position_;
}

void Entity::setPosition(sf::Vector2f position)
{
    position_ = position;
}

sf::FloatRect Entity::getBounds() const
{
    return {position_, boundsSize_};
}
