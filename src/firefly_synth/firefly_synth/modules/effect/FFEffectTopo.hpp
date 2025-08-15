#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_synth/dsp/shared/FFTrigFunctions.hpp>

#include <string>
#include <memory>

struct FBStaticModule;
inline int constexpr FFEffectBlockCount = FFLFOCount; // easy access mod
std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool global);

enum FFEffectFoldMode { FFEffectFoldModeFold = FFTrigCount };
enum class FFEffectSkewMode { Uni, Bi };
enum class FFEffectClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp  };

enum class FFEffectKind { Off, StVar, Comb, CombPlus, CombMin, Clip, Fold, Skew };
std::string FFEffectKindToString(FFEffectKind kind);

enum class FFEffectParam { On, Oversample, TrackingKey, LastKeySmoothTime, 
  Kind, EnvAmt, LFOAmt,
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreq, StVarRes, StVarGain,  
  ClipMode, FoldMode, SkewMode, DistDrive, DistMix, DistBias, DistAmt, Count };