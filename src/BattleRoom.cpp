#include "BattleRoom.h"

#include "AdditionalRoomMapData.h"
#include "AssetPaths.h"
#include "BattleRoomOneMapData.h"
#include "Enemy.h"
#include "Mushroom.h"
#include "TerrainCollision.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace
{
constexpr int battleOneColumns = 32;
constexpr int battleOneBottomRow = 6;

bool isTerrainTile(int id)
{
    constexpr std::array<int, 21> terrainIds = {
        0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 27,
        28, 29, 33, 34, 35, 36, 37, 42, 43, 55};
    return std::find(terrainIds.begin(), terrainIds.end(), id) != terrainIds.end();
}
}

BattleRoom::BattleRoom(RoomType type)
    : Room(type)
{
    roomSize_ = type == RoomType::Battle
        ? sf::Vector2f{2048.f, 900.f}
        : sf::Vector2f{2048.f, 900.f};
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

    for (const auto& decor : decors_)
    {
        target.draw(decor);
    }

    Room::draw(target);
}

sf::Vector2f BattleRoom::getPlayerSpawnFeet() const
{
    return type_ == RoomType::Battle
        ? sf::Vector2f{96.f, groundTop_}
        : sf::Vector2f{96.f, groundTop_};
}

void BattleRoom::loadTextures()
{
    hasBackground_ = backgroundTexture_.loadFromFile(AssetPaths::resolve("backgrounds/forest_background.png").string());
    backgroundTexture_.setSmooth(false);

    hasBattleOneAtlas_ = battleOneAtlasTexture_.loadFromFile(
        AssetPaths::resolve("Maps/battle_room_1_spritesheet.png").string());
    battleOneAtlasTexture_.setSmooth(false);

    loadTexture("grass", "Tiles/grass/grass.png");
    loadTexture("dirt", "Tiles/grass/dirt_inside_filling.png");
    loadTexture("grassLeftCorner", "Tiles/grass/grass_left_uppercoorner.png");
    loadTexture("grassRightCorner", "Tiles/grass/grass_right_uppercoorner.png");
    loadTexture("grassLeftOverhang", "Tiles/grass/grass_left_overhang.png");
    loadTexture("grassRightOverhang", "Tiles/grass/grass_right_overhang.png");
    loadTexture("slopeLeft", "Tiles/grass/rocks_slope_left.png");
    loadTexture("slopeRight", "Tiles/grass/rocks_slope_right.png");

    loadTexture("bush", "decors/bush.png");
    loadTexture("pumpkin", "decors/pumpkin.png");
    loadTexture("tent", "decors/tent.png");
    loadTexture("largeRocks", "decors/large_rocks.png");
    loadTexture("smallRocks1", "decors/small_rocks_1.png");
    loadTexture("smallRocks2", "decors/small_rocks_2.png");
    loadTexture("smallRock", "decors/small_rock.png");
    loadTexture("logAndAxe", "decors/log_and_axe.png");
    loadTexture("cookingPot", "decors/cooking_pot.png");
    loadTexture("sign", "decors/sign.png");

    hasTiles_ = hasBattleOneAtlas_ || (textures_.count("grass") > 0 && textures_.count("dirt") > 0);
}

void BattleRoom::buildGeometry()
{
    if (type_ == RoomType::Battle)
    {
        buildBattleOneGeometry();
        return;
    }

    buildBattleTwoGeometry();
}

