#include "Boss.h"

namespace
{
Enemy::Settings crystalGolemSettings()
{
    Enemy::Settings settings;
    settings.animationRoot = "Animations/Enemy/crystal_golem";
    settings.colliderSize = {104.f, 148.f};
    settings.textureScale = 6.f;
    settings.detectionRange = 1600.f;
    settings.attackRange = 164.f;
    settings.attackHeight = 148.f;
    settings.attackInset = 48.f;
    settings.preferredPlayerGap = 68.f;
    settings.maxHealth = 15;
    settings.attackDamage = 2;
    settings.attackActiveStartFrame = 6;
    settings.attackActiveEndFrame = 9;
    return settings;
}
}

Boss::Boss(sf::Vector2f feetPosition)
    : Enemy(feetPosition, crystalGolemSettings())
{
}

void Boss::update(float deltaTime)
{
    Enemy::update(deltaTime);
}

void Boss::update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders)
{
    Enemy::update(deltaTime, player, solidColliders);
}
