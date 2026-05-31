#include "Player.h"

#include "AssetPaths.h"

#include <algorithm>
#include <cmath>

const sf::Vector2f Player::colliderSize_ = {46.f, 92.f};

Player::Player()
{
    boundsSize_ = colliderSize_;
    placeholder_.setSize(colliderSize_);
    placeholder_.setFillColor(sf::Color(70, 110, 180));
    placeholder_.setOutlineColor(sf::Color::White);
    placeholder_.setOutlineThickness(2.f);

    loadAnimations();
    syncDrawable();
}

void Player::update(float deltaTime)
{
    updateActionTimers(deltaTime);
    updateAnimation(deltaTime);
    syncDrawable();
}

void Player::handleEvent(const sf::Event& event)
{
    if (event.type != sf::Event::KeyPressed)
    {
        return;
    }

    const bool jumpKey = event.key.code == sf::Keyboard::W
        || event.key.code == sf::Keyboard::Up;

    if (jumpKey && onGround_)
    {
        jumpQueued_ = true;
    }

    if (event.key.code == sf::Keyboard::Space && !attacking_ && !sliding_)
    {
        attackQueued_ = true;
    }

    if (event.key.code == sf::Keyboard::LAlt && onGround_ && !attacking_ && !sliding_)
    {
        slideQueued_ = true;
    }
}

void Player::update(float deltaTime, const std::vector<sf::FloatRect>& solidColliders, const sf::FloatRect& worldBounds)
{
    applyInput();
    moveHorizontally(deltaTime, solidColliders);
    moveVertically(deltaTime, solidColliders);
    keepInsideWorld(worldBounds);
    updateActionTimers(deltaTime);
    updateAnimation(deltaTime);
    syncDrawable();
}

void Player::draw(sf::RenderTarget& target) const
{
    if (currentAnimation() != nullptr)
    {
        target.draw(sprite_);
    }
    else
    {
        target.draw(placeholder_);
    }
}

void Player::setFeetPosition(sf::Vector2f feetPosition)
{
    position_.x = feetPosition.x - colliderSize_.x * 0.5f;
    position_.y = feetPosition.y - colliderSize_.y;
    velocity_ = {};
    onGround_ = true;
    jumpQueued_ = false;
    attackQueued_ = false;
    slideQueued_ = false;
    attacking_ = false;
    sliding_ = false;
    attackTimer_ = 0.f;
    slideTimer_ = 0.f;
    syncDrawable();
}

sf::Vector2f Player::getCenter() const
{
    return {position_.x + colliderSize_.x * 0.5f, position_.y + colliderSize_.y * 0.5f};
}

sf::FloatRect Player::getBounds() const
{
    return {position_, colliderSize_};
}

void Player::loadAnimations()
{
    loadFrameSeries(AnimationState::Idle, "Animations/Player/idle", "Warrior_Idle_", 1, 6, 0.14f, true);
    loadFrameSeries(AnimationState::Run, "Animations/Player/Run", "Warrior_Run_", 1, 8, 0.08f, true);
    loadFrameSeries(AnimationState::Jump, "Animations/Player/Jump", "Warrior_Jump_", 1, 3, 0.10f, false);
    loadFrameSeries(AnimationState::Fall, "Animations/Player/Fall", "Warrior_Fall_", 1, 3, 0.10f, true);
    loadFrameSeries(AnimationState::Attack, "Animations/Player/Attack", "Warrior_Attack_", 1, 12, 0.055f, false);
    loadFrameSeries(AnimationState::Slide, "Animations/Player/Slide", "Warrior-Slide_", 1, 5, 0.07f, false);
}

void Player::loadFrameSeries(AnimationState state, const std::string& directory, const std::string& prefix, int first, int last, float frameTime, bool loop)
{
    Animation animation;
    animation.frameTime = frameTime;
    animation.loop = loop;

    for (int frame = first; frame <= last; ++frame)
    {
        sf::Texture texture;
        texture.setSmooth(false);

        const auto path = AssetPaths::resolve(directory + "/" + prefix + std::to_string(frame) + ".png");
        if (texture.loadFromFile(path.string()))
        {
            texture.setSmooth(false);
            animation.frames.push_back(texture);
        }
    }

    if (!animation.frames.empty())
    {
        animations_[state] = animation;
    }
}

