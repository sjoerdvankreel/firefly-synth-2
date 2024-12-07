#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
#if 0 // TODO
  _accBySampleThenParam.clear();
  _accBySampleThenParam.insert(_accBySampleThenParam.begin(), input.acc)
  auto& acc = output.state->single.acc;
  for (int p = 0; p < output.state->isAcc.size(); p++)
    if(output.state->isAcc[p])
      for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
        acc[p]->smoothedCV[s] = acc[p]->smooth.Next(acc[p]->rampedCV[s]);
#endif
}