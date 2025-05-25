#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEffectTopo();

enum class FFEffectType { Off, Filter, Comb, Shaper };
enum class FFEffectParam { Type, ShaperGain, Count };