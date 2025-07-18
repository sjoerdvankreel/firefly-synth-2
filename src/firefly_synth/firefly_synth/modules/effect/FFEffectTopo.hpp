#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_synth/dsp/shared/FFTrigFunctions.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFEffectBlockCount = 4;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool global);

enum FFEffectFoldMode { FFEffectFoldModeFold = FFTrigCount };
enum class FFEffectSkewMode { Uni, Bi };
enum class FFEffectClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp  };
enum class FFEffectKind { Off, StVar, Comb, CombPlus, CombMin, Clip, Fold, Skew };

enum class FFEffectParam { On, Oversample, TrackingKey, LastKeySmoothTime, Kind, 
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreq, StVarRes, StVarGain,  
  ClipMode, FoldMode, SkewMode, DistDrive, DistMix, DistBias, DistAmt, Count };