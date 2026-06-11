#include "BattleRoom.h"

#include "AssetPaths.h"
#include "Enemy.h"

BattleRoom::BattleRoom()
    : Room(RoomType::Battle)
{
    roomSize_ = {2304.f, 900.f};
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
    return {160.f, groundTop_};
}

void BattleRoom::loadTextures()
{
    hasBackground_ = backgroundTexture_.loadFromFile(AssetPaths::resolve("backgrounds/forest_background.png").string());
    backgroundTexture_.setSmooth(false);

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

    hasTiles_ = textures_.count("grass") > 0 && textures_.count("dirt") > 0;
}

void BattleRoom::buildGeometry()
{
    const auto tileLeft = [](int x) {
        return static_cast<float>(x - 1) * tileSize_;
    };

    const auto tileTop = [](int y) {
        return groundTop_ - static_cast<float>(y - 1) * tileSize_;
    };

    const auto tileCenterX = [&](int x) {
        return tileLeft(x) + tileSize_ * 0.5f;
    };

    constexpr int columns = 36;
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

    for (int column = 1; column <= columns; ++column)
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
        for (int column = 1; column <= columns; ++column)
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

    RoomExit& tutorialExit = addExit(
        RoomType::Tutorial,
        {1880.f, 772.f},
        {0.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    const auto signTexture = textures_.find("sign");
    if (signTexture != textures_.end())
    {
        tutorialExit.setTexture(signTexture->second, {tileSize_ * 1.5f, groundTop_}, tileScale_);
    }

    RoomExit& chestExit = addExit(
        RoomType::Chest,
        {160.f, 772.f},
        {roomSize_.x - tileSize_ * 3.f, groundTop_ - tileSize_, tileSize_ * 3.f, tileSize_ * 2.f});
    if (signTexture != textures_.end())
    {
        chestExit.setTexture(signTexture->second, {roomSize_.x - tileSize_ * 1.5f, groundTop_}, tileScale_);
    }

    addObject<Enemy>(sf::Vector2f{tileCenterX(14), tileTop(1)});
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
