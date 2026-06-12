#include "Spell.h"

Spell::Spell(int damage, float projectileSpeed, float cooldown, int manaCost)
    : damage_(damage)
    , projectileSpeed_(projectileSpeed)
    , cooldown_(cooldown)
    , manaCost_(manaCost)
{
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
