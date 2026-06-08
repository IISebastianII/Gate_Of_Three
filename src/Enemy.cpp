#include "Enemy.h"

#include "AssetPaths.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

const sf::Vector2f Enemy::colliderSize_ = {52.f, 74.f};

namespace
{
std::string frameName(int frame)
{
    std::ostringstream stream;
    stream << "tile" << std::setw(3) << std::setfill('0') << frame << ".png";
    return stream.str();
}
}

Enemy::Enemy(sf::Vector2f feetPosition, const std::string& animationRoot)
{
    boundsSize_ = colliderSize_;
    position_ = {feetPosition.x - colliderSize_.x * 0.5f, feetPosition.y - colliderSize_.y};

    debugShape_.setSize(colliderSize_);
    debugShape_.setFillColor(sf::Color(120, 70, 80));
    debugShape_.setOutlineColor(sf::Color::White);
    debugShape_.setOutlineThickness(2.f);

    loadAnimations(animationRoot);
    syncDrawable();
}

void Enemy::update(float deltaTime)
{
    if (damageCooldown_ > 0.f)
    {
        damageCooldown_ = std::max(0.f, damageCooldown_ - deltaTime);
    }

    if (dying_)
    {
        deathTimer_ -= deltaTime;
        if (deathTimer_ <= 0.f)
        {
            destroy();
            return;
        }
    }
    else if (currentState_ == AnimationState::Hurt && damageCooldown_ <= 0.1f)
    {
        setAnimationState(AnimationState::Idle);
    }

    updateAnimation(deltaTime);
    syncDrawable();
}

void Enemy::draw(sf::RenderTarget& target) const
{
    if (currentAnimation() != nullptr)
    {
        target.draw(sprite_);
    }
    else
    {
        target.draw(debugShape_);
    }

    if (!dying_)
    {
        healthIndicator_.draw(target);
    }
}

void Enemy::receiveDamage(int damage, sf::Vector2f sourcePosition)
{
    if (dying_ || damageCooldown_ > 0.f)
    {
        return;
    }

    health_ -= damage;
    facingRight_ = sourcePosition.x > getBounds().left + getBounds().width * 0.5f;
    damageCooldown_ = 0.42f;

    if (health_ <= 0)
    {
        dying_ = true;
        setAnimationState(AnimationState::Dead);
        const Animation* animation = currentAnimation();
        deathTimer_ = animation == nullptr
            ? 0.35f
            : static_cast<float>(animation->frames.size()) * animation->frameTime;
        return;
    }

    setAnimationState(AnimationState::Hurt);
}

void Enemy::loadAnimations(const std::string& animationRoot)
{
    loadFrameSeries(AnimationState::Idle, animationRoot + "/idle", 0, 7, 0.12f, true);
    loadFrameSeries(AnimationState::Hurt, animationRoot + "/hurt", 0, 4, 0.08f, false);
    loadFrameSeries(AnimationState::Dead, animationRoot + "/die", 0, 12, 0.09f, false);
}

void Enemy::loadFrameSeries(AnimationState state, const std::string& directory, int first, int last, float frameTime, bool loop)
{
    Animation animation;
    animation.frameTime = frameTime;
    animation.loop = loop;

    for (int frame = first; frame <= last; ++frame)
    {
        sf::Texture texture;
        texture.setSmooth(false);

        const auto path = AssetPaths::resolve(directory + "/" + frameName(frame));
        if (texture.loadFromFile(path.string()))
        {
            texture.setSmooth(false);
            animation.frames.push_back(texture);
        }
    }

    if (!animation.frames.empty())
    {
        animations_[state] = std::move(animation);
    }
}

void Enemy::updateAnimation(float deltaTime)
{
    const Animation* animation = currentAnimation();
    if (animation == nullptr || animation->frames.empty())
    {
        return;
    }

    animationAccumulator_ += deltaTime;
    while (animationAccumulator_ >= animation->frameTime)
    {
        animationAccumulator_ -= animation->frameTime;
        if (currentFrame_ + 1 < animation->frames.size())
        {
            ++currentFrame_;
        }
        else if (animation->loop)
        {
            currentFrame_ = 0;
        }
    }
}

void Enemy::setAnimationState(AnimationState state)
{
    if (currentState_ == state)
    {
        return;
    }

    currentState_ = state;
    currentFrame_ = 0;
    animationAccumulator_ = 0.f;
}

const Enemy::Animation* Enemy::currentAnimation() const
{
    const auto found = animations_.find(currentState_);
    if (found == animations_.end() || found->second.frames.empty())
    {
        return nullptr;
    }

    return &found->second;
}

void Enemy::syncDrawable()
{
    debugShape_.setPosition(position_);
    healthIndicator_.setPosition({position_.x + colliderSize_.x * 0.5f + 10.f, position_.y + colliderSize_.y + 30.f});
    healthIndicator_.setValue(healthPercent());

    const Animation* animation = currentAnimation();
    if (animation == nullptr || animation->frames.empty())
    {
        return;
    }

    const sf::Texture& texture = animation->frames[std::min(currentFrame_, animation->frames.size() - 1)];
    sprite_.setTexture(texture, true);
    sprite_.setOrigin(static_cast<float>(texture.getSize().x) * 0.5f, static_cast<float>(texture.getSize().y));
    sprite_.setPosition(position_.x + colliderSize_.x * 0.5f, position_.y + colliderSize_.y + 1.f);
    sprite_.setScale(facingRight_ ? textureScale_ : -textureScale_, textureScale_);
}

float Enemy::healthPercent() const
{
    return static_cast<float>(std::max(0, health_)) / static_cast<float>(maxHealth_);
}
