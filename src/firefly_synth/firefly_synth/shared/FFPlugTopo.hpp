#pragma once

#include <firefly_synth/modules/mod_matrix/FFModMatrixSource.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

#include <string>
#include <memory>

#if !defined(FF_PLUG_VERSION_MAJOR) || !defined(FF_PLUG_VERSION_MINOR) || !defined(FF_PLUG_VERSION_PATCH)
#error
#endif

#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)

struct FBStaticTopoMeta;

inline int constexpr FFLFOCount = 4;
inline int constexpr FFOsciCount = FFLFOCount; // easy access mod
inline int constexpr FFEffectCount = FFLFOCount; // easy access mod
inline int constexpr FFEnvCount = FFLFOCount + 1; // easy access mod, interleaving & amp env
inline int constexpr FFAmpEnvSlot = FFLFOCount;

inline constexpr char FFVendorName[] = "Sjoerd van Kreel";
inline constexpr char FFVendorMail[] = "sjoerdvankreel@gmail.com";
inline constexpr char FFVendorURL[] = "https://github.com/sjoerdvankreel";
inline constexpr char FFPlugName[] = "Firefly Synth 2";
inline constexpr char FFPlugUniqueId[] = "754068B351A04DB4813B58D562BDFC1F";
inline constexpr char FFPlugControllerId[] = "959E6302402B461A8C9AA5A6737BCAAD"; 

// TODO rearrange
enum class FFModuleType {
  Output, GUISettings, Master, GMix, Osci, OsciMod, VEffect, 
  GEffect, VMix, Env, VLFO, GLFO, GMatrix, VMatrix, MIDI, External, Count }; 

struct FFStaticTopo:
public FBStaticTopo
{
  ~FFStaticTopo() override {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FFStaticTopo);

  // These are used for the mod matrix so make sure it lines up with what the audio engine is doing.
  std::vector<FFModMatrixSource> gMatrixSources = {};
  std::vector<FFModMatrixSource> vMatrixSources = {};
  std::vector<FBParamTopoIndices> gMatrixTargets = {};
  std::vector<FBParamTopoIndices> vMatrixTargets = {};
};

FBStaticTopoMeta FFPlugMeta(FBPlugFormat format);
std::unique_ptr<FFStaticTopo> FFMakeTopo(FBPlugFormat format);
std::string FFFormatBlockSlot(FBStaticTopo const&, int moduleSlot, int itemSlot);
