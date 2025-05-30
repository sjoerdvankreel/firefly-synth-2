#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
inline int constexpr FFEffectBlockCount = 3;

enum class FFEffectType { Off, StVar, Comb, Clip, Shape };
enum class FFEffectClipMode { Hard, TanH, Sin, TSQ, Cube, Inv, Exp  };
enum class FFEffectShapeMode { 
  Off, Tri, Fldbk, SkewU, SkewB, Sin, Cos, Sin2, Cos2, SinCos, CosSin, 
  Sin3, Cos3, Sn2Cs, Cs2Sn, SnCs2, CsSn2, SnCsSn, CsSnCs };

enum class FFEffectParam { On, Oversample, Type, ShaperGain, 
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, 
  StVarMode, StVarKeyTrak, StVarFreq, StVarRes, StVarGain,  
  ClipMode, ShapeMode, DistDrive, DistMix, DistAmt, DistFdbk, Count };