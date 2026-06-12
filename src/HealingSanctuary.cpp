#include "HealingSanctuary.h"

#include "AssetPaths.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
constexpr float pi = 3.14159265f;
}

HealingSanctuary::HealingSanctuary(sf::Vector2f feetPosition)
    : feetPosition_(feetPosition)
    , bounds_({
        feetPosition.x - boundsWidth_ * 0.5f,
        feetPosition.y - boundsHeight_,
        boundsWidth_,
        boundsHeight_})
{
    hasTexture_ = texture_.loadFromFile(AssetPaths::resolve("decors/Sanctuary_Healing_Statue.png").string());
    texture_.setSmooth(false);
    if (hasTexture_)
    {
        sprite_.setTexture(texture_, true);
        const sf::Vector2u size = texture_.getSize();
        sprite_.setOrigin(static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y));
        sprite_.setScale(textureScale_, textureScale_);
        sprite_.setPosition(feetPosition_);
    }

    placeholder_.setSize({boundsWidth_, boundsHeight_});
    placeholder_.setOrigin(boundsWidth_ * 0.5f, boundsHeight_);
    placeholder_.setPosition(feetPosition_);
    placeholder_.setFillColor(sf::Color(75, 145, 110));

    outerRing_.setRadius(34.f);
    outerRing_.setOrigin(34.f, 34.f);
    outerRing_.setFillColor(sf::Color::Transparent);
    outerRing_.setOutlineThickness(4.f);

    innerRing_.setRadius(22.f);
    innerRing_.setOrigin(22.f, 22.f);
    innerRing_.setFillColor(sf::Color::Transparent);
    innerRing_.setOutlineThickness(3.f);

    for (std::size_t index = 0; index < particles_.size(); ++index)
    {
        const float radius = index % 2 == 0 ? 4.f : 3.f;
        particles_[index].setRadius(radius);
        particles_[index].setOrigin(radius, radius);
    }

    updateEffects();
}

void HealingSanctuary::update(float deltaTime)
{
    effectTimer_ = std::max(0.f, effectTimer_ - deltaTime);
    updateEffects();
}

void HealingSanctuary::draw(sf::RenderTarget& target) const
{
    if (hasTexture_)
    {
        target.draw(sprite_);
    }
    else
    {
        target.draw(placeholder_);
    }

    if (effectTimer_ <= 0.f)
    {
        return;
    }

    target.draw(outerRing_);
    target.draw(innerRing_);
    for (const auto& particle : particles_)
    {
        target.draw(particle);
    }
}

sf::FloatRect HealingSanctuary::getBounds() const
{
    return bounds_;
}

void HealingSanctuary::interact()
{
    healingRequested_ = true;
    effectTimer_ = effectDuration_;
    updateEffects();
}

bool HealingSanctuary::consumeHealingRequest()
{
    return std::exchange(healingRequested_, false);
}

void HealingSanctuary::updateEffects()
{
    const float progress = effectTimer_ > 0.f
        ? 1.f - effectTimer_ / effectDuration_
        : 0.f;
    const float pulse = 0.88f + std::sin(progress * pi * 6.f) * 0.12f;
    const sf::Vector2f center = {feetPosition_.x, feetPosition_.y - 92.f};
    const sf::Uint8 alpha = effectTimer_ > 0.f
        ? static_cast<sf::Uint8>(200.f * std::min(1.f, effectTimer_ * 2.f))
        : 0;

    outerRing_.setPosition(center);
    outerRing_.setScale(pulse * (1.f + progress * 0.55f), pulse * (1.f + progress * 0.55f));
    outerRing_.setOutlineColor(sf::Color(120, 255, 170, alpha));

    innerRing_.setPosition(center);
    innerRing_.setScale(1.15f - progress * 0.25f, 1.15f - progress * 0.25f);
    innerRing_.setOutlineColor(sf::Color(255, 230, 125, alpha));

    for (std::size_t index = 0; index < particles_.size(); ++index)
    {
        const float phase = static_cast<float>(index) / static_cast<float>(particles_.size());
        const float angle = phase * pi * 2.f + progress * pi * 3.f;
        const float orbit = 28.f + static_cast<float>(index % 3) * 10.f;
        const float rise = std::fmod(progress + phase, 1.f) * 105.f;
        particles_[index].setPosition({
            center.x + std::cos(angle) * orbit,
            feetPosition_.y - 20.f - rise});
        particles_[index].setFillColor(index % 2 == 0
            ? sf::Color(130, 255, 175, alpha)
            : sf::Color(255, 235, 135, alpha));
    }
}
