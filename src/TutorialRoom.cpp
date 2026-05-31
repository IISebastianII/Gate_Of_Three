#include "TutorialRoom.h"

#include "AssetPaths.h"
#include "Npc.h"

#include <cmath>
#include <initializer_list>

TutorialRoom::TutorialRoom()
    : Room(RoomType::Tutorial)
{
    roomSize_ = {2048.f, 900.f};
    clearColor_ = sf::Color(238, 241, 242);
    loadTextures();
    buildGeometry();
}

void TutorialRoom::update(float deltaTime)
{
    Room::update(deltaTime);
}

void TutorialRoom::draw(sf::RenderTarget& target) const
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

sf::Vector2f TutorialRoom::getPlayerSpawnFeet() const
{
    return {180.f, baseGroundTop_};
}

void TutorialRoom::loadTextures()
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

void TutorialRoom::buildGeometry()
{
    const auto tileLeft = [](int x) {
        return static_cast<float>(x - 1) * tileSize_;
    };

    const auto tileTop = [](int y) {
        return baseGroundTop_ - static_cast<float>(y - 1) * tileSize_;
    };

    const auto tileBottom = [&](int y) {
        return tileTop(y) + tileSize_;
    };

    const auto addSolidTile = [&](int x, int y) {
        solidColliders_.push_back({tileLeft(x), tileTop(y), tileSize_, roomSize_.y - tileTop(y)});
    };

    solidColliders_.clear();

    for (int x = 1; x <= 9; ++x)
    {
        addSolidTile(x, 1);
    }

    const float slopeLeft = tileLeft(10);
    const float slopeWidth = tileSize_ * 3.f;
    const float slopeHeight = baseGroundTop_ - plateauTop_;
    const float halfTile = tileSize_ * 0.5f;
    for (int step = 0; step < 6; ++step)
    {
        const float progress = static_cast<float>(step) / 5.f;
        const float top = baseGroundTop_ - slopeHeight * progress;
        solidColliders_.push_back({slopeLeft + halfTile * static_cast<float>(step), top, halfTile, roomSize_.y - top});
    }

    for (int x = 13; x <= 16; ++x)
    {
        for (int y = 1; y <= 3; ++y)
        {
            addSolidTile(x, y);
        }
    }

    addSolidTile(17, 1);
    addSolidTile(17, 2);
    addSolidTile(17, 3);
    addSolidTile(18, 1);
    addSolidTile(18, 2);

    for (int x = 19; x <= 32; ++x)
    {
        addSolidTile(x, 1);
    }

    Npc& catNpc = addObject<Npc>();
    catNpc.setFeetPosition({tileLeft(6) + tileSize_ * 0.5f, tileTop(1)});

    if (hasBackground_)
    {
        const auto textureSize = backgroundTexture_.getSize();
        const float scale = roomSize_.x / static_cast<float>(textureSize.x);
        background_.setTexture(backgroundTexture_);
        background_.setScale(scale, scale);
        background_.setColor(sf::Color::White);
        background_.setPosition(0.f, baseGroundTop_ - static_cast<float>(textureSize.y) * scale + 4.f);
    }

    if (hasTiles_)
    {
        for (int x = 1; x <= 9; ++x)
        {
            addTile("grass", x, 1);
            addTile("dirt", x, 0);
        }

        addSprite("slopeLeft", {tileLeft(10) + slopeWidth * 0.5f, tileBottom(1)}, tileScale_);

        for (int x = 10; x <= 18; ++x)
        {
            addTile("dirt", x, 0);
        }

        for (int x = 13; x <= 16; ++x)
        {
            addTile("dirt", x, 1);
            addTile("dirt", x, 2);
            addTile("grass", x, 3);
        }

        addTile("dirt", 17, 1);
        addTile("grassRightOverhang", 17, 2);
        addTile("grassRightCorner", 17, 3);
        addTile("grassRightOverhang", 18, 1);
        addTile("grassRightCorner", 18, 2);

        for (int x = 19; x <= 32; ++x)
        {
            addTile("grass", x, 1);
            addTile("dirt", x, 0);
        }

        addSprite("bush", {tileLeft(2) + tileSize_ * 0.5f, tileTop(1)}, tileScale_);
        addSprite("pumpkin", {tileLeft(9) + tileSize_ * 0.5f, tileTop(1)}, tileScale_);
        addSprite("cookingPot", {tileLeft(12) + tileSize_ * 0.5f, tileTop(3)}, tileScale_);
        addSprite("logAndAxe", {tileLeft(13) + tileSize_ * 0.5f, tileTop(3)}, tileScale_);
        addSprite("tent", {tileLeft(14) + tileSize_ * 1.5f, tileTop(3)}, tileScale_);
        addSprite("largeRocks", {tileLeft(20) + tileSize_ * 1.5f, tileTop(1)}, tileScale_);
        addSprite("bush", {tileLeft(25) + tileSize_ * 0.5f, tileTop(1)}, tileScale_);
        addSprite("sign", {tileLeft(31) + tileSize_ * 0.5f, tileTop(1)}, tileScale_);
        return;
    }

    for (const auto& platform : solidColliders_)
    {
        sf::RectangleShape fallback({platform.width, platform.height});
        fallback.setPosition(platform.left, platform.top);
        fallback.setFillColor(sf::Color(74, 124, 70));
        fallback.setOutlineColor(sf::Color(35, 76, 48));
        fallback.setOutlineThickness(2.f);
        fallbackPlatforms_.push_back(fallback);
    }
}

void TutorialRoom::loadTexture(const std::string& id, const std::string& relativePath)
{
    sf::Texture texture;
    if (!texture.loadFromFile(AssetPaths::resolve(relativePath).string()))
    {
        return;
    }

    texture.setSmooth(false);
    textures_[id] = std::move(texture);
}

void TutorialRoom::addTile(const std::string& textureId, int column, int visualRow)
{
    const auto found = textures_.find(textureId);
    if (found == textures_.end())
    {
        return;
    }

    sf::Sprite tile(found->second);
    tile.setScale(tileScale_, tileScale_);
    tile.setPosition(static_cast<float>(column - 1) * tileSize_, baseGroundTop_ - static_cast<float>(visualRow - 1) * tileSize_);
    tiles_.push_back(tile);
}

void TutorialRoom::addSprite(const std::string& textureId, sf::Vector2f bottomCenter, float scale)
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

void TutorialRoom::addTiledPlatform(const sf::FloatRect& bounds)
{
    const int columns = static_cast<int>(std::ceil(bounds.width / tileSize_));
    const int rows = static_cast<int>(std::ceil(bounds.height / tileSize_));

    for (int column = 0; column < columns; ++column)
    {
        for (int row = 0; row < rows; ++row)
        {
            const std::string textureId = row == 0 ? "grass" : "dirt";
            addTile(textureId, static_cast<int>((bounds.left + column * tileSize_) / tileSize_) + 1, static_cast<int>((baseGroundTop_ - bounds.top - row * tileSize_) / tileSize_) + 1);
        }
    }
}
