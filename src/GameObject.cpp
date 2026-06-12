#include "GameObject.h"

void GameObject::update(float)
{
}

void GameObject::update(float deltaTime, Player&, const std::vector<sf::FloatRect>&)
{
    update(deltaTime);
}

sf::FloatRect GameObject::getBounds() const
{
    return {};
}

void GameObject::receiveDamage(int, sf::Vector2f)
{
}

bool GameObject::canReceiveDamage() const
{
    return false;
}

void GameObject::interact()
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
