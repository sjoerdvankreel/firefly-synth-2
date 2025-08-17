#include <firefly_synth/modules/echo/FFGEchoProcessor.hpp>
#include <cmath>

void
FFGEchoProcessor::InitializeBuffers(float sampleRate)
{
  int maxSamples = (int)std::ceil(sampleRate * FFGEchoMaxSeconds);
  for (int t = 0; t < FFGEchoTapCount; t++)
    for (int c = 0; c < 2; c++)
      _delayLines[t][c].InitializeBuffers(maxSamples);
}
