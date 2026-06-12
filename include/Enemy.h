#pragma once

#include "Entity.h"
#include "RadialHealthIndicator.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

class Enemy : public Entity
{
public:
    explicit Enemy(sf::Vector2f feetPosition, const std::string& animationRoot = "Animations/Enemy/skeleton");

    void update(float deltaTime) override;
    void update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders) override;
    void draw(sf::RenderTarget& target) const override;
    void receiveDamage(int damage, sf::Vector2f sourcePosition) override;
    bool canReceiveDamage() const override;

private:
    enum class AnimationState
    {
        Idle,
        Walk,
        Attack,
        Hurt,
        Dead
    };

    struct Animation
    {
        std::vector<sf::Texture> frames;
        float frameTime = 0.12f;
        bool loop = true;
    };

    void loadAnimations(const std::string& animationRoot);
    void loadFrameSeries(AnimationState state, const std::string& directory, int first, int last, float frameTime, bool loop);
    void updateAi(float deltaTime, Player& player);
    void moveHorizontally(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    void moveVertically(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    sf::Vector2f getCenter() const;
    sf::FloatRect getAttackBounds() const;
    bool isAttackActive() const;
    void tryHitPlayer(Player& player);
    void updateAnimation(float deltaTime);
    void setAnimationState(AnimationState state);
    const Animation* currentAnimation() const;
    float animationDuration(AnimationState state, float fallbackDuration) const;
    void syncDrawable();
    float healthPercent() const;

    static constexpr float textureScale_ = 3.f;
    static constexpr float patrolSpeed_ = 70.f;
    static constexpr float chaseSpeed_ = 125.f;
    static constexpr float patrolRange_ = 180.f;
    static constexpr float detectionRange_ = 390.f;
    static constexpr float attackRange_ = 82.f;
    static constexpr float attackHeight_ = 70.f;
    static constexpr float attackInset_ = 24.f;
    static constexpr float preferredPlayerGap_ = 34.f;
    static constexpr float attackCooldownDuration_ = 0.95f;
    static constexpr float attackActiveStart_ = 0.22f;
    static constexpr float attackActiveEnd_ = 0.45f;
    static constexpr float gravity_ = 1850.f;
    static constexpr int attackDamage_ = 1;
    static constexpr int maxHealth_ = 3;
    static const sf::Vector2f colliderSize_;

    int health_ = maxHealth_;
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
