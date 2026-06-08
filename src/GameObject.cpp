#include "GameObject.h"

void GameObject::update(float)
{
}

sf::FloatRect GameObject::getBounds() const
{
    return {};
}

void GameObject::receiveDamage(int, sf::Vector2f)
{
}

bool GameObject::isAlive() const
{
    return alive_;
}

void GameObject::destroy()
{
    alive_ = false;
}
