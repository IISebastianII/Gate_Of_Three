#pragma once

#include "Entity.h"

#include <SFML/Graphics.hpp>

#include <vector>

// Short-lived visual effect used by the player's long range spell.
class Projectile : public Entity
{
public:
    Projectile(sf::Vector2f startPosition, sf::Vector2f direction, float speed, int damage);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    void setBeamLength(float beamLength);
    int getDamage() const;

private:
    void loadVisualAnimation();
    void syncDrawable();

    static constexpr float projectileHeight_ = 42.f;
    static constexpr float maxLifetime_ = 0.16f;
    static constexpr float visualFrameTime_ = 0.035f;
    static constexpr float visualScale_ = 0.42f;

    sf::Vector2f direction_ = {1.f, 0.f};
    int damage_ = 0;
    float lifetime_ = 0.f;
    float frameAccumulator_ = 0.f;
    float beamLength_ = 0.f;
    sf::Vector2f anchorPosition_ = {0.f, 0.f};
    std::vector<sf::Texture> frames_;
    std::size_t currentFrame_ = 0;
    bool hasAnimatedVisual_ = false;

    sf::Sprite sprite_;
    sf::CircleShape glowShape_;
    sf::CircleShape coreShape_;
};
