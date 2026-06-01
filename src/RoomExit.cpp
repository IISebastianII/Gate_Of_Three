#include "RoomExit.h"

RoomExit::RoomExit(RoomType targetRoom, sf::Vector2f targetSpawnFeet, const sf::FloatRect& triggerBounds)
    : targetRoom_(targetRoom)
    , targetSpawnFeet_(targetSpawnFeet)
    , triggerBounds_(triggerBounds)
{
}

void RoomExit::setTexture(const sf::Texture& texture, sf::Vector2f bottomCenter, float scale)
{
    sprite_.setTexture(texture, true);
    const sf::Vector2u textureSize = texture.getSize();
    sprite_.setOrigin(static_cast<float>(textureSize.x) * 0.5f, static_cast<float>(textureSize.y));
    sprite_.setScale(scale, scale);
    sprite_.setPosition(bottomCenter);
    hasSprite_ = true;
}

void RoomExit::draw(sf::RenderTarget& target) const
{
    if (hasSprite_)
    {
        target.draw(sprite_);
    }
}

sf::FloatRect RoomExit::getBounds() const
{
    return triggerBounds_;
}

RoomType RoomExit::getTargetRoom() const
{
    return targetRoom_;
}

sf::Vector2f RoomExit::getTargetSpawnFeet() const
{
    return targetSpawnFeet_;
}

bool RoomExit::overlaps(const sf::FloatRect& bounds) const
{
    return triggerBounds_.intersects(bounds);
}
