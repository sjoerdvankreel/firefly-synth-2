#include <firefly_synth/modules/glfo/FFGLFOState.hpp>
#include <firefly_synth/modules/glfo/FFGLFOProcessor.hpp>

FFGLFODSPState::
~FFGLFODSPState() {}

FFGLFODSPState::
FFGLFODSPState():
processor(std::make_unique<FFGLFOProcessor>()) {}