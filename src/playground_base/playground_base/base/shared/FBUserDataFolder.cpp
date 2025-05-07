#include <playground_base/base/shared/FBUserDataFolder.hpp>
#include <playground_base/base/topo/static/FBStaticTopoMeta.hpp>

#include <juce_core/juce_core.h>

#include <string>
#include <fstream>

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
FBGetUserPluginDataFolder(FBStaticTopoMeta const& meta)
{
  return FBGetUserDataFolder() / meta.vendor / meta.name / meta.id;
}

std::vector<std::uint8_t>
FBReadFile(std::filesystem::path const& p)
{
  auto length = std::filesystem::file_size(p);
  std::vector<std::uint8_t> buffer(length);
  std::ifstream str(p.string(), std::ios_base::binary);
  str.read(reinterpret_cast<char*>(buffer.data()), length);
  str.close();
  return buffer;
}
