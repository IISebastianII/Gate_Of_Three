#include "ChestRoom.h"

#include "AssetPaths.h"
#include "Chest.h"

#include <algorithm>
#include <utility>

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

    Room::draw(target);
}

sf::Vector2f ChestRoom::getPlayerSpawnFeet() const
{
    return {160.f, groundTop_};
}

void ChestRoom::update(float deltaTime, Player& player)
{
    Room::update(deltaTime, player);

    if (chest_ != nullptr && chest_->isOpen() && !longBlastUnlockGranted_)
    {
        longBlastUnlockGranted_ = true;
        longBlastUnlockRequested_ = true;
    }
}

bool ChestRoom::consumeLongBlastUnlockRequest()
{
    return std::exchange(longBlastUnlockRequested_, false);
}

void ChestRoom::loadTextures()
{
    hasBackground_ = backgroundTexture_.loadFromFile(AssetPaths::resolve("backgrounds/forest_background.png").string());
    backgroundTexture_.setSmooth(false);

    loadTexture("grass", "Tiles/grass/grass.png");
    loadTexture("dirt", "Tiles/grass/dirt_inside_filling.png");
    loadTexture("sign", "decors/sign.png");

    hasTiles_ = textures_.count("grass") > 0 && textures_.count("dirt") > 0;
}

void ChestRoom::buildGeometry()
{
    solidColliders_.clear();
    tiles_.clear();
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

    chest_ = &addObject<Chest>(sf::Vector2f{roomSize_.x * 0.5f, groundTop_});

    RoomExit& battleExit = addExit(
        RoomType::BattleTwo,
        {160.f, groundTop_},
        {roomSize_.x - tileSize_ * 3.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    const auto signTexture = textures_.find("sign");
    if (signTexture != textures_.end())
    {
        battleExit.setTexture(signTexture->second, {roomSize_.x - tileSize_ * 1.5f, groundTop_}, tileScale_);
    }

    longBlastUnlockRequested_ = false;
    longBlastUnlockGranted_ = false;
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
