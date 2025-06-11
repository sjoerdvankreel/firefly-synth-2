#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFEffectBlockCount = 4;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo(bool global);

enum class FFEffectKind { Off, StVar, Comb, CombPlus, CombMin, Clip, Fold, Skew };
enum class FFEffectSkewMode { Uni, Bi };
enum class FFEffectClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp  };
enum class FFEffectFoldMode { 
  Fold, Sin, Cos, Sin2, Cos2, SinCos, CosSin, 
  Sin3, Cos3, Sn2Cs, Cs2Sn, SnCs2, CsSn2, SnCsSn, CsSnCs };

enum class FFEffectParam { On, Oversample, TrackingKey, LastKeySmoothTime, Kind, 
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreq, StVarRes, StVarGain,  
  ClipMode, FoldMode, SkewMode, DistDrive, DistMix, DistBias, DistAmt, Count };