#include <firefly_synth/modules/osci_mod/FFOsciModState.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

FFOsciModDSPState::
~FFOsciModDSPState() {}

FFOsciModDSPState::
FFOsciModDSPState():
processor(std::make_unique<FFOsciModProcessor>()) {}