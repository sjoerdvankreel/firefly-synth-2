#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedOutputBlock& output)
{
  auto& proc = output.state.proc->param;
  for (int p = 0; p < proc.size(); p++)
    for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
      proc[p]->smoothedCV[s] = proc[p]->smooth.Next(proc[p]->rampedCV[s]);
}