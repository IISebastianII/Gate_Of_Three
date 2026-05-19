#include "StaticNpc.h"

#include "AssetPaths.h"

#include <algorithm>

StaticNpc::StaticNpc()
{
    loadCatIdle();
    syncPlaceholder();
    syncDrawable();
}

void StaticNpc::update(float deltaTime)
{
    if (frames_.empty())
    {
        return;
    }

    animationAccumulator_ += deltaTime;
    while (animationAccumulator_ >= frameTime_)
    {
        animationAccumulator_ -= frameTime_;
        currentFrame_ = (currentFrame_ + 1) % frames_.size();
    }

    syncDrawable();
}

void StaticNpc::draw(sf::RenderTarget& target) const
{
    if (!frames_.empty())
    {
        target.draw(sprite_);
        return;
    }

    target.draw(tail_);
    target.draw(body_);
    target.draw(head_);
    target.draw(leftEar_);
    target.draw(rightEar_);
    target.draw(leftEye_);
    target.draw(rightEye_);
}

void StaticNpc::setFeetPosition(sf::Vector2f feetPosition)
{
    feetPosition_ = feetPosition;
    syncPlaceholder();
    syncDrawable();
}

bool StaticNpc::loadCatIdle()
{
    return loadFramesFromDirectory("Animations/NPC/cat_NPC/Idle")
        || loadFramesFromDirectory("Animations/NPC/cat_NPC/idle")
        || loadFramesFromDirectory("Animations/NPC/cat_NPC");
}

bool StaticNpc::loadFramesFromDirectory(const std::string& relativeDirectory)
{
    const std::filesystem::path directory = AssetPaths::resolve(relativeDirectory);
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        return false;
    }

    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const std::filesystem::path path = entry.path();
        if (path.extension() == ".png")
        {
            files.push_back(path);
        }
    }

    std::sort(files.begin(), files.end());
    for (const auto& file : files)
    {
        sf::Image image;
        if (!image.loadFromFile(file.string()))
        {
            continue;
        }

        Frame frame;
        frame.visibleBounds = findVisibleBounds(image);
        if (frame.texture.loadFromImage(image))
        {
            frame.texture.setSmooth(false);
            frames_.push_back(std::move(frame));
        }
    }

    return !frames_.empty();
}

sf::IntRect StaticNpc::findVisibleBounds(const sf::Image& image) const
{
    const sf::Vector2u size = image.getSize();
    unsigned minX = size.x;
    unsigned minY = size.y;
    unsigned maxX = 0;
    unsigned maxY = 0;
    bool hasVisiblePixels = false;

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

    if (!hasVisiblePixels)
    {
        return {0, 0, static_cast<int>(size.x), static_cast<int>(size.y)};
    }

    return {
        static_cast<int>(minX),
        static_cast<int>(minY),
        static_cast<int>(maxX - minX + 1),
        static_cast<int>(maxY - minY + 1)
    };
}

void StaticNpc::syncDrawable()
{
    if (frames_.empty())
    {
        return;
    }

    const Frame& frame = frames_[std::min(currentFrame_, frames_.size() - 1)];
    sprite_.setTexture(frame.texture, true);
    sprite_.setOrigin(
        static_cast<float>(frame.visibleBounds.left) + static_cast<float>(frame.visibleBounds.width) * 0.5f,
        static_cast<float>(frame.visibleBounds.top + frame.visibleBounds.height)
    );
    sprite_.setScale(textureScale_, textureScale_);
    sprite_.setPosition(feetPosition_.x, feetPosition_.y + 1.f);
}

void StaticNpc::syncPlaceholder()
{
    const sf::Color bodyColor(94, 104, 114);
    const sf::Color darkColor(42, 48, 56);
    const sf::Color eyeColor(206, 225, 132);

    body_.setSize({62.f, 28.f});
    body_.setFillColor(bodyColor);
    body_.setPosition(feetPosition_.x - 34.f, feetPosition_.y - 28.f);

    head_.setSize({32.f, 30.f});
    head_.setFillColor(bodyColor);
    head_.setPosition(feetPosition_.x + 12.f, feetPosition_.y - 54.f);

    tail_.setSize({10.f, 42.f});
    tail_.setFillColor(bodyColor);
    tail_.setOrigin(5.f, 38.f);
    tail_.setPosition(feetPosition_.x - 36.f, feetPosition_.y - 22.f);
    tail_.setRotation(-28.f);

    leftEar_.setPointCount(3);
    leftEar_.setPoint(0, {0.f, 18.f});
    leftEar_.setPoint(1, {9.f, 0.f});
    leftEar_.setPoint(2, {18.f, 18.f});
    leftEar_.setFillColor(bodyColor);
    leftEar_.setPosition(feetPosition_.x + 14.f, feetPosition_.y - 66.f);

    rightEar_.setPointCount(3);
    rightEar_.setPoint(0, {0.f, 18.f});
    rightEar_.setPoint(1, {9.f, 0.f});
    rightEar_.setPoint(2, {18.f, 18.f});
    rightEar_.setFillColor(bodyColor);
    rightEar_.setPosition(feetPosition_.x + 28.f, feetPosition_.y - 66.f);

    leftEye_.setRadius(2.6f);
    leftEye_.setFillColor(eyeColor);
    leftEye_.setPosition(feetPosition_.x + 20.f, feetPosition_.y - 43.f);

    rightEye_.setRadius(2.6f);
    rightEye_.setFillColor(eyeColor);
    rightEye_.setPosition(feetPosition_.x + 33.f, feetPosition_.y - 43.f);

    body_.setOutlineColor(darkColor);
    body_.setOutlineThickness(2.f);
    head_.setOutlineColor(darkColor);
    head_.setOutlineThickness(2.f);
}
