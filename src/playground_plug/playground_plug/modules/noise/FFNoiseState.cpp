#include <playground_plug/modules/noise/FFNoiseState.hpp>
#include <playground_plug/modules/noise/FFNoiseProcessor.hpp>

FFNoiseDSPState::
~FFNoiseDSPState() {}

FFNoiseDSPState::
FFNoiseDSPState() :
processor(std::make_unique<FFNoiseProcessor>()) {}