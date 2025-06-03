#include <firefly_synth/modules/physical/FFPhysState.hpp>
#include <firefly_synth/modules/physical/FFPhysProcessor.hpp>

FFPhysDSPState::
~FFPhysDSPState() {}

FFPhysDSPState::
FFPhysDSPState() :
processor(std::make_unique<FFPhysProcessor>()) {}