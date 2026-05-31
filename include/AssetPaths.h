#pragma once

#include <filesystem>
#include <string>

namespace AssetPaths
{
std::filesystem::path resolve(const std::string& relativePath);
}
