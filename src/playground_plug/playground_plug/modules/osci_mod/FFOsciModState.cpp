#include <playground_plug/modules/osci_mod/FFOsciModState.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModProcessor.hpp>

FFOsciModDSPState::
~FFOsciModDSPState() {}

FFOsciModDSPState::
FFOsciModDSPState():
processor(std::make_unique<FFOsciModProcessor>()) {}