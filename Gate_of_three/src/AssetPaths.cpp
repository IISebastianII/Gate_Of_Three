#include "AssetPaths.h"

#include <array>

namespace
{
std::filesystem::path configuredRoot()
{
#ifdef ASSET_ROOT
    return std::filesystem::path(ASSET_ROOT);
#else
    return std::filesystem::current_path() / "Assets";
#endif
}
}

namespace AssetPaths
{
std::filesystem::path resolve(const std::string& relativePath)
{
    const std::filesystem::path relative(relativePath);
    std::filesystem::path current = std::filesystem::current_path();

    for (int depth = 0; depth < 5; ++depth)
    {
        const std::array<std::filesystem::path, 2> localCandidates = {
            current / "Assets" / relative,
            current / "assets" / relative
        };

        for (const auto& candidate : localCandidates)
        {
            if (std::filesystem::exists(candidate))
            {
                return candidate;
            }
        }

        if (!current.has_parent_path() || current == current.parent_path())
        {
            break;
        }
        current = current.parent_path();
    }

    return configuredRoot() / relative;
}
}
