#include "HealRoom.h"

#include "AdditionalRoomMapData.h"
#include "AssetPaths.h"
#include "HealingSanctuary.h"
#include "Player.h"
#include "TerrainCollision.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>

namespace
{
bool isHealTerrainTile(int id)
{
    constexpr std::array<int, 25> terrainIds = {
        0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 27, 28, 29,
        33, 34, 35, 36, 37, 42, 43, 52, 53, 54, 63, 72};
    return std::find(terrainIds.begin(), terrainIds.end(), id) != terrainIds.end();
}
}

HealRoom::HealRoom()
    : Room(RoomType::Heal)
{
    roomSize_ = {1280.f, 900.f};
    clearColor_ = sf::Color(190, 220, 210);
    loadTextures();
    buildGeometry();
}

void HealRoom::update(float deltaTime, Player& player)
{
    Room::update(deltaTime, player);
    if (sanctuary_ != nullptr && sanctuary_->consumeHealingRequest())
    {
        player.healToFull();
    }
}

void HealRoom::draw(sf::RenderTarget& target) const
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
        target.draw(ground_);
    }

    for (const auto& decor : decors_)
    {
        target.draw(decor);
    }

    Room::draw(target);
}

sf::Vector2f HealRoom::getPlayerSpawnFeet() const
{
    return {32.f, groundTop_};
}

void HealRoom::loadTextures()
{
    hasBackground_ = backgroundTexture_.loadFromFile(AssetPaths::resolve("backgrounds/forest_background.png").string());
    backgroundTexture_.setSmooth(false);
    hasMapAtlas_ = mapAtlasTexture_.loadFromFile(AssetPaths::resolve("Maps/battle_room_1_spritesheet.png").string());
    mapAtlasTexture_.setSmooth(false);

    loadTexture("grass", "Tiles/grass/grass.png");
    loadTexture("dirt", "Tiles/grass/dirt_inside_filling.png");
    loadTexture("rockDirt", "Tiles/rocks/dirt_inside_filling.png");
    loadTexture("sign", "decors/sign.png");
    loadTexture("bush", "decors/bush.png");

    hasTiles_ = textures_.count("grass") > 0 && textures_.count("dirt") > 0;
}

void HealRoom::buildGeometry()
{
    constexpr int columns = 20;
    constexpr int bottomRow = 13;
    constexpr float mapTop = groundTop_ - static_cast<float>(bottomRow) * tileSize_;
    std::array<int, columns> surfaceRows;
    surfaceRows.fill(std::numeric_limits<int>::max());

    solidColliders_.clear();
    tiles_.clear();
    decors_.clear();

    for (const auto& tile : healRoomTiles)
    {
        const bool sanctuaryTile = tile.id == 57 || tile.id == 58 || tile.id == 61 || tile.id == 62;
        if (hasMapAtlas_ && tile.id < 72 && !sanctuaryTile)
        {
            addAtlasTile(tile.id, tile.x, tile.y, mapTop);
        }
        else if (tile.id == 72)
        {
            const auto rockDirt = textures_.find("rockDirt");
            if (rockDirt != textures_.end())
            {
                sf::Sprite sprite(rockDirt->second);
                sprite.setScale(tileScale_, tileScale_);
                sprite.setPosition(static_cast<float>(tile.x) * tileSize_, mapTop + static_cast<float>(tile.y) * tileSize_);
                tiles_.push_back(sprite);
            }
        }

        // The corrected map also contains a detached cave ceiling at rows 0-3.
        // Only the lower, ground-connected terrain should create floor colliders.
        if (tile.x >= 0 && tile.x < columns && tile.y >= 8 && isHealTerrainTile(tile.id))
        {
            surfaceRows[static_cast<std::size_t>(tile.x)] =
                std::min(surfaceRows[static_cast<std::size_t>(tile.x)], tile.y);
        }
    }

    if (hasMapAtlas_)
    {
        TerrainCollision::addAtlasSurfaceColliders(
            solidColliders_,
            healRoomTiles,
            mapAtlasTexture_.copyToImage(),
            8,
            32,
            tileScale_,
            mapTop,
            columns,
            roomSize_.y,
            [](const auto& tile) { return tile.y >= 8 && isHealTerrainTile(tile.id); });
    }
    else
    {
        TerrainCollision::addSurfaceColliders(solidColliders_, surfaceRows, mapTop, tileSize_, roomSize_.y);
    }

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

    ground_.setSize({roomSize_.x, roomSize_.y - groundTop_});
    ground_.setPosition(0.f, groundTop_);
    ground_.setFillColor(sf::Color(74, 124, 70));

    sanctuary_ = &addObject<HealingSanctuary>(sf::Vector2f{7.f * tileSize_, mapTop + 12.f * tileSize_});

    addExit(
        RoomType::Boss,
        {160.f, 836.f},
        {roomSize_.x - tileSize_ * 3.f, mapTop + 7.f * tileSize_, tileSize_ * 3.f, tileSize_ * 3.f});
}

void HealRoom::loadTexture(const std::string& id, const std::string& relativePath)
{
    sf::Texture texture;
    if (!texture.loadFromFile(AssetPaths::resolve(relativePath).string()))
    {
        return;
    }

    texture.setSmooth(false);
    textures_[id] = std::move(texture);
}

void HealRoom::addTile(const std::string& textureId, int column, int row)
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

void HealRoom::addAtlasTile(int id, int column, int row, float mapTop)
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

void HealRoom::addSprite(const std::string& textureId, sf::Vector2f bottomCenter, float scale)
{
    const auto found = textures_.find(textureId);
    if (found == textures_.end())
    {
        return;
    }

    sf::Sprite sprite(found->second);
    const sf::Vector2u size = found->second.getSize();
    sprite.setOrigin(static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y));
    sprite.setScale(scale, scale);
    sprite.setPosition(bottomCenter);
    decors_.push_back(sprite);
}
