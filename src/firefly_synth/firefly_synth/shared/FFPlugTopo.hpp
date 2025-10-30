#pragma once

#include <firefly_synth/modules/mod_matrix/FFModMatrixSource.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>

#include <string>
#include <memory>

#if !defined(FF_PLUG_VERSION_MAJOR) || !defined(FF_PLUG_VERSION_MINOR) || !defined(FF_PLUG_VERSION_PATCH)
#error
#endif

#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)

inline int constexpr FFLFOCount = 6;
inline int constexpr FFOsciCount = 4; // easy access mod needs env/lfo >= this
inline int constexpr FFEffectCount = FFOsciCount; // easy access mod needs env/lfo >= this, also GUI needs these to be equal
inline int constexpr FFEnvCount = FFLFOCount + 1; // easy access mod, interleaving & amp env
inline int constexpr FFAmpEnvSlot = 0;
inline int constexpr FFEnvSlotOffset = 1;

inline constexpr char FFVendorName[] = "Sjoerd van Kreel";
inline constexpr char FFVendorMail[] = "sjoerdvankreel@gmail.com";
inline constexpr char FFVendorURL[] = "https://github.com/sjoerdvankreel";
inline constexpr char FFPlugShortNameFX[] = "FF2 FX";
inline constexpr char FFPlugShortNameInst[] = "FF2";
inline constexpr char FFPlugNameFX[] = "Firefly Synth 2 FX";
inline constexpr char FFPlugNameInst[] = "Firefly Synth 2";
inline constexpr char FFPlugUniqueIdFX[] = "E761431919AC4A2087B3CF8EFCD6396F";
inline constexpr char FFPlugUniqueIdInst[] = "754068B351A04DB4813B58D562BDFC1F";
inline constexpr char FFPlugControllerIdFX[] = "51E397E58B694A129208424AEF1C7A84";
inline constexpr char FFPlugControllerIdInst[] = "959E6302402B461A8C9AA5A6737BCAAD"; 

enum class FFModuleType {
  GUISettings, Settings, MIDI, GNote, Master, GlobalUni, VoiceModule, Osci, OsciMod, VNote, Env, 
  VEffect, GEffect, VLFO, GLFO, VMix, GMix, VEcho, GEcho, VMatrix, GMatrix, Output, Count };

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

FBStaticTopoMeta FFPlugMeta(FBPlugFormat format, bool isFx);
std::unique_ptr<FFStaticTopo> FFMakeTopo(FBPlugFormat format, bool isFx);
std::string FFFormatBlockSlot(FBStaticTopo const&, int moduleSlot, int itemSlot);
