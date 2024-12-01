#include <playground_base/dsp/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/fixed/FBFixedOutputBlock.hpp>

#include <playground_base/base/shared/FBProcAddrs.hpp>
#include <playground_base/base/shared/FBScalarAddrs.hpp>

void 
FBRampProcessor::ProcessRamping(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  for (int p = 0; p < output.proc->pos.size(); p++)
    *output.proc->pos[p] = 0;
  for (int c = 0; c < output.proc->cv.size(); c++)
    output.proc->cv[c]->Fill(0, output.proc->cv[c]->Count(), *output.scalar->acc[c]);

  for (int a = 0; a < input.acc.size(); a++)
  {
    auto const& event = input.acc[a];
    int currentPos = *output.proc->pos[event.index];
    float currentVal = *output.scalar->acc[event.index];
    int posRange = event.pos - currentPos;
    float valRange = event.normalized - currentVal;
    *output.proc->pos[event.index] = event.pos;
    *output.scalar->acc[event.index] = event.normalized;

    for (int pos = 0; pos <= posRange; pos++)
      (*output.proc->cv[event.index])[currentPos + pos] =
      currentVal + pos / static_cast<float>(posRange) * valRange;
    if (a < input.acc.size() - 1 && input.acc[a + 1].index != event.index)
      for (int pos = event.pos; pos < input.audio.Count(); pos++)
        (*output.proc->cv[event.index])[pos] = event.normalized;
  }
}