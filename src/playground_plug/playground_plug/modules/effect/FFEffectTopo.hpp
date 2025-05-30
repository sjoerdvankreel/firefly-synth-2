#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
inline int constexpr FFEffectBlockCount = 4;

enum class FFEffectType { Off, StVar, Comb, Clip, Fold, Skew };
enum class FFEffectSkewMode { Uni, Bi };
enum class FFEffectClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp  };
enum class FFEffectFoldMode { 
  Fold, Sin, Cos, Sin2, Cos2, SinCos, CosSin, 
  Sin3, Cos3, Sn2Cs, Cs2Sn, SnCs2, CsSn2, SnCsSn, CsSnCs };

enum class FFEffectParam { On, Oversample, Type, ShaperGain, 
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreq, StVarRes, StVarGain,  
  ClipMode, FoldMode, SkewMode, DistDrive, DistMix, DistBias, DistAmt, Count };