#pragma once

class Spell
{
public:
    Spell() = default;
    Spell(int damage, float projectileSpeed, float cooldown, int manaCost);

    int getDamage() const;
    float getProjectileSpeed() const;
    float getCooldown() const;
    int getManaCost() const;

private:
    int damage_ = 0;
    float projectileSpeed_ = 0.f;
    float cooldown_ = 0.f;
    int manaCost_ = 0;
};
