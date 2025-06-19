#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixTopo.hpp>

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeGMixTopo();

enum class FFGMixParam {
  VoiceToGFX, GFXToGFX,
  VoiceToOut, GFXToOut, Count };