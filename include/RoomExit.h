#pragma once

#include "GameObject.h"
#include "RoomType.h"

#include <SFML/Graphics.hpp>

// Defines a trigger area and the room reached through it.
class RoomExit : public GameObject
{
public:
    RoomExit(RoomType targetRoom, sf::Vector2f targetSpawnFeet, const sf::FloatRect& triggerBounds);

    void setTexture(const sf::Texture& texture, sf::Vector2f bottomCenter, float scale);
    void draw(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    RoomType getTargetRoom() const;
    sf::Vector2f getTargetSpawnFeet() const;
    bool overlaps(const sf::FloatRect& bounds) const;

private:
    RoomType targetRoom_;
    sf::Vector2f targetSpawnFeet_;
    sf::FloatRect triggerBounds_;
    sf::Sprite sprite_;
    bool hasSprite_ = false;
};
