#include "Chest.h"

#include "AssetPaths.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace
{
std::string frameName(int frame)
{
    std::ostringstream stream;
    stream << "Open(DustFX)" << frame << ".png";
    return stream.str();
}
}

Chest::Chest(sf::Vector2f feetPosition)
    : feetPosition_(feetPosition)
    , bounds_({feetPosition.x - boundsWidth_ * 0.5f, feetPosition.y - boundsHeight_, boundsWidth_, boundsHeight_})
{
    debugShape_.setSize({boundsWidth_, boundsHeight_});
    debugShape_.setOrigin(boundsWidth_ * 0.5f, boundsHeight_);
    debugShape_.setPosition(feetPosition_);
    debugShape_.setFillColor(sf::Color(126, 78, 38));
    debugShape_.setOutlineColor(sf::Color(35, 22, 12));
    debugShape_.setOutlineThickness(2.f);

    loadFrame("Animations/Surrondings/Chest/Chest.png", closedFrame_);
    loadFrame("Animations/Surrondings/Chest/ChestOpen.png", openFrame_);
    loadOpeningAnimation();
    syncDrawable();
}

void Chest::update(float deltaTime)
{
    if (state_ != State::Opening || openingFrames_.empty())
    {
        syncDrawable();
        return;
    }

    animationAccumulator_ += deltaTime;
    while (animationAccumulator_ >= frameTime_)
    {
        animationAccumulator_ -= frameTime_;
        if (currentFrame_ + 1 < openingFrames_.size())
        {
            ++currentFrame_;
        }
        else
        {
            state_ = State::Open;
            currentFrame_ = openingFrames_.size() - 1;
            animationAccumulator_ = 0.f;
            break;
        }
    }

    syncDrawable();
}

void Chest::draw(sf::RenderTarget& target) const
{
    if (hasVisual_)
    {
        target.draw(sprite_);
    }
    else
    {
        target.draw(debugShape_);
    }
}

sf::FloatRect Chest::getBounds() const
{
    return bounds_;
}

bool Chest::isOpen() const
{
    return state_ == State::Open;
}

void Chest::interact()
{
    // The chest can only be opened once.
    if (state_ != State::Closed)
    {
        return;
    }

    if (openingFrames_.empty())
    {
        state_ = State::Open;
    }
    else
    {
        state_ = State::Opening;
        currentFrame_ = 0;
        animationAccumulator_ = 0.f;
    }

    syncDrawable();
}

bool Chest::loadFrame(const std::string& relativePath, Frame& frame)
{
    const auto path = AssetPaths::resolve(relativePath);

    sf::Image image;
    if (!image.loadFromFile(path.string()))
    {
        return false;
    }

    const sf::Vector2u size = image.getSize();
    unsigned minX = size.x;
    unsigned minY = size.y;
    unsigned maxX = 0;
    unsigned maxY = 0;
    bool hasVisiblePixels = false;

    // Transparent margins are ignored so every frame stays on the floor.
    for (unsigned y = 0; y < size.y; ++y)
    {
        for (unsigned x = 0; x < size.x; ++x)
        {
            if (image.getPixel(x, y).a == 0)
            {
                continue;
            }

            hasVisiblePixels = true;
            minX = std::min(minX, x);
            minY = std::min(minY, y);
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
        }
    }

    frame.visibleBounds = hasVisiblePixels
        ? sf::IntRect{
            static_cast<int>(minX),
            static_cast<int>(minY),
            static_cast<int>(maxX - minX + 1),
            static_cast<int>(maxY - minY + 1)}
        : sf::IntRect{0, 0, static_cast<int>(size.x), static_cast<int>(size.y)};

    if (!frame.texture.loadFromImage(image))
    {
        return false;
    }

    frame.texture.setSmooth(false);
    return true;
}

void Chest::loadOpeningAnimation()
{
    openingFrames_.clear();

    for (int frame = 1; frame <= 10; ++frame)
    {
        Frame animationFrame;
        const std::string relativePath = "Animations/Surrondings/Chest/Open_animation/" + frameName(frame);
        if (loadFrame(relativePath, animationFrame))
        {
            openingFrames_.push_back(std::move(animationFrame));
        }
    }
}

void Chest::syncDrawable()
{
    const Frame* activeFrame = nullptr;

    switch (state_)
    {
    case State::Closed:
        activeFrame = closedFrame_.texture.getSize().x > 0 ? &closedFrame_ : nullptr;
        break;
    case State::Opening:
        if (!openingFrames_.empty())
        {
            activeFrame = &openingFrames_[std::min(currentFrame_, openingFrames_.size() - 1)];
        }
        else if (closedFrame_.texture.getSize().x > 0)
        {
            activeFrame = &closedFrame_;
        }
        break;
    case State::Open:
        activeFrame = openFrame_.texture.getSize().x > 0 ? &openFrame_ : nullptr;
        break;
    }

    if (activeFrame != nullptr)
    {
        hasVisual_ = true;
        sprite_.setTexture(activeFrame->texture, true);
        sprite_.setOrigin(
            static_cast<float>(activeFrame->visibleBounds.left) + static_cast<float>(activeFrame->visibleBounds.width) * 0.5f,
            static_cast<float>(activeFrame->visibleBounds.top + activeFrame->visibleBounds.height));
        sprite_.setScale(textureScale_, textureScale_);
        sprite_.setPosition(feetPosition_.x, feetPosition_.y + 1.f);
        return;
    }

    hasVisual_ = false;
    debugShape_.setPosition(feetPosition_);

    switch (state_)
    {
    case State::Closed:
        debugShape_.setFillColor(sf::Color(126, 78, 38));
        break;
    case State::Opening:
        debugShape_.setFillColor(sf::Color(196, 130, 58));
        break;
    case State::Open:
        debugShape_.setFillColor(sf::Color(209, 181, 100));
        break;
    }
}
