#include <playground_plug/modules/glfo/FFGLFOState.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

FFGLFODSPState::
~FFGLFODSPState() {}

FFGLFODSPState::
FFGLFODSPState():
processor(std::make_unique<FFGLFOProcessor>()) {}