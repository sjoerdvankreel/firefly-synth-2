#pragma once

#include <firefly_synth/dsp/shared/FFTrigFunctions.hpp>
#include <string>
#include <memory>

struct FBStaticModule;
inline int constexpr FFEffectBlockCount = 4; // must be <= lfo count and env count minus amp env for easy access mod
std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool global);

enum FFEffectFoldMode { FFEffectFoldModeFold = FFTrigCount };
enum class FFEffectCompMode { Peak, RMS };
enum class FFEffectSkewMode { Uni, Bi };
enum class FFEffectClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp  };
enum class FFEffectFilterMode { Freq, Pitch, Track };

std::string FFEffectCompModeToString(FFEffectCompMode mode);
std::string FFEffectFoldModeToString(FFEffectFoldMode mode);
std::string FFEffectSkewModeToString(FFEffectSkewMode mode);
std::string FFEffectClipModeToString(FFEffectClipMode mode);
std::string FFEffectFilterModeToString(FFEffectFilterMode mode);

enum class FFEffectKind { 
  Off, 
  LPF, BPF, HPF, BSF, APF, PEQ, BLL, LSH, HSH,
  Comb, CombPlus, CombMin,
  Clip, Fold, Skew,
  Compressor };
std::string FFEffectKindToString(FFEffectKind kind);

enum class FFEffectParam { On, Oversample, TrackingKey, LastKeySmoothTime, 
  Kind, EnvAmt, LFOAmt, FilterMode,
  CombKeyTrk, CombFreqFreqPlus, CombPitchCoarsePlus, CombResPlus, CombFreqFreqMin, CombPitchCoarseMin, CombResMin, 
  StVarKeyTrak, StVarFreqFreq, StVarPitchCoarse, StVarRes, StVarGain,  
  ClipMode, FoldMode, SkewMode, DistDrive, DistMix, DistBias, DistAmt, 
  CompMode, CompThreshold, CompRatio, //CompRMSSize, CompSide, CompAttack, CompRelease, CompLookahead, CompKnee,
  Count };

inline bool
FFEffectKindIsShaper(FFEffectKind kind)
{
  switch (kind)
  {
  case FFEffectKind::Clip:
  case FFEffectKind::Skew:
  case FFEffectKind::Fold:
    return true;
  default:
    return false;
  }
}

inline bool 
FFEffectKindIsComb(FFEffectKind kind)
{
  switch (kind)
  {
  case FFEffectKind::Comb:
  case FFEffectKind::CombPlus:
  case FFEffectKind::CombMin:
    return true;
  default:
    return false;
  }
}

inline bool
FFEffectKindIsSVF(FFEffectKind kind)
{
  switch (kind)
  {
  case FFEffectKind::LPF:
  case FFEffectKind::BPF:
  case FFEffectKind::HPF:
  case FFEffectKind::BSF:
  case FFEffectKind::APF:
  case FFEffectKind::PEQ:
  case FFEffectKind::BLL:
  case FFEffectKind::LSH:
  case FFEffectKind::HSH:
    return true;
  default:
    return false;
  }
}

inline bool
FFEffectKindIsSVFWithGain(FFEffectKind kind)
{
  switch (kind)
  {
  case FFEffectKind::BLL:
  case FFEffectKind::LSH:
  case FFEffectKind::HSH:
    return true;
  default:
    return false;
  }
}

inline bool
FFEffectKindIsFilter(FFEffectKind kind)
{
  return FFEffectKindIsComb(kind) || FFEffectKindIsSVF(kind);
}