#include "Chest.h"

#include "AssetPaths.h"

Chest::Chest(sf::Vector2f feetPosition)
{
    debugShape_.setSize({72.f, 48.f});
    debugShape_.setOrigin(debugShape_.getSize().x * 0.5f, debugShape_.getSize().y);
    debugShape_.setPosition(feetPosition);
    debugShape_.setFillColor(sf::Color(126, 78, 38));
    debugShape_.setOutlineColor(sf::Color(35, 22, 12));
    debugShape_.setOutlineThickness(2.f);

    if (texture_.loadFromFile(AssetPaths::resolve("Animations/Surrondings/Chest/Chest.png").string()))
    {
        texture_.setSmooth(false);
        sprite_.setTexture(texture_, true);
        sprite_.setOrigin(static_cast<float>(texture_.getSize().x) * 0.5f, static_cast<float>(texture_.getSize().y));
        sprite_.setScale(textureScale_, textureScale_);
        sprite_.setPosition(feetPosition);
    }
}

void Chest::update(float)
{
}

void Chest::draw(sf::RenderTarget& target) const
{
    if (texture_.getSize().x > 0)
    {
        target.draw(sprite_);
    }
    else
    {
        target.draw(debugShape_);
    }
}

sf::FloatRect Chest::getBounds() const
{
    if (texture_.getSize().x > 0)
    {
        return sprite_.getGlobalBounds();
    }

    return debugShape_.getGlobalBounds();
}
