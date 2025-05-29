#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
inline int constexpr FFEffectBlockCount = 3;

enum class FFEffectType { Off, StVar, Comb, Dist };
enum class FFEffectDistMode { Clip, Shape, Skew };
enum class FFEffectDistSkewKind { Unipolar, Bipolar };
enum class FFEffectDistShapeKind { Off, Sin, Cos, Tri, Fldbk };
enum class FFEffectDistClipKind { Hard, TanH, Sin, TSQ, Cube, Inv, Exp };

enum class FFEffectParam { On, Oversample, Type, ShaperGain, 
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreq, StVarRes, StVarGain,  
  DistMode, DistGain, DistMix, DistClipKind, DistClipExp, DistSkewKind, DistSkewAmt, DistShapeKind1, DistShapeKind2, Count };