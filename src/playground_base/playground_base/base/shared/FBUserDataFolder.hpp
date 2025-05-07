#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>

struct FBStaticTopoMeta;

// todo not belong here
std::vector<std::uint8_t>
FBReadFile(std::filesystem::path const& p);

std::filesystem::path
FBGetUserDataFolder();
std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopoMeta const& meta);