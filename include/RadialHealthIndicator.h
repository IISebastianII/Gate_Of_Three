#pragma once

#include <SFML/Graphics.hpp>

// Draws the circular health indicator shown below enemies.
class RadialHealthIndicator
{
public:
    RadialHealthIndicator();

    void setPosition(sf::Vector2f center);
    void setValue(float value);
    void draw(sf::RenderTarget& target) const;

private:
    void rebuildFill();

    static constexpr float radius_ = 16.f;
    static constexpr unsigned segmentCount_ = 40;

    sf::Vector2f center_ = {};
    float value_ = 1.f;
    sf::CircleShape background_;
    sf::CircleShape outline_;
    sf::VertexArray fill_;
};
