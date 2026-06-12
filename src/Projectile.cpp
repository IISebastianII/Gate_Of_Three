#include "Projectile.h"

#include "AssetPaths.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <utility>

namespace
{
sf::Vector2f normalizeDirection(sf::Vector2f direction)
{
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length <= 0.0001f)
    {
        return {1.f, 0.f};
    }

    return {direction.x / length, direction.y / length};
}
}

Projectile::Projectile(sf::Vector2f startPosition, sf::Vector2f direction, float speed, int damage)
    : direction_(normalizeDirection(direction))
    , damage_(damage)
{
    beamLength_ = std::max(0.f, speed);
    anchorPosition_ = startPosition;
    boundsSize_ = {beamLength_, projectileHeight_};
    position_ = {
        direction_.x >= 0.f ? startPosition.x : startPosition.x - beamLength_,
        startPosition.y - projectileHeight_ * 0.5f
    };
    velocity_ = {0.f, 0.f};

    loadVisualAnimation();

    glowShape_.setRadius(projectileHeight_ * 0.5f);
    glowShape_.setOrigin(projectileHeight_ * 0.5f, projectileHeight_ * 0.5f);
    glowShape_.setFillColor(sf::Color(170, 95, 255, 70));
    glowShape_.setOutlineThickness(4.f);
    glowShape_.setOutlineColor(sf::Color(230, 210, 255, 90));

    coreShape_.setRadius(projectileHeight_ * 0.24f);
    coreShape_.setOrigin(projectileHeight_ * 0.24f, projectileHeight_ * 0.24f);
    coreShape_.setFillColor(sf::Color(245, 232, 255));

    syncDrawable();
}

void Projectile::update(float deltaTime)
{
    lifetime_ += deltaTime;

    if (hasAnimatedVisual_ && !frames_.empty())
    {
        frameAccumulator_ += deltaTime;
        while (frameAccumulator_ >= visualFrameTime_)
        {
            frameAccumulator_ -= visualFrameTime_;
            currentFrame_ = (currentFrame_ + 1) % frames_.size();
        }
    }

    if (lifetime_ >= maxLifetime_)
    {
        destroy();
    }

    syncDrawable();
}

void Projectile::draw(sf::RenderTarget& target) const
{
    if (hasAnimatedVisual_ && !frames_.empty())
    {
        target.draw(sprite_);
        return;
    }

    target.draw(glowShape_);
    target.draw(coreShape_);
}

void Projectile::setBeamLength(float beamLength)
{
    beamLength_ = std::max(0.f, beamLength);
    boundsSize_ = {beamLength_, projectileHeight_};
    position_.x = direction_.x >= 0.f ? anchorPosition_.x : anchorPosition_.x - beamLength_;
    position_.y = anchorPosition_.y - projectileHeight_ * 0.5f;
    syncDrawable();
}

int Projectile::getDamage() const
{
    return damage_;
}

sf::Vector2f Projectile::getDirection() const
{
    return direction_;
}

bool Projectile::hasExpired() const
{
    return lifetime_ >= maxLifetime_;
}

void Projectile::loadVisualAnimation()
{
    const std::filesystem::path directory = AssetPaths::resolve("Animations/Player/long_speel_animation");
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        hasAnimatedVisual_ = false;
        return;
    }

    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".png")
        {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());
    for (const auto& file : files)
    {
        sf::Texture texture;
        texture.setSmooth(false);
        if (texture.loadFromFile(file.string()))
        {
            frames_.push_back(std::move(texture));
        }
    }

    hasAnimatedVisual_ = !frames_.empty();
    if (hasAnimatedVisual_)
    {
        sprite_.setTexture(frames_.front(), true);
    }
}

void Projectile::syncDrawable()
{
    const sf::Vector2f center = {
        position_.x + beamLength_ * 0.5f,
        position_.y + projectileHeight_ * 0.5f
    };

    if (hasAnimatedVisual_ && !frames_.empty())
    {
        const sf::Texture& texture = frames_[std::min(currentFrame_, frames_.size() - 1)];
        sprite_.setTexture(texture, true);
        sprite_.setOrigin(
            static_cast<float>(texture.getSize().x) * 0.5f,
            static_cast<float>(texture.getSize().y) * 0.5f);
        sprite_.setPosition(center);
        sprite_.setColor(sf::Color(245, 240, 255));
        const float baseScaleX = beamLength_ / std::max(1.f, static_cast<float>(texture.getSize().x));
        const float scaleX = direction_.x >= 0.f ? baseScaleX : -baseScaleX;
        sprite_.setScale(scaleX, visualScale_);
        return;
    }

    glowShape_.setPosition(center);
    coreShape_.setPosition(center);
}
