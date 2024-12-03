#include <playground_base/base/state/FBProcState.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedOutputBlock& output)
{
  auto& dense = output.state->dense;
  for (int p = 0; p < dense.size(); p++)
    for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
      dense[p]->smoothedCV[s] = dense[p]->smooth.Next(dense[p]->rampedCV[s]);
}