#pragma once

#include "Entity.h"
#include "RadialHealthIndicator.h"

#include <SFML/Graphics.hpp>

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

// Basic melee enemy. Its settings are also reused by the boss.
class Enemy : public Entity
{
public:
    struct Settings
    {
        std::string animationRoot = "Animations/Enemy/skeleton";
        sf::Vector2f colliderSize = {52.f, 74.f};
        float textureScale = 3.f;
        bool spriteFacesRightByDefault = true;
        float patrolSpeed = 70.f;
        float chaseSpeed = 125.f;
        float patrolRange = 180.f;
        float detectionRange = 390.f;
        float attackRange = 82.f;
        float attackHeight = 70.f;
        float attackInset = 24.f;
        float preferredPlayerGap = 34.f;
        float attackCooldownDuration = 0.95f;
        float maxStepHeight = 68.f;
        int maxHealth = 3;
        int attackDamage = 1;
        std::size_t attackActiveStartFrame = 4;
        std::size_t attackActiveEndFrame = 6;
    };

    explicit Enemy(sf::Vector2f feetPosition);
    Enemy(sf::Vector2f feetPosition, Settings settings);

    void update(float deltaTime) override;
    void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders) override;
    void draw(sf::RenderTarget& target) const override;
    void receiveDamage(int damage, sf::Vector2f sourcePosition) override;
    bool canReceiveDamage() const override;

protected:
    enum class AnimationState
    {
        Idle,
        Walk,
        Attack,
        Hurt,
        Dead
    };

private:
    struct Animation
    {
        std::vector<sf::Texture> frames;
        float frameTime = 0.12f;
        bool loop = true;
    };

    void loadAnimations(const std::string& animationRoot);
    void loadFrameSeries(AnimationState state, const std::vector<std::string>& directories, int first, int last, float frameTime, bool loop);
    void updateAi(float deltaTime, Player& player);
protected:
    bool updatePassiveState(float deltaTime);
    void updateVisuals(float deltaTime);
    void moveHorizontally(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    void moveVertically(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    sf::Vector2f getCenter() const;
    void setFacingRight(bool facingRight);
    bool isAttacking() const;
    bool isDying() const;
    bool isHurt() const;
    std::size_t getCurrentAnimationFrame() const;
    void startAttack();
    void setAnimationState(AnimationState state);

private:
    sf::FloatRect getAttackBounds() const;
    bool isAttackActive() const;
    void tryHitPlayer(Player& player);
    void updateAnimation(float deltaTime);
    const Animation* currentAnimation() const;
    float animationDuration(AnimationState state, float fallbackDuration) const;
    void syncDrawable();
    float healthPercent() const;

    static constexpr float gravity_ = 1850.f;

    sf::Vector2f colliderSize_ = {52.f, 74.f};
    float textureScale_ = 3.f;
    bool spriteFacesRightByDefault_ = true;
    float patrolSpeed_ = 70.f;
    float chaseSpeed_ = 125.f;
    float patrolRange_ = 180.f;
    float detectionRange_ = 390.f;
    float attackRange_ = 82.f;
    float attackHeight_ = 70.f;
    float attackInset_ = 24.f;
    float preferredPlayerGap_ = 34.f;
    float attackCooldownDuration_ = 0.95f;
    float maxStepHeight_ = 68.f;

    int maxHealth_ = 3;
    int attackDamage_ = 1;
    int health_ = 3;
    std::size_t attackActiveStartFrame_ = 4;
    std::size_t attackActiveEndFrame_ = 6;
    bool facingRight_ = false;
    bool onGround_ = false;
    bool dying_ = false;
    bool attacking_ = false;
    bool attackHitDone_ = false;
    float spawnFeetX_ = 0.f;
    float patrolDirection_ = -1.f;
    float damageCooldown_ = 0.f;
    float deathTimer_ = 0.f;
    float attackTimer_ = 0.f;
    float attackDuration_ = 0.f;
    float attackCooldown_ = 0.f;

    std::map<AnimationState, Animation> animations_;
    AnimationState currentState_ = AnimationState::Idle;
    std::size_t currentFrame_ = 0;
    float animationAccumulator_ = 0.f;

    sf::Sprite sprite_;
    sf::RectangleShape debugShape_;
    RadialHealthIndicator healthIndicator_;
};
