#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
inline int constexpr FFEffectBlockCount = 3;

enum class FFEffectType { Off, StVar, Comb, Dist };
enum class FFEffectDistMode { Clip, Shape, Skew };
enum class FFEffectDistSkewMode { Unipolar, Bipolar };
enum class FFEffectDistClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp };
enum class FFEffectDistShapeMode { 
  Sin, Cos, Tri, Foldback, SinSin, SinCos, CosSin, CosCos, 
  SinSinSin, SinSinCos, SinCosSin, SinCosCos, CosSinSin, CosSinCos, CosCosSin, CosCosCos };

enum class FFEffectParam { On, Oversample, Type, ShaperGain, 
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreq, StVarRes, StVarGain,  
  DistMode, DistGain, DistMix, DistClipMode, DistClipExp, DistSkewMode, DistSkewAmt, DistShapeMode, Count };