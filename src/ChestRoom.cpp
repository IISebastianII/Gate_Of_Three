#include "ChestRoom.h"

#include "AssetPaths.h"
#include "Chest.h"

ChestRoom::ChestRoom()
    : Room(RoomType::Chest)
{
    roomSize_ = {1280.f, 900.f};
    clearColor_ = sf::Color(68, 72, 82);
    loadTextures();
    buildGeometry();
}

void ChestRoom::draw(sf::RenderTarget& target) const
{
    if (hasTiles_)
    {
        for (const auto& tile : tiles_)
        {
            target.draw(tile);
        }
    }
    else
    {
        target.draw(ground_);
    }

    Room::draw(target);
}

sf::Vector2f ChestRoom::getPlayerSpawnFeet() const
{
    return {160.f, groundTop_};
}

void ChestRoom::loadTextures()
{
    loadTexture("grass", "Tiles/grass/grass.png");
    loadTexture("dirt", "Tiles/grass/dirt_inside_filling.png");

    hasTiles_ = textures_.count("grass") > 0 && textures_.count("dirt") > 0;
}

void ChestRoom::buildGeometry()
{
    solidColliders_.clear();
    tiles_.clear();
    solidColliders_.push_back({0.f, groundTop_, roomSize_.x, roomSize_.y - groundTop_});

    ground_.setSize({roomSize_.x, roomSize_.y - groundTop_});
    ground_.setPosition(0.f, groundTop_);
    ground_.setFillColor(sf::Color(42, 45, 52));

    if (hasTiles_)
    {
        const int columns = static_cast<int>(roomSize_.x / tileSize_);
        for (int column = 1; column <= columns; ++column)
        {
            addTile("grass", column, 1);
            addTile("dirt", column, 0);
        }
    }

    addObject<Chest>(sf::Vector2f{roomSize_.x * 0.5f, groundTop_});
}

void ChestRoom::loadTexture(const std::string& id, const std::string& relativePath)
{
    sf::Texture texture;
    if (!texture.loadFromFile(AssetPaths::resolve(relativePath).string()))
    {
        return;
    }

    texture.setSmooth(false);
    textures_[id] = std::move(texture);
}

void ChestRoom::addTile(const std::string& textureId, int column, int row)
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
