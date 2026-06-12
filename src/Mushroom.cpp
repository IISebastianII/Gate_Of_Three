#include "Mushroom.h"

#include "Player.h"

#include <algorithm>
#include <cmath>

namespace
{
Enemy::Settings mushroomSettings()
{
    Enemy::Settings settings;
    settings.animationRoot = "Animations/Enemy/mushroom";
    settings.colliderSize = {26.f, 37.f};
    settings.textureScale = 1.5f;
    settings.spriteFacesRightByDefault = false;
    settings.maxHealth = 3;
    settings.attackDamage = 1;
    return settings;
}

sf::Vector2f normalized(sf::Vector2f vector)
{
    const float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    if (length <= 0.001f)
    {
        return {1.f, 0.f};
    }

    return {vector.x / length, vector.y / length};
}
}

class MushroomProjectile
{
public:
    MushroomProjectile(sf::Vector2f position, sf::Vector2f direction)
        : position_(position)
        , velocity_(normalized(direction) * speed_)
    {
        shape_.setRadius(radius_);
        shape_.setOrigin(radius_, radius_);
        shape_.setPosition(position_);
        shape_.setFillColor(sf::Color(120, 220, 75));
        shape_.setOutlineColor(sf::Color(220, 255, 170));
        shape_.setOutlineThickness(3.f);
    }

    void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders)
    {
        lifetime_ += deltaTime;
        position_ += velocity_ * deltaTime;
        shape_.setPosition(position_);

        const sf::FloatRect bounds = getBounds();
        if (bounds.intersects(player.getBounds()))
        {
            player.receiveDamage(1, position_);
            alive_ = false;
            return;
        }

        for (const auto& solid : solidColliders)
        {
            if (bounds.intersects(solid))
            {
                alive_ = false;
                return;
            }
        }

        if (lifetime_ >= maxLifetime_)
        {
            alive_ = false;
        }
    }

    void draw(sf::RenderTarget& target) const
    {
        target.draw(shape_);
    }

    bool isAlive() const
    {
        return alive_;
    }

private:
    sf::FloatRect getBounds() const
    {
        return {position_.x - radius_, position_.y - radius_, radius_ * 2.f, radius_ * 2.f};
    }

    static constexpr float radius_ = 11.f;
    static constexpr float speed_ = 180.f;
    static constexpr float maxLifetime_ = 4.f;

    sf::Vector2f position_;
    sf::Vector2f velocity_;
    sf::CircleShape shape_;
    float lifetime_ = 0.f;
    bool alive_ = true;
};

Mushroom::Mushroom(sf::Vector2f feetPosition)
    : Enemy(feetPosition, mushroomSettings())
    , spawnFeetX_(feetPosition.x)
{
}

Mushroom::~Mushroom() = default;

void Mushroom::update(float deltaTime)
{
    if (updatePassiveState(deltaTime))
    {
        return;
    }

    updateVisuals(deltaTime);
}

void Mushroom::update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders)
{
    updateProjectiles(deltaTime, player, solidColliders);

    if (updatePassiveState(deltaTime))
    {
        return;
    }

    if (!isDying())
    {
        shotCooldown_ = std::max(0.f, shotCooldown_ - deltaTime);
        const sf::Vector2f toPlayer = player.getCenter() - getCenter();
        setFacingRight(toPlayer.x > 0.f);
        const bool playerDetected = std::abs(toPlayer.x) <= detectionRange_
            && std::abs(toPlayer.y) <= verticalDetectionRange_;

        if (isAttacking())
        {
            velocity_.x = 0.f;
            if (!projectileSpawned_ && getCurrentAnimationFrame() >= projectileSpawnFrame_)
            {
                spawnProjectile(player);
                projectileSpawned_ = true;
            }
        }
        else if (isHurt())
        {
            velocity_.x = 0.f;
        }
        else if (playerDetected)
        {
            projectileSpawned_ = false;
            const float horizontalDistance = std::abs(toPlayer.x);
            if (shotCooldown_ <= 0.f && horizontalDistance <= preferredMaxDistance_)
            {
                velocity_.x = 0.f;
                startAttack();
                shotCooldown_ = shotCooldownDuration_;
            }
            else if (horizontalDistance > preferredMaxDistance_)
            {
                velocity_.x = toPlayer.x > 0.f ? moveSpeed_ : -moveSpeed_;
                setFacingRight(velocity_.x > 0.f);
                setAnimationState(AnimationState::Walk);
            }
            else if (horizontalDistance < preferredMinDistance_)
            {
                velocity_.x = toPlayer.x > 0.f ? -moveSpeed_ : moveSpeed_;
                setFacingRight(velocity_.x > 0.f);
                setAnimationState(AnimationState::Walk);
            }
            else
            {
                velocity_.x = 0.f;
                setAnimationState(AnimationState::Idle);
            }
        }
        else
        {
            projectileSpawned_ = false;
            const float feetX = getCenter().x;
            if (feetX < spawnFeetX_ - patrolRange_)
            {
                patrolDirection_ = 1.f;
            }
            else if (feetX > spawnFeetX_ + patrolRange_)
            {
                patrolDirection_ = -1.f;
            }

            velocity_.x = patrolDirection_ * moveSpeed_;
            setFacingRight(velocity_.x > 0.f);
            setAnimationState(AnimationState::Walk);
        }

        moveHorizontally(deltaTime, solidColliders);
        moveVertically(deltaTime, solidColliders);
    }

    updateVisuals(deltaTime);
}

void Mushroom::draw(sf::RenderTarget& target) const
{
    Enemy::draw(target);
    for (const auto& projectile : projectiles_)
    {
        projectile->draw(target);
    }
}

void Mushroom::updateProjectiles(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders)
{
    for (auto& projectile : projectiles_)
    {
        projectile->update(deltaTime, player, solidColliders);
    }

    projectiles_.erase(
        std::remove_if(projectiles_.begin(), projectiles_.end(), [](const auto& projectile) {
            return !projectile->isAlive();
        }),
        projectiles_.end());
}

void Mushroom::spawnProjectile(const Player& player)
{
    const sf::Vector2f direction = normalized(player.getCenter() - getCenter());
    const sf::Vector2f start = getCenter() + normalized(direction) * 38.f;
    projectiles_.push_back(std::make_unique<MushroomProjectile>(start, direction));
}
