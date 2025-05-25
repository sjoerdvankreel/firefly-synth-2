#pragma once

#include <memory>

#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)

struct FBStaticTopo;
struct FBStaticTopoMeta;

FBStaticTopoMeta FFPlugMeta();
std::unique_ptr<FBStaticTopo> FFMakeTopo();

enum class FFModuleType { GLFO, GFilter, Phys, Osci, OsciMod, VEffect, Env, Master, Output, GUISettings, Count };

inline int constexpr FFEnvCount = 8;
inline int constexpr FFOsciCount = 4;
inline int constexpr FFPhysCount = 2;
inline int constexpr FFGLFOCount = 1;
inline int constexpr FFGFilterCount = 2;
inline int constexpr FFVEffectCount = 4;
inline constexpr char FFVendorName[] = "Sjoerd van Kreel";
inline constexpr char FFVendorMail[] = "sjoerdvankreel@gmail.com";
inline constexpr char FFVendorURL[] = "https://github.com/sjoerdvankreel";
inline constexpr char FFPlugName[] = "Playground Plug";
inline constexpr char FFPlugUniqueId[] = "754068B351A04DB4813B58D562BDFC1F";
inline constexpr char FFPlugControllerId[] = "959E6302402B461A8C9AA5A6737BCAAD";