void Player::applyInput()
{
    if (attackQueued_ && !attacking_ && !sliding_)
    {
        attacking_ = true;
        attackTimer_ = animationDuration(AnimationState::Attack, 0.45f);
        setAnimationState(AnimationState::Attack);
    }

    if (slideQueued_ && onGround_ && !attacking_ && !sliding_)
    {
        sliding_ = true;
        slideTimer_ = slideDuration_;
        slideDirection_ = facingRight_ ? 1.f : -1.f;
        setAnimationState(AnimationState::Slide);
    }

    attackQueued_ = false;
    slideQueued_ = false;

    if (sliding_)
    {
        velocity_.x = slideDirection_ * slideSpeed_;
        jumpQueued_ = false;
        return;
    }

    if (attacking_)
    {
        velocity_.x = 0.f;
        jumpQueued_ = false;
        return;
    }

    const bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    const bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    velocity_.x = 0.f;
    if (leftPressed != rightPressed)
    {
        velocity_.x = leftPressed ? -moveSpeed_ : moveSpeed_;
        facingRight_ = rightPressed;
    }

    if (jumpQueued_ && onGround_)
    {
        velocity_.y = jumpVelocity_;
        onGround_ = false;
    }
    jumpQueued_ = false;
}

void Player::moveHorizontally(float deltaTime, const std::vector<sf::FloatRect>& solidColliders)
{
    position_.x += velocity_.x * deltaTime;
    sf::FloatRect bounds = getBounds();

    for (const auto& solid : solidColliders)
    {
        if (!bounds.intersects(solid))
        {
            continue;
        }

        const float verticalOverlapFromFeet = bounds.top + bounds.height - solid.top;
        if (onGround_ && verticalOverlapFromFeet > 0.f && verticalOverlapFromFeet <= maxStepHeight_)
        {
            position_.y -= verticalOverlapFromFeet;
            bounds = getBounds();
            continue;
        }

        if (velocity_.x > 0.f)
        {
            position_.x = solid.left - colliderSize_.x;
        }
        else if (velocity_.x < 0.f)
        {
            position_.x = solid.left + solid.width;
        }

        velocity_.x = 0.f;
        bounds = getBounds();
    }
}

void Player::moveVertically(float deltaTime, const std::vector<sf::FloatRect>& solidColliders)
{
    velocity_.y += gravity_ * deltaTime;
    position_.y += velocity_.y * deltaTime;
    onGround_ = false;

    sf::FloatRect bounds = getBounds();
    for (const auto& solid : solidColliders)
    {
        if (!bounds.intersects(solid))
        {
            continue;
        }

        if (velocity_.y > 0.f)
        {
            position_.y = solid.top - colliderSize_.y;
            onGround_ = true;
        }
        else if (velocity_.y < 0.f)
        {
            position_.y = solid.top + solid.height;
        }

        velocity_.y = 0.f;
        bounds = getBounds();
    }
}

void Player::keepInsideWorld(const sf::FloatRect& worldBounds)
{
    if (position_.x < worldBounds.left)
    {
        position_.x = worldBounds.left;
        velocity_.x = 0.f;
    }

    const float maxX = worldBounds.left + worldBounds.width - colliderSize_.x;
    if (position_.x > maxX)
    {
        position_.x = maxX;
        velocity_.x = 0.f;
    }

    const float maxY = worldBounds.top + worldBounds.height - colliderSize_.y;
    if (position_.y > maxY)
    {
        position_.y = maxY;
        velocity_.y = 0.f;
        onGround_ = true;
    }
}

void Player::updateActionTimers(float deltaTime)
{
    if (attacking_)
    {
        attackTimer_ -= deltaTime;
        if (attackTimer_ <= 0.f)
        {
            attacking_ = false;
            attackTimer_ = 0.f;
        }
    }

    if (sliding_)
    {
        slideTimer_ -= deltaTime;
        if (slideTimer_ <= 0.f || !onGround_)
        {
            sliding_ = false;
            slideTimer_ = 0.f;
        }
    }
}

void Player::updateAnimation(float deltaTime)
{
    if (sliding_)
    {
        setAnimationState(AnimationState::Slide);
    }
    else if (attacking_)
    {
        setAnimationState(AnimationState::Attack);
    }
    else if (!onGround_)
    {
        setAnimationState(velocity_.y < 0.f ? AnimationState::Jump : AnimationState::Fall);
    }
    else if (std::abs(velocity_.x) > 1.f)
    {
        setAnimationState(AnimationState::Run);
    }
    else
    {
        setAnimationState(AnimationState::Idle);
    }

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

void Player::setAnimationState(AnimationState state)
{
    if (currentState_ == state)
    {
        return;
    }

    currentState_ = state;
    currentFrame_ = 0;
    animationAccumulator_ = 0.f;
}

const Player::Animation* Player::currentAnimation() const
{
    const auto found = animations_.find(currentState_);
    if (found == animations_.end() || found->second.frames.empty())
    {
        return nullptr;
    }

    return &found->second;
}

float Player::animationDuration(AnimationState state, float fallbackDuration) const
{
    const auto found = animations_.find(state);
    if (found == animations_.end() || found->second.frames.empty())
    {
        return fallbackDuration;
    }

    return static_cast<float>(found->second.frames.size()) * found->second.frameTime;
}

void Player::syncDrawable()
{
    placeholder_.setPosition(position_);

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
