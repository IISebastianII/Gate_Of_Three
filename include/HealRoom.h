#pragma once

#include "Room.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

class HealRoom : public Room
{
public:
    HealRoom();

    void draw(sf::RenderTarget& target) const override;
    sf::Vector2f getPlayerSpawnFeet() const override;

private:
    void loadTextures();
    void buildGeometry();
    void loadTexture(const std::string& id, const std::string& relativePath);
    void addTile(const std::string& textureId, int column, int row);
    void addSprite(const std::string& textureId, sf::Vector2f bottomCenter, float scale);

    static constexpr float tileScale_ = 2.f;
    static constexpr float tileSize_ = 64.f;
    static constexpr float groundTop_ = 772.f;

    sf::Texture backgroundTexture_;
    std::map<std::string, sf::Texture> textures_;
    bool hasBackground_ = false;
    bool hasTiles_ = false;

    sf::Sprite background_;
    sf::RectangleShape ground_;
    std::vector<sf::Sprite> tiles_;
    std::vector<sf::Sprite> decors_;
};
