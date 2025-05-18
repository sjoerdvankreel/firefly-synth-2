#include <playground_plug/modules/ks_noise/FFKSNoiseState.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseProcessor.hpp>

FFKSNoiseDSPState::
~FFKSNoiseDSPState() {}

FFKSNoiseDSPState::
FFKSNoiseDSPState() :
processor(std::make_unique<FFKSNoiseProcessor>()) {}