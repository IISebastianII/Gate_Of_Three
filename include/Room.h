#pragma once

#include "GameObject.h"
#include "RoomExit.h"
#include "RoomType.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

class Room
{
public:
    explicit Room(RoomType type);
    virtual ~Room() = default;

    virtual void onEnter();
    virtual void onExit();
    virtual void update(float deltaTime);
    virtual void draw(sf::RenderTarget& target) const;

    RoomType getType() const;
    virtual sf::FloatRect getBounds() const;
    virtual sf::Vector2f getPlayerSpawnFeet() const;

    const std::vector<sf::FloatRect>& getSolidColliders() const;
    const RoomExit* findTouchedExit(const sf::FloatRect& bounds) const;
    void damageObjectsInBounds(const sf::FloatRect& damageBounds, int damage, sf::Vector2f sourcePosition);

protected:
    template <typename ObjectType, typename... Args>
    ObjectType& addObject(Args&&... args)
    {
        auto object = std::make_unique<ObjectType>(std::forward<Args>(args)...);
        ObjectType& reference = *object;
        objects_.push_back(std::move(object));
        return reference;
    }

    void removeDestroyedObjects();
    RoomExit& addExit(RoomType targetRoom, sf::Vector2f targetSpawnFeet, const sf::FloatRect& triggerBounds);

    RoomType type_;
    sf::Vector2f roomSize_ = {1280.f, 720.f};
    sf::Color clearColor_ = sf::Color(126, 184, 208);
    std::vector<sf::FloatRect> solidColliders_;
    std::vector<std::unique_ptr<GameObject>> objects_;
    std::vector<RoomExit> exits_;
};
