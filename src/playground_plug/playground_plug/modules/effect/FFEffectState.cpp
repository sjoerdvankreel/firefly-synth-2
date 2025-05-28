#include <playground_plug/modules/effect/FFEffectState.hpp>
#include <playground_plug/modules/effect/FFEffectProcessor.hpp>

FFEffectDSPState::
~FFEffectDSPState() {}

FFEffectDSPState::
FFEffectDSPState() :
processor(std::make_unique<FFEffectProcessor>()) {}