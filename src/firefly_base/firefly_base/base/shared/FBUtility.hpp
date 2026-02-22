#pragma once

#include <juce_core/juce_core.h>

#include <string>
#include <vector>
#include <cassert>
#include <cstdint>
#include <optional>
#include <filesystem>

#if defined(__aarch64__)
#define FB_AARCH64 1
#else
#define FB_AARCH64 0
#endif

struct FBStaticTopoMeta;
inline int const FBDefaultDisplayPrecision = 3;

#ifdef NDEBUG
#define FB_ASSERT(e) ((void)(e))
#else
#define FB_ASSERT(e) assert(e)
#endif

#define FB_STRINGIFY_(x) #x
#define FB_STRINGIFY(x) FB_STRINGIFY_(x)

#define FB_COPY_MOVE_NODEFCTOR(x) \
  x(x&&) = default; \
  x(x const&) = default; \
  x& operator=(x&&) = default; \
  x& operator=(x const&) = default
#define FB_COPY_MOVE_DEFCTOR(x) \
  FB_COPY_MOVE_NODEFCTOR(x); \
  x() = default

#define FB_COPY_NOMOVE_NODEFCTOR(x) \
  x(x&&) = delete; \
  x(x const&) = default; \
  x& operator=(x&&) = delete; \
  x& operator=(x const&) = default
#define FB_COPY_NOMOVE_DEFCTOR(x) \
  FB_COPY_NOMOVE_NODEFCTOR(x); \
  x() = default

#define FB_NOCOPY_MOVE_NODEFCTOR(x) \
  x(x&&) = default; \
  x(x const&) = delete; \
  x& operator=(x&&) = default; \
  x& operator=(x const&) = delete
#define FB_NOCOPY_MOVE_DEFCTOR(x) \
  FB_NOCOPY_MOVE_NODEFCTOR(x); \
  x() = default

#define FB_NOCOPY_NOMOVE_NODEFCTOR(x) \
  x(x&&) = delete; \
  x(x const&) = delete; \
  x& operator=(x&&) = delete; \
  x& operator=(x const&) = delete
#define FB_NOCOPY_NOMOVE_DEFCTOR(x) \
  FB_NOCOPY_NOMOVE_NODEFCTOR(x); \
  x() = default

#define FB_EXPLICIT_COPY_MOVE_NODEFCTOR(x) \
  x(x&&) = default; \
  explicit x(x const&) = default; \
  x& operator=(x&&) = default; \
  x& operator=(x const&) = delete
#define FB_EXPLICIT_COPY_MOVE_DEFCTOR(x) \
  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(x); \
  x() = default

std::string
FBPitchToStringNotes(float pitch);
std::string
FBToStringHz(float val, int precision);
std::string
FBToStringPercent(float val, int precision);
std::string
FBToStringSeconds(float val, int precision);
std::string
FBGainToStringDb(float gain, int precision);
std::string
FBPitchToStringSemis(float coarse, float fine, int precision, bool unit);

// Remove { and } for use in json and urls.
std::string
FBCleanTopoId(std::string const& topoId);

// Oh c'mon cpp, don't make me resort to ICU when i *know* input is in ascii.
std::string
FBAsciiToUpper(std::string const& in);
std::vector<std::string>
FBStringSplit(std::string str, std::string const& delim);

// Localization for std::strtod gave me issues on Linux.
std::string
FBFormatDoubleCLocale(double val);
std::string
FBFormatDoubleCLocale(double val, int precision);
std::optional<double>
FBStringToDoubleOptCLocale(std::string const& text);

std::vector<std::uint8_t>
FBReadFile(std::filesystem::path const& p);
bool
FBParseJson(std::string const& text, juce::var& json);

std::filesystem::path
FBGetUserDataFolder();
std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopoMeta const& meta);
std::filesystem::path
FBGetPluginContentsFolderPath();
std::filesystem::path
FBGetThemesFolderPath();
std::filesystem::path
FBGetPresetsFolderPath();
