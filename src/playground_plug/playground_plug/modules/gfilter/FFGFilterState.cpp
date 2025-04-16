#include <playground_plug/modules/gfilter/FFGFilterState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

FFGFilterDSPState::
~FFGFilterDSPState() {}

FFGFilterDSPState::
FFGFilterDSPState():
processor(std::make_unique<FFGFilterProcessor>()) {}