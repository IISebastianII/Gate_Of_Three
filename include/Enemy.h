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
    void draw(sf::RenderTarget& target) const override;
    void receiveDamage(int damage, sf::Vector2f sourcePosition) override;

private:
    enum class AnimationState
    {
        Idle,
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
    void updateAnimation(float deltaTime);
    void setAnimationState(AnimationState state);
    const Animation* currentAnimation() const;
    void syncDrawable();
    float healthPercent() const;

    static constexpr float textureScale_ = 3.f;
    static constexpr int maxHealth_ = 3;
    static const sf::Vector2f colliderSize_;

    int health_ = maxHealth_;
    bool facingRight_ = false;
    bool dying_ = false;
    float damageCooldown_ = 0.f;
    float deathTimer_ = 0.f;

    std::map<AnimationState, Animation> animations_;
    AnimationState currentState_ = AnimationState::Idle;
    std::size_t currentFrame_ = 0;
    float animationAccumulator_ = 0.f;

    sf::Sprite sprite_;
    sf::RectangleShape debugShape_;
    RadialHealthIndicator healthIndicator_;
};
