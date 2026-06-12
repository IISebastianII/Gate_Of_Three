#include "Enemy.h"

#include "AssetPaths.h"
#include "Player.h"

#include <algorithm>
#include <cmath>
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
    spawnFeetX_ = feetPosition.x;

    debugShape_.setSize(colliderSize_);
    debugShape_.setFillColor(sf::Color(120, 70, 80));
    debugShape_.setOutlineColor(sf::Color::White);
    debugShape_.setOutlineThickness(2.f);

    loadAnimations(animationRoot);
    syncDrawable();
}

void Enemy::update(float deltaTime)
{
    updateAnimation(deltaTime);
    syncDrawable();
}

void Enemy::update(float deltaTime, Player& player, const std::vector<sf::FloatRect>& solidColliders)
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
    else
    {
        updateAi(deltaTime, player);
        moveHorizontally(deltaTime, solidColliders);
        moveVertically(deltaTime, solidColliders);
        tryHitPlayer(player);
    }

    if (attackCooldown_ > 0.f)
    {
        attackCooldown_ = std::max(0.f, attackCooldown_ - deltaTime);
    }

    if (attacking_)
    {
        attackTimer_ -= deltaTime;
        if (attackTimer_ <= 0.f)
        {
            attacking_ = false;
            attackTimer_ = 0.f;
            attackDuration_ = 0.f;
            attackHitDone_ = false;
            attackCooldown_ = attackCooldownDuration_;
        }
    }

    if (dying_)
    {
        velocity_.x = 0.f;
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
    attacking_ = false;
    attackTimer_ = 0.f;
    attackDuration_ = 0.f;
    attackHitDone_ = false;

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

bool Enemy::canReceiveDamage() const
{
    return true;
}

void Enemy::loadAnimations(const std::string& animationRoot)
{
    loadFrameSeries(AnimationState::Idle, animationRoot + "/idle", 0, 7, 0.12f, true);
    loadFrameSeries(AnimationState::Walk, animationRoot + "/walk", 0, 9, 0.09f, true);
    loadFrameSeries(AnimationState::Attack, animationRoot + "/atack", 0, 9, 0.065f, false);
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

void Enemy::updateAi(float, Player& player)
{
    if (currentState_ == AnimationState::Hurt)
    {
        velocity_.x = 0.f;
        return;
    }

    const sf::Vector2f playerCenter = player.getCenter();
    const sf::Vector2f enemyCenter = getCenter();
    const sf::FloatRect playerBounds = player.getBounds();
    const sf::FloatRect enemyBounds = getBounds();
    const float distanceX = playerCenter.x - enemyCenter.x;
    const float distanceY = std::abs(playerCenter.y - enemyCenter.y);
    const float edgeGap = distanceX >= 0.f
        ? playerBounds.left - (enemyBounds.left + enemyBounds.width)
        : enemyBounds.left - (playerBounds.left + playerBounds.width);
    const float clampedEdgeGap = std::max(0.f, edgeGap);

    if (attacking_)
    {
        velocity_.x = 0.f;
        setAnimationState(AnimationState::Attack);
        return;
    }

    if (clampedEdgeGap <= attackRange_ && distanceY <= attackHeight_ && attackCooldown_ <= 0.f)
    {
        attacking_ = true;
        attackHitDone_ = false;
        attackDuration_ = animationDuration(AnimationState::Attack, 0.55f);
        attackTimer_ = attackDuration_;
        facingRight_ = distanceX > 0.f;
        velocity_.x = 0.f;
        setAnimationState(AnimationState::Attack);
        return;
    }

    if (std::abs(distanceX) <= detectionRange_ && distanceY <= 120.f)
    {
        facingRight_ = distanceX > 0.f;
        if (clampedEdgeGap <= preferredPlayerGap_)
        {
            velocity_.x = 0.f;
            setAnimationState(AnimationState::Idle);
            return;
        }

        velocity_.x = facingRight_ ? chaseSpeed_ : -chaseSpeed_;
        setAnimationState(AnimationState::Walk);
        return;
    }

    const float feetX = position_.x + colliderSize_.x * 0.5f;
    if (feetX < spawnFeetX_ - patrolRange_)
    {
        patrolDirection_ = 1.f;
    }
    else if (feetX > spawnFeetX_ + patrolRange_)
    {
        patrolDirection_ = -1.f;
    }

    facingRight_ = patrolDirection_ > 0.f;
    velocity_.x = patrolDirection_ * patrolSpeed_;
    setAnimationState(AnimationState::Walk);
}

void Enemy::moveHorizontally(float deltaTime, const std::vector<sf::FloatRect>& solidColliders)
{
    position_.x += velocity_.x * deltaTime;
    sf::FloatRect bounds = getBounds();

    for (const auto& solid : solidColliders)
    {
        if (!bounds.intersects(solid))
        {
            continue;
        }

        if (velocity_.x > 0.f)
        {
            position_.x = solid.left - colliderSize_.x;
            patrolDirection_ = -1.f;
        }
        else if (velocity_.x < 0.f)
        {
            position_.x = solid.left + solid.width;
            patrolDirection_ = 1.f;
        }

        velocity_.x = 0.f;
        bounds = getBounds();
    }
}

void Enemy::moveVertically(float deltaTime, const std::vector<sf::FloatRect>& solidColliders)
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

sf::Vector2f Enemy::getCenter() const
{
    return {position_.x + colliderSize_.x * 0.5f, position_.y + colliderSize_.y * 0.5f};
}

sf::FloatRect Enemy::getAttackBounds() const
{
    const float top = position_.y + (colliderSize_.y - attackHeight_) * 0.5f;
    if (facingRight_)
    {
        return {position_.x + colliderSize_.x - attackInset_, top, attackRange_ + attackInset_, attackHeight_};
    }

    return {position_.x - attackRange_, top, attackRange_ + attackInset_, attackHeight_};
}

bool Enemy::isAttackActive() const
{
    if (!attacking_ || attackDuration_ <= 0.f)
    {
        return false;
    }

    const float elapsed = attackDuration_ - attackTimer_;
    return elapsed >= attackActiveStart_ && elapsed <= attackActiveEnd_;
}

void Enemy::tryHitPlayer(Player& player)
{
    if (attackHitDone_ || !isAttackActive())
    {
        return;
    }

    if (getAttackBounds().intersects(player.getBounds()))
    {
        player.receiveDamage(attackDamage_, getCenter());
        attackHitDone_ = true;
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

float Enemy::animationDuration(AnimationState state, float fallbackDuration) const
{
    const auto found = animations_.find(state);
    if (found == animations_.end() || found->second.frames.empty())
    {
        return fallbackDuration;
    }

    return static_cast<float>(found->second.frames.size()) * found->second.frameTime;
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
