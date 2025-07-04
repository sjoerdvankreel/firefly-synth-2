#pragma once

#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

#include <string>
#include <memory>

#if !defined(FF_PLUG_VERSION_MAJOR) || !defined(FF_PLUG_VERSION_MINOR) || !defined(FF_PLUG_VERSION_PATCH)
#error
#endif

#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)

struct FBStaticTopo;
struct FBStaticTopoMeta;

FBStaticTopoMeta FFPlugMeta(FBPlugFormat format);
std::string FFFormatBlockSlot(FBStaticTopo const&, int slot);
std::unique_ptr<FBStaticTopo> FFMakeTopo(FBPlugFormat format);

// Be VERY AFRAID of reordering this.
// Even REAPER the holy grail of plugin compatibility has problems reconnecting automation lanes if this changes.
// And yes, i checked. Plugin param indexes were unstable, but param ids were stable. Still broke.
enum class FFModuleType { Output, GUISettings, Master, GMix, Osci, OsciMod, VEffect, GEffect, VMix, Env, VLFO, GLFO, Count };

inline int constexpr FFEnvCount = 8;
inline int constexpr FFLFOCount = 8;
inline int constexpr FFOsciCount = 4;
inline int constexpr FFEffectCount = 4;
inline constexpr char FFVendorName[] = "Sjoerd van Kreel";
inline constexpr char FFVendorMail[] = "sjoerdvankreel@gmail.com";
inline constexpr char FFVendorURL[] = "https://github.com/sjoerdvankreel";
inline constexpr char FFPlugName[] = "Firefly Synth 2";
inline constexpr char FFPlugUniqueId[] = "754068B351A04DB4813B58D562BDFC1F";
inline constexpr char FFPlugControllerId[] = "959E6302402B461A8C9AA5A6737BCAAD";