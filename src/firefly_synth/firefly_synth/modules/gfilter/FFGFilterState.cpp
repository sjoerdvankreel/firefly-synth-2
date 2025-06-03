#include <firefly_synth/modules/gfilter/FFGFilterState.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterProcessor.hpp>

FFGFilterDSPState::
~FFGFilterDSPState() {}

FFGFilterDSPState::
FFGFilterDSPState():
processor(std::make_unique<FFGFilterProcessor>()) {}