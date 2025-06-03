#include <firefly_synth/modules/master/FFMasterState.hpp>
#include <firefly_synth/modules/master/FFMasterProcessor.hpp>

FFMasterDSPState::
~FFMasterDSPState() {}

FFMasterDSPState::
FFMasterDSPState():
processor(std::make_unique<FFMasterProcessor>()) {}