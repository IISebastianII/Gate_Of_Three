#include "RadialHealthIndicator.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float pi = 3.14159265f;
constexpr float startAngle = -90.f;

float toRadians(float degrees)
{
    return degrees * pi / 180.f;
}
}

RadialHealthIndicator::RadialHealthIndicator()
    : fill_(sf::TriangleFan)
{
    background_.setRadius(radius_);
    background_.setOrigin(radius_, radius_);
    background_.setFillColor(sf::Color(95, 95, 95));

    outline_.setRadius(radius_);
    outline_.setOrigin(radius_, radius_);
    outline_.setFillColor(sf::Color::Transparent);
    outline_.setOutlineColor(sf::Color(10, 10, 10));
    outline_.setOutlineThickness(3.f);

    rebuildFill();
}

void RadialHealthIndicator::setPosition(sf::Vector2f center)
{
    center_ = center;
    background_.setPosition(center_);
    outline_.setPosition(center_);
    rebuildFill();
}

void RadialHealthIndicator::setValue(float value)
{
    value_ = std::clamp(value, 0.f, 1.f);
    rebuildFill();
}

void RadialHealthIndicator::draw(sf::RenderTarget& target) const
{
    target.draw(background_);
    if (value_ > 0.f)
    {
        target.draw(fill_);
    }
    target.draw(outline_);
}

void RadialHealthIndicator::rebuildFill()
{
    fill_.clear();
    fill_.append(sf::Vertex(center_, sf::Color(185, 30, 36)));

    const unsigned visibleSegments = std::max(1u, static_cast<unsigned>(std::ceil(segmentCount_ * value_)));
    for (unsigned segment = 0; segment <= visibleSegments; ++segment)
    {
        const float t = static_cast<float>(segment) / static_cast<float>(visibleSegments);
        const float angle = toRadians(startAngle + 360.f * value_ * t);
        const sf::Vector2f point = {
            center_.x + std::cos(angle) * radius_,
            center_.y + std::sin(angle) * radius_
        };
        fill_.append(sf::Vertex(point, sf::Color(220, 37, 43)));
    }
}
