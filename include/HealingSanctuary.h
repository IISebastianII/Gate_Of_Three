#pragma once

#include "GameObject.h"

#include <SFML/Graphics.hpp>

#include <array>

class HealingSanctuary : public GameObject
{
public:
    explicit HealingSanctuary(sf::Vector2f feetPosition);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void interact() override;
    bool consumeHealingRequest();

private:
    void updateEffects();

    static constexpr float textureScale_ = 2.f;
    static constexpr float effectDuration_ = 1.8f;
    static constexpr float boundsWidth_ = 150.f;
    static constexpr float boundsHeight_ = 180.f;
    static constexpr std::size_t particleCount_ = 10;

    sf::Vector2f feetPosition_;
    sf::FloatRect bounds_;
    sf::Texture texture_;
    sf::Sprite sprite_;
    sf::RectangleShape placeholder_;
    sf::CircleShape outerRing_;
    sf::CircleShape innerRing_;
    std::array<sf::CircleShape, particleCount_> particles_;
    bool hasTexture_ = false;
    bool healingRequested_ = false;
    float effectTimer_ = 0.f;
};
