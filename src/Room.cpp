#include "Room.h"

#include "Player.h"
#include "Projectile.h"

#include <algorithm>
#include <utility>

namespace
{
bool overlapsVerticalBand(const sf::FloatRect& rect, float bandTop, float bandHeight)
{
    return rect.top < bandTop + bandHeight && rect.top + rect.height > bandTop;
}

float computeLightningLength(
    sf::Vector2f startPosition,
    sf::Vector2f direction,
    float maxRange,
    float thickness,
    float rearReach,
    const std::vector<sf::FloatRect>& solidColliders,
    const std::vector<std::unique_ptr<GameObject>>& objects,
    GameObject*& hitTarget)
{
    const bool right = direction.x >= 0.f;
    const float bandTop = startPosition.y - thickness * 0.5f;
    float strikeLength = std::max(0.f, maxRange);
    float rearLimit = std::max(0.f, rearReach);
    hitTarget = nullptr;

    for (const auto& solid : solidColliders)
    {
        if (!overlapsVerticalBand(solid, bandTop, thickness))
        {
            continue;
        }

        const float blockerDistance = right
            ? solid.left - startPosition.x
            : startPosition.x - (solid.left + solid.width);
        if (blockerDistance >= 0.f && blockerDistance < strikeLength)
        {
            strikeLength = blockerDistance;
        }

        const float rearBlockerDistance = right
            ? startPosition.x - (solid.left + solid.width)
            : solid.left - startPosition.x;
        if (rearBlockerDistance >= 0.f && rearBlockerDistance < rearLimit)
        {
            rearLimit = rearBlockerDistance;
        }
    }

    float bestTargetDistance = strikeLength;
    for (const auto& object : objects)
    {
        if (!object->isAlive() || !object->canReceiveDamage())
        {
            continue;
        }

        const sf::FloatRect bounds = object->getBounds();
        if (!overlapsVerticalBand(bounds, bandTop, thickness))
        {
            continue;
        }

        const float targetDistance = right
            ? bounds.left - startPosition.x
            : startPosition.x - (bounds.left + bounds.width);
        if (targetDistance >= 0.f && targetDistance < bestTargetDistance)
        {
            bestTargetDistance = targetDistance;
            hitTarget = object.get();
        }
    }

    if (hitTarget != nullptr)
    {
        return bestTargetDistance;
    }

    float bestRearTargetDistance = rearLimit;
    for (const auto& object : objects)
    {
        if (!object->isAlive() || !object->canReceiveDamage())
        {
            continue;
        }

        const sf::FloatRect bounds = object->getBounds();
        if (!overlapsVerticalBand(bounds, bandTop, thickness))
        {
            continue;
        }

        const float rearTargetDistance = right
            ? startPosition.x - (bounds.left + bounds.width)
            : bounds.left - startPosition.x;
        if (rearTargetDistance >= 0.f && rearTargetDistance < bestRearTargetDistance)
        {
            bestRearTargetDistance = rearTargetDistance;
            hitTarget = object.get();
        }
    }

    return bestTargetDistance;
}
}

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

    updateProjectileCollisions();
    removeDestroyedObjects();
}

void Room::update(float deltaTime, Player& player)
{
    for (auto& object : objects_)
    {
        object->update(deltaTime, player, solidColliders_);
    }

    updateProjectileCollisions();
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

void Room::damageObjectsInBounds(const sf::FloatRect& damageBounds, int damage, sf::Vector2f sourcePosition)
{
    for (auto& object : objects_)
    {
        if (object->isAlive() && object->canReceiveDamage() && damageBounds.intersects(object->getBounds()))
        {
            object->receiveDamage(damage, sourcePosition);
        }
    }
}

void Room::interactObjectsInBounds(const sf::FloatRect& interactionBounds)
{
    for (auto& object : objects_)
    {
        if (object->isAlive() && interactionBounds.intersects(object->getBounds()))
        {
            object->interact();
        }
    }
}

Projectile& Room::spawnProjectile(sf::Vector2f startPosition, sf::Vector2f direction, float speed, int damage)
{
    GameObject* hitTarget = nullptr;
    const float strikeLength = computeLightningLength(
        startPosition,
        direction,
        speed,
        92.f,
        120.f,
        solidColliders_,
        objects_,
        hitTarget);

    if (hitTarget != nullptr)
    {
        hitTarget->receiveDamage(damage, startPosition);
    }

    auto projectile = std::make_unique<Projectile>(startPosition, direction, strikeLength, damage);
    projectile->setBeamLength(std::max(24.f, strikeLength));
    Projectile& reference = *projectile;
    objects_.push_back(std::move(projectile));
    return reference;
}

bool Room::tryCastSpell(Player& player)
{
    return player.beginSpellCast();
}

bool Room::consumeLongBlastUnlockRequest()
{
    return false;
}

void Room::updateProjectileCollisions()
{
    // Lightning strike damage is resolved on spawn; projectiles only age out here.
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
