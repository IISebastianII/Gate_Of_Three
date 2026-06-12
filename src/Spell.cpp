#include "Spell.h"

#include <utility>

Spell::Spell(std::string id, int damage, float projectileSpeed, float cooldown, int manaCost)
    : id_(std::move(id))
    , damage_(damage)
    , projectileSpeed_(projectileSpeed)
    , cooldown_(cooldown)
    , manaCost_(manaCost)
{
}

const std::string& Spell::getId() const
{
    return id_;
}

int Spell::getDamage() const
{
    return damage_;
}

float Spell::getProjectileSpeed() const
{
    return projectileSpeed_;
}

float Spell::getCooldown() const
{
    return cooldown_;
}

int Spell::getManaCost() const
{
    return manaCost_;
}
