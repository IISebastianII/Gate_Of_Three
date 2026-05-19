#pragma once

#include <SFML/Graphics.hpp>

#include <filesystem>
#include <vector>

class StaticNpc
{
public:
    StaticNpc();

    void update(float deltaTime);
    void draw(sf::RenderTarget& target) const;
    void setFeetPosition(sf::Vector2f feetPosition);

private:
    struct Frame
    {
        sf::Texture texture;
        sf::IntRect visibleBounds;
    };

    bool loadCatIdle();
    bool loadFramesFromDirectory(const std::string& relativeDirectory);
    sf::IntRect findVisibleBounds(const sf::Image& image) const;
    void syncDrawable();
    void syncPlaceholder();

    static constexpr float textureScale_ = 2.4f;

    std::vector<Frame> frames_;
    sf::Sprite sprite_;
    std::size_t currentFrame_ = 0;
    float frameTime_ = 0.16f;
    float animationAccumulator_ = 0.f;
    sf::Vector2f feetPosition_ = {0.f, 0.f};

    sf::RectangleShape body_;
    sf::RectangleShape head_;
    sf::RectangleShape tail_;
    sf::ConvexShape leftEar_;
    sf::ConvexShape rightEar_;
    sf::CircleShape leftEye_;
    sf::CircleShape rightEye_;
};
