#include <playground_base/dsp/pipeline/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBRampProcessor::ProcessRamping(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  for (int p = 0; p < output.state.proc->pos.size(); p++)
    *output.state.proc->pos[p] = 0;
  for (int c = 0; c < output.state.proc->cv.size(); c++)
    output.state.proc->cv[c]->Fill(0, output.state.proc->cv[c]->Count(), *output.state.scalar->acc[c]);

  for (int a = 0; a < input.acc.size(); a++)
  {
    auto const& event = input.acc[a];
    int currentPos = *output.state.proc->pos[event.index];
    float currentVal = *output.state.scalar->acc[event.index];
    int posRange = event.pos - currentPos;
    float valRange = event.normalized - currentVal;
    *output.state.proc->pos[event.index] = event.pos;
    *output.state.scalar->acc[event.index] = event.normalized;

    for (int pos = 0; pos <= posRange; pos++)
      (*output.state.proc->cv[event.index])[currentPos + pos] =
      currentVal + pos / static_cast<float>(posRange) * valRange;
    if (a < input.acc.size() - 1 && input.acc[a + 1].index != event.index)
      for (int pos = event.pos; pos < input.audio.Count(); pos++)
        (*output.state.proc->cv[event.index])[pos] = event.normalized;
  }
}