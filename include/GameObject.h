#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

class Player;

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void update(float deltaTime);
    virtual void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders);
    virtual void draw(sf::RenderTarget& target) const = 0;
    virtual sf::FloatRect getBounds() const;
    virtual void receiveDamage(int damage, sf::Vector2f sourcePosition);
    virtual bool canReceiveDamage() const;
    virtual void interact();

    bool isAlive() const;
    void destroy();

protected:
    bool alive_ = true;
};
