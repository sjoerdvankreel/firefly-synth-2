#pragma once

#include <filesystem>

struct FBStaticTopo;

std::filesystem::path
FBGetUserDataFolder();
std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopo const* topo);