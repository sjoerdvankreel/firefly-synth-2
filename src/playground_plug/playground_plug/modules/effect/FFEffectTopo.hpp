#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
inline int constexpr FFEffectBlockCount = 3;

enum class FFEffectType { Off, Shaper, Comb };
enum class FFEffectParam { On, Oversample, Type, ShaperGain, 
  CombKeyTrk, CombFreqPlus, CombResPlus, CombFreqMin, CombResMin, Count };