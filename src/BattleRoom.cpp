#include "BattleRoom.h"

#include "AssetPaths.h"

BattleRoom::BattleRoom()
    : Room(RoomType::Battle)
{
    roomSize_ = {1600.f, 900.f};
    clearColor_ = sf::Color(222, 236, 239);
    loadTextures();
    buildGeometry();
}

void BattleRoom::draw(sf::RenderTarget& target) const
{
    sf::RectangleShape sky(roomSize_);
    sky.setFillColor(clearColor_);
    target.draw(sky);

    if (hasBackground_)
    {
        target.draw(background_);
    }

    if (hasTiles_)
    {
        for (const auto& tile : tiles_)
        {
            target.draw(tile);
        }
    }
    else
    {
        for (const auto& platform : fallbackPlatforms_)
        {
            target.draw(platform);
        }
    }

    Room::draw(target);
}

sf::Vector2f BattleRoom::getPlayerSpawnFeet() const
{
    return {160.f, groundTop_};
}

void BattleRoom::loadTextures()
{
    hasBackground_ = backgroundTexture_.loadFromFile(AssetPaths::resolve("backgrounds/forest_background.png").string());
    backgroundTexture_.setSmooth(false);

    loadTexture("grass", "Tiles/grass/grass.png");
    loadTexture("dirt", "Tiles/grass/dirt_inside_filling.png");
    loadTexture("sign", "decors/sign.png");

    hasTiles_ = textures_.count("grass") > 0 && textures_.count("dirt") > 0;
}

void BattleRoom::buildGeometry()
{
    solidColliders_.clear();
    solidColliders_.push_back({0.f, groundTop_, roomSize_.x, roomSize_.y - groundTop_});

    if (hasBackground_)
    {
        const auto textureSize = backgroundTexture_.getSize();
        const float scaleX = roomSize_.x / static_cast<float>(textureSize.x);
        const float scaleY = roomSize_.y / static_cast<float>(textureSize.y);
        const float scale = std::max(scaleX, scaleY);
        background_.setTexture(backgroundTexture_);
        background_.setScale(scale, scale);
        background_.setColor(sf::Color::White);
        background_.setPosition((roomSize_.x - static_cast<float>(textureSize.x) * scale) * 0.5f, 0.f);
    }

    if (hasTiles_)
    {
        const int columns = static_cast<int>(roomSize_.x / tileSize_);
        for (int column = 1; column <= columns; ++column)
        {
            addTile("grass", column, 1);
            addTile("dirt", column, 0);
        }
    }
    else
    {
        sf::RectangleShape ground({roomSize_.x, roomSize_.y - groundTop_});
        ground.setPosition(0.f, groundTop_);
        ground.setFillColor(sf::Color(74, 124, 70));
        fallbackPlatforms_.push_back(ground);
    }

    RoomExit& tutorialExit = addExit(
        RoomType::Tutorial,
        {1880.f, 772.f},
        {0.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    const auto signTexture = textures_.find("sign");
    if (signTexture != textures_.end())
    {
        tutorialExit.setTexture(signTexture->second, {tileSize_ * 1.5f, groundTop_}, tileScale_);
    }
}

void BattleRoom::loadTexture(const std::string& id, const std::string& relativePath)
{
    sf::Texture texture;
    if (!texture.loadFromFile(AssetPaths::resolve(relativePath).string()))
    {
        return;
    }

    texture.setSmooth(false);
    textures_[id] = std::move(texture);
}

void BattleRoom::addTile(const std::string& textureId, int column, int row)
{
    const auto found = textures_.find(textureId);
    if (found == textures_.end())
    {
        return;
    }

    sf::Sprite tile(found->second);
    tile.setScale(tileScale_, tileScale_);
    tile.setPosition(static_cast<float>(column - 1) * tileSize_, groundTop_ - static_cast<float>(row - 1) * tileSize_);
    tiles_.push_back(tile);
}
