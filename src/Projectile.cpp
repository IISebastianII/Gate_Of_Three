#include "Projectile.h"

#include <cmath>

namespace
{
sf::Vector2f normalizeDirection(sf::Vector2f direction)
{
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length <= 0.0001f)
    {
        return {1.f, 0.f};
    }

    return {direction.x / length, direction.y / length};
}
}

Projectile::Projectile(sf::Vector2f startPosition, sf::Vector2f direction, float speed, int damage)
    : direction_(normalizeDirection(direction))
    , damage_(damage)
{
    boundsSize_ = {projectileSize_, projectileSize_};
    position_ = {
        startPosition.x - projectileSize_ * 0.5f,
        startPosition.y - projectileSize_ * 0.5f
    };
    velocity_ = {direction_.x * speed, direction_.y * speed};

    glowShape_.setRadius(projectileSize_ * 0.5f);
    glowShape_.setOrigin(projectileSize_ * 0.5f, projectileSize_ * 0.5f);
    glowShape_.setFillColor(sf::Color(170, 95, 255, 70));
    glowShape_.setOutlineThickness(4.f);
    glowShape_.setOutlineColor(sf::Color(230, 210, 255, 90));

    coreShape_.setRadius(projectileSize_ * 0.24f);
    coreShape_.setOrigin(projectileSize_ * 0.24f, projectileSize_ * 0.24f);
    coreShape_.setFillColor(sf::Color(245, 232, 255));

    syncDrawable();
}

void Projectile::update(float deltaTime)
{
    position_ += velocity_ * deltaTime;
    lifetime_ += deltaTime;

    if (lifetime_ >= maxLifetime_)
    {
        destroy();
    }

    syncDrawable();
}

void Projectile::draw(sf::RenderTarget& target) const
{
    target.draw(glowShape_);
    target.draw(coreShape_);
}

int Projectile::getDamage() const
{
    return damage_;
}

sf::Vector2f Projectile::getDirection() const
{
    return direction_;
}

bool Projectile::hasExpired() const
{
    return lifetime_ >= maxLifetime_;
}

void Projectile::syncDrawable()
{
    const sf::Vector2f center = {position_.x + projectileSize_ * 0.5f, position_.y + projectileSize_ * 0.5f};
    glowShape_.setPosition(center);
    coreShape_.setPosition(center);
}
