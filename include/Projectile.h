#pragma once

#include "Entity.h"

#include <SFML/Graphics.hpp>

class Projectile : public Entity
{
public:
    Projectile(sf::Vector2f startPosition, sf::Vector2f direction, float speed, int damage);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    int getDamage() const;
    sf::Vector2f getDirection() const;
    bool hasExpired() const;

private:
    void syncDrawable();

    static constexpr float projectileSize_ = 24.f;
    static constexpr float maxLifetime_ = 2.2f;

    sf::Vector2f direction_ = {1.f, 0.f};
    int damage_ = 0;
    float lifetime_ = 0.f;
    sf::CircleShape glowShape_;
    sf::CircleShape coreShape_;
};
