#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

#include <juce_core/juce_core.h>
#include <string>
#include <sstream>
#include <fstream>

#if (defined __APPLE__) && defined(__aarch64__)
#include <sse2neon.h>
#else
#include <immintrin.h>
#endif

using namespace juce;

std::string
FBFormatDouble(double val, int precision)
{
  std::stringstream ss;
  ss << std::fixed;
  ss.precision(precision);
  ss << val;
  return ss.str();
}

void
FBRestoreDenormal(FBDenormalState state)
{
  _MM_SET_FLUSH_ZERO_MODE(state.first);
  _MM_SET_DENORMALS_ZERO_MODE(state.second);
}

FBDenormalState
FBDisableDenormal()
{
  std::uint32_t ftz = _MM_GET_FLUSH_ZERO_MODE();
  std::uint32_t daz = _MM_GET_DENORMALS_ZERO_MODE();
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  return std::make_pair(ftz, daz);
}

std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopoMeta const& meta)
{
  return FBGetUserDataFolder() / meta.vendor / meta.name / FBPlugFormatToString(meta.format) / meta.id;
}

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

std::vector<std::uint8_t>
FBReadFile(std::filesystem::path const& p)
{ 
  FB_LOG_INFO("Reading file " + p.string());
  auto length = std::filesystem::file_size(p);
  std::vector<std::uint8_t> buffer(length);
  std::ifstream str(p.string(), std::ios_base::binary);
  str.read(reinterpret_cast<char*>(buffer.data()), length);
  str.close();
  FB_LOG_INFO("Read file " + p.string());
  return buffer;
}
