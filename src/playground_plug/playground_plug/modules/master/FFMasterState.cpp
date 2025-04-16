#include <playground_plug/modules/master/FFMasterState.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>

FFMasterDSPState::
~FFMasterDSPState() {}

FFMasterDSPState::
FFMasterDSPState():
processor(std::make_unique<FFMasterProcessor>()) {}