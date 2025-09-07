#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

#include <juce_core/juce_core.h>
#include <string>
#include <sstream>
#include <fstream>

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

std::optional<double>
FBStringToDoubleOptCLocale(std::string const& text)
{
  double result = 0.0;
  std::istringstream str(text);
  str.imbue(std::locale("C"));
  str >> result;
  if (str.fail() || str.bad() || !str.eof())
  {
    FB_LOG_WARN("Failed to parse '" + text + "' to double, returning 0.");
    return std::nullopt;
  }
  return result;
}

void
FBRestoreDenormal(FBDenormalState state)
{
#if (defined __APPLE__) && defined(__aarch64__)
  if (state.wasApplied)
    fesetenv(&state.env);
#else
  _MM_SET_FLUSH_ZERO_MODE(state.ftz);
  _MM_SET_DENORMALS_ZERO_MODE(state.daz);
#endif
}

FBDenormalState
FBDisableDenormal()
{
#if (defined __APPLE__) && defined(__aarch64__)
  FBDenormalState result = {};
  result.wasApplied = fegetenv(&result.env) == 0;
  if (result.wasApplied)
    result.wasApplied = fesetenv(FE_DFL_DISABLE_DENORMS_ENV) == 0;
  return result;
#else
  FBDenormalState result;
  result.ftz = _MM_GET_FLUSH_ZERO_MODE();
  result.daz = _MM_GET_DENORMALS_ZERO_MODE();
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  return result;
#endif
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
