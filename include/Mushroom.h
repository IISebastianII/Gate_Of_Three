#pragma once

#include "Enemy.h"

#include <memory>

class MushroomProjectile;

class Mushroom : public Enemy
{
public:
    explicit Mushroom(sf::Vector2f feetPosition);
    ~Mushroom() override;

    void update(float deltaTime) override;
    void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders) override;
    void draw(sf::RenderTarget& target) const override;

private:
    void updateProjectiles(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders);
    void spawnProjectile(const Player& player);

    static constexpr float detectionRange_ = 720.f;
    static constexpr float verticalDetectionRange_ = 220.f;
    static constexpr float shotCooldownDuration_ = 1.8f;
    static constexpr std::size_t projectileSpawnFrame_ = 6;

    float shotCooldown_ = 0.f;
    bool projectileSpawned_ = false;
    std::vector<std::unique_ptr<MushroomProjectile>> projectiles_;
};
