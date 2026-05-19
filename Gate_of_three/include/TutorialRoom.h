#pragma once

#include "StaticNpc.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

class TutorialRoom
{
public:
    TutorialRoom();

    void update(float deltaTime);
    void draw(sf::RenderTarget& target) const;

    const std::vector<sf::FloatRect>& getSolidColliders() const;
    sf::FloatRect getBounds() const;
    sf::Vector2f getPlayerSpawnFeet() const;

private:
    void loadTextures();
    void buildGeometry();
    void loadTexture(const std::string& id, const std::string& relativePath);
    void addTile(const std::string& textureId, int column, int visualRow);
    void addSprite(const std::string& textureId, sf::Vector2f bottomCenter, float scale = tileScale_);
    void addTiledPlatform(const sf::FloatRect& bounds);

    static constexpr float tileScale_ = 2.f;
    static constexpr float tileSize_ = 64.f;
    static constexpr float mapTop_ = 516.f;
    static constexpr float baseGroundTop_ = mapTop_ + tileSize_ * 4.f;
    static constexpr float lowStepTop_ = mapTop_ + tileSize_ * 3.f;
    static constexpr float plateauTop_ = mapTop_ + tileSize_ * 2.f;

    sf::Vector2f roomSize_ = {2048.f, 900.f};
    sf::Color skyColor_ = sf::Color(238, 241, 242);

    sf::Texture backgroundTexture_;
    std::map<std::string, sf::Texture> textures_;
    bool hasBackground_ = false;
    bool hasTiles_ = false;

    sf::Sprite background_;
    std::vector<sf::Sprite> tiles_;
    std::vector<sf::Sprite> decors_;
    std::vector<sf::RectangleShape> fallbackPlatforms_;
    std::vector<sf::FloatRect> solidColliders_;
    StaticNpc catNpc_;
};
