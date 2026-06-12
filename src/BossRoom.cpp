#include "BossRoom.h"

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
    solidColliders_.clear();
    tiles_.clear();
    solidColliders_.push_back({0.f, groundTop_, roomSize_.x, roomSize_.y - groundTop_});

    fallbackGround_.setSize({roomSize_.x, roomSize_.y - groundTop_});
    fallbackGround_.setPosition(0.f, groundTop_);
    fallbackGround_.setFillColor(sf::Color(48, 52, 62));

    constexpr int columns = 20;
    if (hasFloorTiles_)
    {
        for (int column = 0; column < columns; ++column)
        {
            const float x = static_cast<float>(column) * tileSize_;
            addTile("rockDirt", {x, groundTop_});
            addTile("rockDirt", {x, groundTop_ + tileSize_});
            addTile("ceiling", {x, 0.f});
        }

        for (float y = tileSize_; y < groundTop_; y += tileSize_)
        {
            addTile("wall1", {0.f, y});
            addTile("wall2", {roomSize_.x - tileSize_, y});
        }

        addTile("leftCorner", {0.f, 0.f});
        addTile("rightCorner", {roomSize_.x - tileSize_, 0.f});
        addTile("ceilingSpikes", {tileSize_ * 5.f, tileSize_});
        addTile("ceilingSpikes", {tileSize_ * 14.f, tileSize_});
    }

    RoomExit& healExit = addExit(
        RoomType::Heal,
        {1120.f, groundTop_},
        {0.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    const auto signTexture = textures_.find("sign");
    if (signTexture != textures_.end())
    {
        healExit.setTexture(signTexture->second, {tileSize_ * 1.5f, groundTop_}, tileScale_);
    }

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
