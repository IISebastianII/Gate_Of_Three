#pragma once

#include "GameObject.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

// Interactive chest with closed, opening and open states.
class Chest : public GameObject
{
public:
    explicit Chest(sf::Vector2f feetPosition);

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void interact() override;
    bool isOpen() const;

private:
    enum class State
    {
        Closed,
        Opening,
        Open
    };

    struct Frame
    {
        sf::Texture texture;
        sf::IntRect visibleBounds;
    };

    bool loadFrame(const std::string& relativePath, Frame& frame);
    void loadOpeningAnimation();
    void syncDrawable();

    static constexpr float textureScale_ = 2.f;
    static constexpr float frameTime_ = 0.06f;
    static constexpr float boundsWidth_ = 72.f;
    static constexpr float boundsHeight_ = 48.f;

    sf::Vector2f feetPosition_ = {0.f, 0.f};
    sf::FloatRect bounds_;
    State state_ = State::Closed;

    Frame closedFrame_;
    Frame openFrame_;
    std::vector<Frame> openingFrames_;
    std::size_t currentFrame_ = 0;
    float animationAccumulator_ = 0.f;
    bool hasVisual_ = false;

    sf::Sprite sprite_;
    sf::RectangleShape debugShape_;
};
