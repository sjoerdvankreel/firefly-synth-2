#include <firefly_synth/modules/env/FFEnvState.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>

FFEnvDSPState::
~FFEnvDSPState() {}

FFEnvDSPState::
FFEnvDSPState() :
processor(std::make_unique<FFEnvProcessor>()) {}