#pragma once

#include <filesystem>

struct FBStaticTopoMeta;

std::filesystem::path
FBGetUserDataFolder();
std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopoMeta const& meta);