#include <playground_plug/modules/output/FFOutputState.hpp>
#include <playground_plug/modules/output/FFOutputProcessor.hpp>

FFOutputDSPState::
~FFOutputDSPState() {}

FFOutputDSPState::
FFOutputDSPState():
processor(std::make_unique<FFOutputProcessor>()) {}