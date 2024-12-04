#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>
#include <playground_base/dsp/pipeline/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBRampProcessor::ProcessRamping(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  auto& acc = output.state->acc;
  for (int p = 0; p < output.state->isBlock.size(); p++)
    if(!output.state->isBlock[p])
    {
      acc[p]->pos = 0;
      acc[p]->rampedCV.Fill(0, acc[p]->rampedCV.Count(), acc[p]->scalar);
    }

  for (int a = 0; a < input.acc.size(); a++)
  {
    auto const& event = input.acc[a];
    int currentPos = acc[event.index]->pos;
    float currentVal = acc[event.index]->scalar;
    int posRange = event.pos - currentPos;
    float valRange = event.normalized - currentVal;
    acc[event.index]->pos = event.pos;
    acc[event.index]->scalar = event.normalized;

    for (int pos = 0; pos < posRange; pos++)
      acc[event.index]->rampedCV[currentPos + pos] =
      currentVal + pos / static_cast<float>(posRange) * valRange;
    if (a < input.acc.size() - 1 && input.acc[a + 1].index != event.index)
      for (int pos = event.pos; pos < input.audio.Count(); pos++)
        acc[event.index]->rampedCV[pos] = event.normalized;
  }
}