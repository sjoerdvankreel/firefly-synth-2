#include <firefly_synth/modules/output/FFOutputState.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>

FFOutputDSPState::
~FFOutputDSPState() {}

FFOutputDSPState::
FFOutputDSPState():
processor(std::make_unique<FFOutputProcessor>()) {}