#include <firefly_synth/modules/osci/FFOsciState.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

FFOsciDSPState::
~FFOsciDSPState() {}

FFOsciDSPState::
FFOsciDSPState() :
processor(std::make_unique<FFOsciProcessor>()) {}