#pragma once

#include "Room.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <string>
#include <vector>

class BossRoom : public Room
{
public:
    BossRoom();

    void draw(sf::RenderTarget& target) const override;
    sf::Vector2f getPlayerSpawnFeet() const override;

private:
    void loadTextures();
    void buildGeometry();
    void loadTexture(const std::string& id, const std::string& relativePath);
    void addTile(const std::string& textureId, sf::Vector2f position);

    static constexpr float tileScale_ = 2.f;
    static constexpr float tileSize_ = 64.f;
    static constexpr float groundTop_ = 772.f;

    std::map<std::string, sf::Texture> textures_;
    bool hasFloorTiles_ = false;

    sf::RectangleShape fallbackGround_;
    std::vector<sf::Sprite> tiles_;
};
