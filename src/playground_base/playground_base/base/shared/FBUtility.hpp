#pragma once

#include <array>
#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <filesystem>

struct FBStaticTopoMeta;
inline int const FBDefaultDisplayPrecision = 3;

typedef std::pair<std::uint32_t, std::uint32_t>
FBDenormalState;

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

FBDenormalState 
FBDisableDenormal();
void 
FBRestoreDenormal(FBDenormalState state);

std::string
FBFormatDouble(double val, int precision);
std::vector<std::uint8_t>
FBReadFile(std::filesystem::path const& p);

std::filesystem::path
FBGetUserDataFolder();
std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopoMeta const& meta);