void BattleRoom::buildBattleOneGeometry()
{
    constexpr float mapTop = groundTop_ - static_cast<float>(battleOneBottomRow) * tileSize_;
    std::array<int, battleOneColumns> surfaceRows;
    surfaceRows.fill(std::numeric_limits<int>::max());

    solidColliders_.clear();
    tiles_.clear();
    decors_.clear();
    fallbackPlatforms_.clear();

    for (const auto& tile : battleRoomOneTiles)
    {
        if (hasBattleOneAtlas_)
        {
            addAtlasTile(tile.id, tile.x, tile.y, mapTop);
        }

        if (tile.x >= 0 && tile.x < battleOneColumns && isTerrainTile(tile.id))
        {
            surfaceRows[static_cast<std::size_t>(tile.x)] =
                std::min(surfaceRows[static_cast<std::size_t>(tile.x)], tile.y);
        }
    }

    if (hasBattleOneAtlas_)
    {
        TerrainCollision::addAtlasSurfaceColliders(
            solidColliders_,
            battleRoomOneTiles,
            battleOneAtlasTexture_.copyToImage(),
            8,
            32,
            tileScale_,
            mapTop,
            battleOneColumns,
            roomSize_.y,
            [](const auto& tile) { return isTerrainTile(tile.id); });
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

    if (!hasBattleOneAtlas_)
    {
        for (const auto& collider : solidColliders_)
        {
            sf::RectangleShape platform({collider.width, collider.height});
            platform.setPosition(collider.left, collider.top);
            platform.setFillColor(sf::Color(74, 124, 70));
            fallbackPlatforms_.push_back(platform);
        }
    }

    addExit(
        RoomType::Chest,
        {32.f, groundTop_},
        {roomSize_.x - tileSize_ * 3.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});

    const auto surfaceFeet = [&](int column) {
        return mapTop + static_cast<float>(surfaceRows[static_cast<std::size_t>(column)]) * tileSize_;
    };

    addObject<Enemy>(sf::Vector2f{9.5f * tileSize_, surfaceFeet(9)});
    addObject<Mushroom>(sf::Vector2f{15.5f * tileSize_, surfaceFeet(15)});
    addObject<Enemy>(sf::Vector2f{22.5f * tileSize_, surfaceFeet(22)});
    addObject<Enemy>(sf::Vector2f{27.5f * tileSize_, surfaceFeet(27)});
}

void BattleRoom::buildBattleTwoGeometry()
{
    constexpr int columns = 32;
    constexpr int bottomRow = 5;
    constexpr float mapTop = groundTop_ - static_cast<float>(bottomRow) * tileSize_;
    std::array<int, columns> surfaceRows;
    surfaceRows.fill(std::numeric_limits<int>::max());

    solidColliders_.clear();
    tiles_.clear();
    decors_.clear();
    fallbackPlatforms_.clear();

    for (const auto& tile : battleRoomTwoTiles)
    {
        if (hasBattleOneAtlas_ && tile.id < 72)
        {
            addAtlasTile(tile.id, tile.x, tile.y, mapTop);
        }

        if (tile.x >= 0 && tile.x < columns && isTerrainTile(tile.id))
        {
            surfaceRows[static_cast<std::size_t>(tile.x)] =
                std::min(surfaceRows[static_cast<std::size_t>(tile.x)], tile.y);
        }
    }

    if (hasBattleOneAtlas_)
    {
        TerrainCollision::addAtlasSurfaceColliders(
            solidColliders_,
            battleRoomTwoTiles,
            battleOneAtlasTexture_.copyToImage(),
            8,
            32,
            tileScale_,
            mapTop,
            columns,
            roomSize_.y,
            [](const auto& tile) { return isTerrainTile(tile.id); });
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

    addExit(
        RoomType::Heal,
        {32.f, groundTop_},
        {roomSize_.x - tileSize_ * 3.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});

    const auto surfaceFeet = [&](int column) {
        return mapTop + static_cast<float>(surfaceRows[static_cast<std::size_t>(column)]) * tileSize_;
    };

    addObject<Enemy>(sf::Vector2f{8.5f * tileSize_, surfaceFeet(8)});
    addObject<Mushroom>(sf::Vector2f{15.5f * tileSize_, surfaceFeet(15)});
    addObject<Enemy>(sf::Vector2f{22.5f * tileSize_, surfaceFeet(22)});
    addObject<Enemy>(sf::Vector2f{28.5f * tileSize_, surfaceFeet(28)});
    return;

    const auto tileLeft = [](int x) {
        return static_cast<float>(x - 1) * tileSize_;
    };

    const auto tileTop = [](int y) {
        return groundTop_ - static_cast<float>(y - 1) * tileSize_;
    };

    const auto tileCenterX = [&](int x) {
        return tileLeft(x) + tileSize_ * 0.5f;
    };

    constexpr int oldColumns = 36;
    const std::vector<int> terrainHeight = {
        1, 1, 1, 1, 1,
        2, 2,
        3, 3,
        2, 2,
        1, 1, 1, 1, 1, 1, 1,
        2, 2, 2,
        1, 1, 1, 1, 1, 1, 1,
        2, 2, 2,
        1, 1, 1, 1, 1
    };

    solidColliders_.clear();
    tiles_.clear();
    decors_.clear();
    fallbackPlatforms_.clear();

    for (int column = 1; column <= oldColumns; ++column)
    {
        const int height = terrainHeight[static_cast<std::size_t>(column - 1)];
        const float top = tileTop(height);
        solidColliders_.push_back({tileLeft(column), top, tileSize_, roomSize_.y - top});
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

    if (hasTiles_)
    {
        for (int column = 1; column <= oldColumns; ++column)
        {
            const int height = terrainHeight[static_cast<std::size_t>(column - 1)];

            for (int row = 0; row < height; ++row)
            {
                addTile("dirt", column, row);
            }

            std::string topTexture = "grass";

            if ((column == 6 && height == 3) || (column == 7 && height == 4))
            {
                topTexture = "grassLeftOverhang";
            }
            else if ((column == 9 && height == 4) || (column == 11 && height == 3))
            {
                topTexture = "grassRightOverhang";
            }

            addTile(topTexture, column, height);
        }

        addSprite("grave1", {tileCenterX(4), tileTop(1)}, tileScale_);
        addSprite("bush", {tileCenterX(6), tileTop(2)}, tileScale_);
        addSprite("smallRocks1", {tileCenterX(11), tileTop(2)}, tileScale_);
        addSprite("largeRocks", {tileCenterX(17), tileTop(1)}, tileScale_);
        addSprite("pumpkin", {tileCenterX(22), tileTop(1)}, tileScale_);
        addSprite("grave2", {tileCenterX(28), tileTop(1)}, tileScale_);
        addSprite("smallRocks2", {tileCenterX(31), tileTop(2)}, tileScale_);
        addSprite("bush", {tileCenterX(35), tileTop(1)}, tileScale_);
    }
    else
    {
        for (const auto& collider : solidColliders_)
        {
            sf::RectangleShape ground({collider.width, collider.height});
            ground.setPosition(collider.left, collider.top);
            ground.setFillColor(sf::Color(74, 124, 70));
            fallbackPlatforms_.push_back(ground);
        }
    }

    const RoomType previousRoom = type_ == RoomType::BattleTwo
        ? RoomType::Chest
        : RoomType::Tutorial;
    const sf::Vector2f previousRoomSpawn = type_ == RoomType::BattleTwo
        ? sf::Vector2f{1120.f, 772.f}
        : sf::Vector2f{1880.f, 772.f};

    RoomExit& previousExit = addExit(
        previousRoom,
        previousRoomSpawn,
        {0.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    const auto signTexture = textures_.find("sign");
    if (signTexture != textures_.end())
    {
        previousExit.setTexture(signTexture->second, {tileSize_ * 1.5f, groundTop_}, tileScale_);
    }

    if (type_ == RoomType::Battle)
    {
        RoomExit& chestExit = addExit(
            RoomType::Chest,
            {160.f, 772.f},
            {roomSize_.x - tileSize_ * 3.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
        if (signTexture != textures_.end())
        {
            chestExit.setTexture(signTexture->second, {roomSize_.x - tileSize_ * 1.5f, groundTop_}, tileScale_);
        }
    }
    else if (type_ == RoomType::BattleTwo)
    {
        RoomExit& healExit = addExit(
            RoomType::Heal,
            {160.f, 772.f},
            {roomSize_.x - tileSize_ * 3.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
        if (signTexture != textures_.end())
        {
            healExit.setTexture(signTexture->second, {roomSize_.x - tileSize_ * 1.5f, groundTop_}, tileScale_);
        }
    }

    addObject<Enemy>(sf::Vector2f{tileCenterX(14), tileTop(1)});
    if (type_ == RoomType::Battle)
    {
        addObject<Mushroom>(sf::Vector2f{tileCenterX(19), tileTop(2)});
    }
    addObject<Enemy>(sf::Vector2f{tileCenterX(24), tileTop(1)});
    addObject<Enemy>(sf::Vector2f{tileCenterX(33), tileTop(1)});
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

void BattleRoom::addAtlasTile(int id, int column, int row, float mapTop)
{
    constexpr int atlasColumns = 8;
    constexpr int sourceTileSize = 32;

    sf::Sprite tile(battleOneAtlasTexture_);
    tile.setTextureRect({
        (id % atlasColumns) * sourceTileSize,
        (id / atlasColumns) * sourceTileSize,
        sourceTileSize,
        sourceTileSize});
    tile.setScale(tileScale_, tileScale_);
    tile.setPosition(static_cast<float>(column) * tileSize_, mapTop + static_cast<float>(row) * tileSize_);
    tiles_.push_back(tile);
}

void BattleRoom::addSprite(const std::string& textureId, sf::Vector2f bottomCenter, float scale)
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
