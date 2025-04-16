#include <playground_plug/modules/env/FFEnvState.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

FFEnvDSPState::
~FFEnvDSPState() {}

FFEnvDSPState::
FFEnvDSPState() :
processor(std::make_unique<FFEnvProcessor>()) {}