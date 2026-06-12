#pragma once

#include "Room.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

// First room of the game with basic terrain, an NPC and an exit.
class TutorialRoom : public Room
{
public:
    TutorialRoom();

    void update(float deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

    sf::Vector2f getPlayerSpawnFeet() const override;

private:
    void loadTextures();
    void buildGeometry();
    void loadTexture(const std::string& id, const std::string& relativePath);
    void addTile(const std::string& textureId, int column, int visualRow);
    void addSprite(const std::string& textureId, sf::Vector2f bottomCenter, float scale = tileScale_);

    static constexpr float tileScale_ = 2.f;
    static constexpr float tileSize_ = 64.f;
    static constexpr float mapTop_ = 516.f;
    static constexpr float baseGroundTop_ = mapTop_ + tileSize_ * 4.f;
    static constexpr float lowStepTop_ = mapTop_ + tileSize_ * 3.f;
    static constexpr float plateauTop_ = mapTop_ + tileSize_ * 2.f;

    sf::Texture backgroundTexture_;
    std::map<std::string, sf::Texture> textures_;
    bool hasBackground_ = false;
    bool hasTiles_ = false;

    sf::Sprite background_;
    std::vector<sf::Sprite> tiles_;
    std::vector<sf::Sprite> decors_;
    std::vector<sf::RectangleShape> fallbackPlatforms_;
};
