#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

namespace TerrainCollision
{
inline void addLinearSlope(
    std::vector<sf::FloatRect>& colliders,
    float left,
    float width,
    float startTop,
    float endTop,
    float roomHeight)
{
    constexpr float targetStripWidth = 4.f;
    const int strips = std::max(1, static_cast<int>(std::ceil(width / targetStripWidth)));
    const float stripWidth = width / static_cast<float>(strips);

    for (int strip = 0; strip < strips; ++strip)
    {
        const float progress = (static_cast<float>(strip) + 0.5f) / static_cast<float>(strips);
        const float top = startTop + (endTop - startTop) * progress;
        colliders.push_back({
            left + static_cast<float>(strip) * stripWidth,
            top,
            stripWidth,
            roomHeight - top});
    }
}

inline void addGrassSlope(
    std::vector<sf::FloatRect>& colliders,
    float left,
    float width,
    float lowTop,
    float highTop,
    float roomHeight,
    bool risesToRight)
{
    // The 96 px grass slope texture contains flat ground before and after
    // the visible rock incline. These ratios follow its actual silhouette.
    constexpr float sourceWidth = 96.f;
    constexpr float inclineStart = 16.f / sourceWidth;
    constexpr float inclineEnd = 68.f / sourceWidth;
    constexpr float targetStripWidth = 4.f;
    const int strips = std::max(1, static_cast<int>(std::ceil(width / targetStripWidth)));
    const float stripWidth = width / static_cast<float>(strips);

    for (int strip = 0; strip < strips; ++strip)
    {
        float progress = (static_cast<float>(strip) + 0.5f) / static_cast<float>(strips);
        if (!risesToRight)
        {
            progress = 1.f - progress;
        }

        float top = lowTop;
        if (progress >= inclineEnd)
        {
            top = highTop;
        }
        else if (progress > inclineStart)
        {
            const float inclineProgress = (progress - inclineStart) / (inclineEnd - inclineStart);
            top = lowTop + (highTop - lowTop) * inclineProgress;
        }

        colliders.push_back({
            left + static_cast<float>(strip) * stripWidth,
            top,
            stripWidth,
            roomHeight - top});
    }
}

inline int findSolidTop(const sf::Image& image, int left, int top, int width, int height)
{
    constexpr sf::Uint8 alphaThreshold = 32;
    constexpr int requiredVerticalRun = 3;

    for (int y = 0; y <= height - requiredVerticalRun; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            bool solidRun = true;
            for (int run = 0; run < requiredVerticalRun; ++run)
            {
                if (image.getPixel(
                        static_cast<unsigned>(left + x),
                        static_cast<unsigned>(top + y + run)).a <= alphaThreshold)
                {
                    solidRun = false;
                    break;
                }
            }

            if (solidRun)
            {
                return y;
            }
        }
    }

    return -1;
}

template <typename TileRange, typename IsTerrain>
void addAtlasSurfaceColliders(
    std::vector<sf::FloatRect>& colliders,
    const TileRange& tiles,
    const sf::Image& atlas,
    int atlasColumns,
    int sourceTileSize,
    float scale,
    float mapTop,
    int roomColumns,
    float roomHeight,
    IsTerrain isTerrain)
{
    constexpr int sourceStripWidth = 2;
    const int stripsPerTile = sourceTileSize / sourceStripWidth;
    const float stripWidth = static_cast<float>(sourceStripWidth) * scale;
    std::vector<float> surfaceTops(
        static_cast<std::size_t>(roomColumns * stripsPerTile),
        std::numeric_limits<float>::max());

    for (const auto& tile : tiles)
    {
        if (tile.x < 0 || tile.x >= roomColumns || !isTerrain(tile))
        {
            continue;
        }

        const int textureLeft = (tile.id % atlasColumns) * sourceTileSize;
        const int textureTop = (tile.id / atlasColumns) * sourceTileSize;
        for (int sourceX = 0; sourceX < sourceTileSize; sourceX += sourceStripWidth)
        {
            const int solidTop = findSolidTop(
                atlas,
                textureLeft + sourceX,
                textureTop,
                sourceStripWidth,
                sourceTileSize);
            if (solidTop < 0)
            {
                continue;
            }

            const std::size_t stripIndex = static_cast<std::size_t>(
                tile.x * stripsPerTile + sourceX / sourceStripWidth);
            const float worldTop =
                mapTop + static_cast<float>(tile.y * sourceTileSize + solidTop) * scale;
            surfaceTops[stripIndex] = std::min(surfaceTops[stripIndex], worldTop);
        }
    }

    for (std::size_t strip = 0; strip < surfaceTops.size(); ++strip)
    {
        const float top = surfaceTops[strip];
        if (top == std::numeric_limits<float>::max())
        {
            continue;
        }

        colliders.push_back({
            static_cast<float>(strip) * stripWidth,
            top,
            stripWidth,
            roomHeight - top});
    }
}

inline void addTextureSurfaceColliders(
    std::vector<sf::FloatRect>& colliders,
    const sf::Image& image,
    sf::Vector2f topLeft,
    float scale,
    float roomHeight)
{
    constexpr int sourceStripWidth = 2;
    const sf::Vector2u size = image.getSize();
    const float stripWidth = static_cast<float>(sourceStripWidth) * scale;

    for (unsigned sourceX = 0; sourceX < size.x; sourceX += sourceStripWidth)
    {
        const int solidTop = findSolidTop(
            image,
            static_cast<int>(sourceX),
            0,
            std::min(sourceStripWidth, static_cast<int>(size.x - sourceX)),
            static_cast<int>(size.y));
        if (solidTop < 0)
        {
            continue;
        }

        const float top = topLeft.y + static_cast<float>(solidTop) * scale;
        colliders.push_back({
            topLeft.x + static_cast<float>(sourceX) * scale,
            top,
            stripWidth,
            roomHeight - top});
    }
}

template <std::size_t Columns>
void addSurfaceColliders(
    std::vector<sf::FloatRect>& colliders,
    const std::array<int, Columns>& surfaceRows,
    float mapTop,
    float tileSize,
    float roomHeight)
{
    const int noSurface = std::numeric_limits<int>::max();

    for (std::size_t column = 0; column < Columns; ++column)
    {
        const int row = surfaceRows[column];
        if (row == noSurface)
        {
            continue;
        }

        const float top = mapTop + static_cast<float>(row) * tileSize;
        const int nextRow = column + 1 < Columns ? surfaceRows[column + 1] : row;

        if (nextRow != noSurface && std::abs(nextRow - row) == 1)
        {
            addLinearSlope(
                colliders,
                static_cast<float>(column) * tileSize,
                tileSize,
                top,
                mapTop + static_cast<float>(nextRow) * tileSize,
                roomHeight);
            continue;
        }

        colliders.push_back({
            static_cast<float>(column) * tileSize,
            top,
            tileSize,
            roomHeight - top});
    }
}
}
