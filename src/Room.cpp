#include "Room.h"

#include "Player.h"
#include "Projectile.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace
{
sf::Vector2f rectCenter(const sf::FloatRect& rect)
{
    return {rect.left + rect.width * 0.5f, rect.top + rect.height * 0.5f};
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
    auto projectile = std::make_unique<Projectile>(startPosition, direction, speed, damage);
    Projectile& reference = *projectile;
    objects_.push_back(std::move(projectile));
    return reference;
}

bool Room::tryCastSpell(Player& player)
{
    if (!player.trySpendSpellResources())
    {
        return false;
    }

    const Spell& spell = player.getLongBlastSpell();
    spawnProjectile(
        player.getSpellSpawnPosition(),
        player.getFacingDirection(),
        spell.getProjectileSpeed(),
        spell.getDamage());
    return true;
}

void Room::updateProjectileCollisions()
{
    const sf::FloatRect worldBounds = getBounds();

    for (const auto& object : objects_)
    {
        Projectile* projectile = dynamic_cast<Projectile*>(object.get());
        if (projectile == nullptr || !projectile->isAlive())
        {
            continue;
        }

        const sf::FloatRect projectileBounds = projectile->getBounds();
        if (!projectileBounds.intersects(worldBounds))
        {
            projectile->destroy();
            continue;
        }

        bool hitSolid = false;
        for (const auto& solid : solidColliders_)
        {
            if (projectileBounds.intersects(solid))
            {
                hitSolid = true;
                break;
            }
        }

        if (hitSolid)
        {
            projectile->destroy();
            continue;
        }

        for (auto& target : objects_)
        {
            if (target.get() == projectile || !target->isAlive() || !target->canReceiveDamage())
            {
                continue;
            }

            const sf::FloatRect targetBounds = target->getBounds();
            if (!projectileBounds.intersects(targetBounds))
            {
                continue;
            }

            target->receiveDamage(projectile->getDamage(), rectCenter(projectileBounds));
            projectile->destroy();
            break;
        }
    }
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
