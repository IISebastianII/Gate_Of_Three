#include "BossRoom.h"

#include "AdditionalRoomMapData.h"
#include "AssetPaths.h"
#include "Boss.h"

#include <utility>

BossRoom::BossRoom()
    : Room(RoomType::Boss)
{
    roomSize_ = {1280.f, 900.f};
    clearColor_ = sf::Color(18, 22, 30);
    loadTextures();
    buildGeometry();
}

void BossRoom::draw(sf::RenderTarget& target) const
{
    sf::RectangleShape caveBackground(roomSize_);
    caveBackground.setFillColor(clearColor_);
    target.draw(caveBackground);

    if (hasFloorTiles_)
    {
        for (const auto& tile : tiles_)
        {
            target.draw(tile);
        }
    }
    else
    {
        target.draw(fallbackGround_);
    }

    Room::draw(target);
}

sf::Vector2f BossRoom::getPlayerSpawnFeet() const
{
    return {160.f, groundTop_};
}

void BossRoom::loadTextures()
{
    hasMapAtlas_ = mapAtlasTexture_.loadFromFile(AssetPaths::resolve("Maps/battle_room_1_spritesheet.png").string());
    mapAtlasTexture_.setSmooth(false);

    loadTexture("rockDirt", "Tiles/rocks/dirt_inside_filling.png");
    loadTexture("wall1", "Tiles/rocks/rocks_wall_1.png");
    loadTexture("wall2", "Tiles/rocks/rocks_wall_2.png");
    loadTexture("ceiling", "Tiles/rocks/rock_celling.png");
    loadTexture("leftCorner", "Tiles/rocks/rock_left_celling_corner.png");
    loadTexture("rightCorner", "Tiles/rocks/rock_right_celling_corner.png");
    loadTexture("ceilingSpikes", "Tiles/rocks/celling_cavespikes.png");
    loadTexture("sign", "decors/sign.png");

    hasFloorTiles_ = textures_.count("rockDirt") > 0;
}

void BossRoom::buildGeometry()
{
    constexpr float mapTop = groundTop_ - 13.f * tileSize_;
    solidColliders_.clear();
    tiles_.clear();
    solidColliders_.push_back({0.f, groundTop_, roomSize_.x, roomSize_.y - groundTop_});
    solidColliders_.push_back({roomSize_.x - tileSize_, 0.f, tileSize_, groundTop_});

    fallbackGround_.setSize({roomSize_.x, roomSize_.y - groundTop_});
    fallbackGround_.setPosition(0.f, groundTop_);
    fallbackGround_.setFillColor(sf::Color(48, 52, 62));

    if (hasMapAtlas_)
    {
        for (const auto& tile : bossRoomTiles)
        {
            if (tile.id < 72)
            {
                addAtlasTile(tile.id, tile.x, tile.y, mapTop);
                continue;
            }

            const char* textureId = "rockDirt";
            if (tile.id == 74 || tile.id == 76)
            {
                textureId = "wall1";
            }
            else if (tile.id == 75)
            {
                textureId = "leftCorner";
            }
            else if (tile.id == 77)
            {
                textureId = "rightCorner";
            }
            addTile(textureId, {
                static_cast<float>(tile.x) * tileSize_,
                mapTop + static_cast<float>(tile.y) * tileSize_});
        }
    }

    addExit(
        RoomType::Heal,
        {1120.f, 452.f},
        {0.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});

    addObject<Boss>(sf::Vector2f{roomSize_.x * 0.68f, groundTop_});
}

void BossRoom::loadTexture(const std::string& id, const std::string& relativePath)
{
    sf::Texture texture;
    if (!texture.loadFromFile(AssetPaths::resolve(relativePath).string()))
    {
        return;
    }

    texture.setSmooth(false);
    textures_[id] = std::move(texture);
}

void BossRoom::addTile(const std::string& textureId, sf::Vector2f position)
{
    const auto found = textures_.find(textureId);
    if (found == textures_.end())
    {
        return;
    }

    sf::Sprite tile(found->second);
    tile.setScale(tileScale_, tileScale_);
    tile.setPosition(position);
    tiles_.push_back(tile);
}

void BossRoom::addAtlasTile(int id, int column, int row, float mapTop)
{
    constexpr int atlasColumns = 8;
    constexpr int sourceTileSize = 32;
    sf::Sprite tile(mapAtlasTexture_);
    tile.setTextureRect({
        (id % atlasColumns) * sourceTileSize,
        (id / atlasColumns) * sourceTileSize,
        sourceTileSize,
        sourceTileSize});
    tile.setScale(tileScale_, tileScale_);
    tile.setPosition(static_cast<float>(column) * tileSize_, mapTop + static_cast<float>(row) * tileSize_);
    tiles_.push_back(tile);
}
