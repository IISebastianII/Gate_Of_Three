#pragma once

#include <string>

class Spell
{
public:
    Spell() = default;
    Spell(std::string id, int damage, float projectileSpeed, float cooldown, int manaCost);

    const std::string& getId() const;
    int getDamage() const;
    float getProjectileSpeed() const;
    float getCooldown() const;
    int getManaCost() const;

private:
    std::string id_;
    int damage_ = 0;
    float projectileSpeed_ = 0.f;
    float cooldown_ = 0.f;
    int manaCost_ = 0;
};
