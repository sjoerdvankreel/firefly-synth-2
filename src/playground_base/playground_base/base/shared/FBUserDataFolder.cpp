#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/shared/FBUserDataFolder.hpp>

#include <juce_core/juce_core.h>
#include <string>

using namespace juce;

std::filesystem::path
FBGetUserDataFolder()
{
#ifdef __linux__
  char const* home = std::getenv("XDG_CONFIG_HOME");
  if (home == nullptr) 
    home = "~/.config";
  return std::filesystem::path(home);
#else
  auto userData = File::getSpecialLocation(File::userApplicationDataDirectory);
  return std::filesystem::path(userData.getFullPathName().toStdString());
#endif
}

std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopo const* topo)
{
  return FBGetUserDataFolder() / topo->
}