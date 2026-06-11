#pragma once

#include "GameObject.h"

class Chest : public GameObject
{
public:
    explicit Chest(sf::Vector2f feetPosition);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

private:
    static constexpr float textureScale_ = 2.f;

    sf::Texture texture_;
    sf::Sprite sprite_;
    sf::RectangleShape debugShape_;
};
