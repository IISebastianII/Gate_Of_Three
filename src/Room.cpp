#include "Room.h"

#include <algorithm>
#include <utility>

Room::Room(RoomType type)
    : type_(type)
{
}

void Room::onEnter()
{
}

void Room::onExit()
{
}

void Room::update(float deltaTime)
{
    for (auto& object : objects_)
    {
        object->update(deltaTime);
    }

    removeDestroyedObjects();
}

void Room::draw(sf::RenderTarget& target) const
{
    for (const auto& exit : exits_)
    {
        exit.draw(target);
    }

    for (const auto& object : objects_)
    {
        object->draw(target);
    }
}

RoomType Room::getType() const
{
    return type_;
}

sf::FloatRect Room::getBounds() const
{
    return {0.f, 0.f, roomSize_.x, roomSize_.y};
}

sf::Vector2f Room::getPlayerSpawnFeet() const
{
    return {64.f, 64.f};
}

const std::vector<sf::FloatRect>& Room::getSolidColliders() const
{
    return solidColliders_;
}

const RoomExit* Room::findTouchedExit(const sf::FloatRect& bounds) const
{
    for (const auto& exit : exits_)
    {
        if (exit.overlaps(bounds))
        {
            return &exit;
        }
    }

    return nullptr;
}

void Room::removeDestroyedObjects()
{
    objects_.erase(
        std::remove_if(objects_.begin(), objects_.end(), [](const auto& object) {
            return !object->isAlive();
        }),
        objects_.end());
}

RoomExit& Room::addExit(RoomType targetRoom, sf::Vector2f targetSpawnFeet, const sf::FloatRect& triggerBounds)
{
    exits_.emplace_back(targetRoom, targetSpawnFeet, triggerBounds);
    return exits_.back();
}
