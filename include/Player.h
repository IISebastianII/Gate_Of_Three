#pragma once

#include "Entity.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

class Player : public Entity
{
public:
    Player();

    void handleEvent(const sf::Event& event);
    void update(float deltaTime) override;
    void update(float deltaTime, const std::vector<sf::FloatRect>& solidColliders, const sf::FloatRect& worldBounds);
    void draw(sf::RenderTarget& target) const override;

    void setFeetPosition(sf::Vector2f feetPosition);
    sf::Vector2f getCenter() const;
    sf::FloatRect getBounds() const override;

private:
    enum class AnimationState
    {
        Idle,
        Run,
        Jump,
        Fall,
        Attack,
        Slide
    };

    struct Animation
    {
        std::vector<sf::Texture> frames;
        float frameTime = 0.12f;
        bool loop = true;
    };

    void loadAnimations();
    void loadFrameSeries(AnimationState state, const std::string& directory, const std::string& prefix, int first, int last, float frameTime, bool loop);
    void applyInput();
    void moveHorizontally(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    void moveVertically(float deltaTime, const std::vector<sf::FloatRect>& solidColliders);
    void keepInsideWorld(const sf::FloatRect& worldBounds);
    void updateActionTimers(float deltaTime);
    void updateAnimation(float deltaTime);
    void setAnimationState(AnimationState state);
    const Animation* currentAnimation() const;
    float animationDuration(AnimationState state, float fallbackDuration) const;
    void syncDrawable();

    static constexpr float moveSpeed_ = 300.f;
    static constexpr float slideSpeed_ = 580.f;
    static constexpr float slideDuration_ = 0.36f;
    static constexpr float jumpVelocity_ = -820.f;
    static constexpr float gravity_ = 1850.f;
    static constexpr float maxStepHeight_ = 36.f;
    static constexpr float textureScale_ = 3.f;
    static const sf::Vector2f colliderSize_;

    bool facingRight_ = true;
    bool onGround_ = false;
    bool jumpQueued_ = false;
    bool attackQueued_ = false;
    bool slideQueued_ = false;
    bool attacking_ = false;
    bool sliding_ = false;
    float attackTimer_ = 0.f;
    float slideTimer_ = 0.f;
    float slideDirection_ = 1.f;

    std::map<AnimationState, Animation> animations_;
    AnimationState currentState_ = AnimationState::Idle;
    std::size_t currentFrame_ = 0;
    float animationAccumulator_ = 0.f;

    sf::Sprite sprite_;
    sf::RectangleShape placeholder_;
};
