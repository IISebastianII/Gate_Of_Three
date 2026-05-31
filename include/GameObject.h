#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void update(float deltaTime);
    virtual void draw(sf::RenderTarget& target) const = 0;
    virtual sf::FloatRect getBounds() const;

    bool isAlive() const;
    void destroy();

protected:
    bool alive_ = true;
};
