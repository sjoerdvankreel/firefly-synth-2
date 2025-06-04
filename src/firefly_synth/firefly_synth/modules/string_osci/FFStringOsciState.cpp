#include <firefly_synth/modules/string_osci/FFStringOsciState.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciProcessor.hpp>

FFStringOsciDSPState::
~FFStringOsciDSPState() {}

FFStringOsciDSPState::
FFStringOsciDSPState() :
processor(std::make_unique<FFStringOsciProcessor>()) {}