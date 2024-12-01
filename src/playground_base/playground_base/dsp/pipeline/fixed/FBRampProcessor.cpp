#include <playground_base/dsp/pipeline/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBRampProcessor::ProcessRamping(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  for (int p = 0; p < output.state.proc->param.size(); p++)
  {
    output.state.proc->param[p]->pos = 0;
    output.state.proc->param[p]->cv.Fill(
      0, output.state.proc->param[p]->cv.Count(), *output.state.scalar->acc[p]);
  }

  for (int a = 0; a < input.acc.size(); a++)
  {
    auto const& event = input.acc[a];
    float currentVal = *output.state.scalar->acc[event.index];
    int currentPos = output.state.proc->param[event.index]->pos;
    int posRange = event.pos - currentPos;
    float valRange = event.normalized - currentVal;
    output.state.proc->param[event.index]->pos = event.pos;
    *output.state.scalar->acc[event.index] = event.normalized;

    for (int pos = 0; pos <= posRange; pos++)
      output.state.proc->param[event.index]->cv[currentPos + pos] =
      currentVal + pos / static_cast<float>(posRange) * valRange;
    if (a < input.acc.size() - 1 && input.acc[a + 1].index != event.index)
      for (int pos = event.pos; pos < input.audio.Count(); pos++)
        output.state.proc->param[event.index]->cv[pos] = event.normalized;
  }
}