#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixTopo.hpp>

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeGMixTopo(bool isFx);

enum class FFGMixParam {
  Amp, LFO5ToAmp, Bal, LFO6ToBal,
  VoiceToGFX, ExtAudioToGFX, GFXToGFX,
  VoiceToOut, ExtAudioToOut, GFXToOut, Count };