#include "GameObject.h"

void GameObject::update(float)
{
}

sf::FloatRect GameObject::getBounds() const
{
    return {};
}

bool GameObject::isAlive() const
{
    return alive_;
}

void GameObject::destroy()
{
    alive_ = false;
}
