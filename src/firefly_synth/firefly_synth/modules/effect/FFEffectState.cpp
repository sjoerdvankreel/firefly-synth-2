#include <firefly_synth/modules/effect/FFEffectState.hpp>
#include <firefly_synth/modules/effect/FFEffectProcessor.hpp>

FFEffectDSPState::
~FFEffectDSPState() {}

FFEffectDSPState::
FFEffectDSPState() :
processor(std::make_unique<FFEffectProcessor>()) {}