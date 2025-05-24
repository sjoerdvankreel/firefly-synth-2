#include <playground_plug/modules/physical/FFPhysState.hpp>
#include <playground_plug/modules/physical/FFPhysProcessor.hpp>

FFPhysDSPState::
~FFPhysDSPState() {}

FFPhysDSPState::
FFPhysDSPState() :
processor(std::make_unique<FFPhysProcessor>()) {}