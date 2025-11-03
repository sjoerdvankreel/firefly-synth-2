#pragma once

#include <firefly_synth/dsp/shared/FFTrigFunctions.hpp>
#include <string>
#include <memory>

struct FBStaticModule;
inline int constexpr FFEffectBlockCount = 4; // must be <= lfo count and env count minus amp env for easy access mod
std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool global);

enum FFEffectFoldMode { FFEffectFoldModeFold = FFTrigCount };
enum class FFEffectSkewMode { Uni, Bi };
enum class FFEffectClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp  };

enum class FFEffectKind { 
  Off, 
  StVarFreq, StVarPitch,
  CombFreq, CombPitch,
  CombPlusFreq, CombPlusPitch,
  CombMinFreq, CombMinPitch,
  Clip, Fold, Skew };
std::string FFEffectKindToString(FFEffectKind kind);

enum class FFEffectParam { On, Oversample, TrackingKey, LastKeySmoothTime, 
  Kind, EnvAmt, LFOAmt,
  CombKeyTrk, CombFreqFreqPlus, CombPitchCoarsePlus, CombResPlus, CombFreqFreqMin, CombPitchCoarseMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreqFreq, StVarPitchCoarse, StVarRes, StVarGain,  
  ClipMode, FoldMode, SkewMode, DistDrive, DistMix, DistBias, DistAmt, Count };