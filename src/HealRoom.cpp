#include "HealRoom.h"

#include "AssetPaths.h"

#include <algorithm>
#include <utility>

HealRoom::HealRoom()
    : Room(RoomType::Heal)
{
    roomSize_ = {1280.f, 900.f};
    clearColor_ = sf::Color(190, 220, 210);
    loadTextures();
    buildGeometry();
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
    return {160.f, groundTop_};
}

void HealRoom::loadTextures()
{
    hasBackground_ = backgroundTexture_.loadFromFile(AssetPaths::resolve("backgrounds/forest_background.png").string());
    backgroundTexture_.setSmooth(false);

    loadTexture("grass", "Tiles/grass/grass.png");
    loadTexture("dirt", "Tiles/grass/dirt_inside_filling.png");
    loadTexture("sign", "decors/sign.png");
    loadTexture("sanctuary", "decors/Sanctuary_Healing_Statue.png");
    loadTexture("bush", "decors/bush.png");

    hasTiles_ = textures_.count("grass") > 0 && textures_.count("dirt") > 0;
}

void HealRoom::buildGeometry()
{
    solidColliders_.clear();
    tiles_.clear();
    decors_.clear();
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
    ground_.setFillColor(sf::Color(74, 124, 70));

    if (hasTiles_)
    {
        const int columns = static_cast<int>(roomSize_.x / tileSize_);
        for (int column = 1; column <= columns; ++column)
        {
            addTile("grass", column, 1);
            addTile("dirt", column, 0);
        }
    }

    addSprite("sanctuary", {roomSize_.x * 0.5f, groundTop_}, tileScale_);
    addSprite("bush", {roomSize_.x * 0.35f, groundTop_}, tileScale_);
    addSprite("bush", {roomSize_.x * 0.65f, groundTop_}, tileScale_);

    const auto signTexture = textures_.find("sign");

    RoomExit& battleExit = addExit(
        RoomType::BattleTwo,
        {2144.f, groundTop_},
        {0.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    if (signTexture != textures_.end())
    {
        battleExit.setTexture(signTexture->second, {tileSize_ * 1.5f, groundTop_}, tileScale_);
    }

    RoomExit& bossExit = addExit(
        RoomType::Boss,
        {160.f, 656.f},
        {roomSize_.x - tileSize_ * 3.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    if (signTexture != textures_.end())
    {
        bossExit.setTexture(signTexture->second, {roomSize_.x - tileSize_ * 1.5f, groundTop_}, tileScale_);
    }
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
