#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo();
inline int constexpr FFEffectBlockCount = 4;

enum class FFEffectType { Off, Shaper, Comb };
enum class FFEffectParam { On, Oversample, Type, ShaperGain, CombDelayPlus, CombGainPlus, CombDelayMin, CombGainMin, Count };