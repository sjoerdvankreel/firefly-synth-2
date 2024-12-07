#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <algorithm>

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  _accBySampleThenParam.clear();
  _accBySampleThenParam.insert(
    _accBySampleThenParam.begin(), 
    input.accByParamThenSample.begin(), 
    input.accByParamThenSample.end());
  auto compare = [](auto const& l, auto const& r) {
    if (l.pos < r.pos) return true;
    if (l.pos > r.pos) return false;
    return l.index < r.index; };
  std::sort(_accBySampleThenParam.begin(), _accBySampleThenParam.end(), compare);

  // todo deal with nondestructive and pervoice
  int eventIndex = 0;
  auto& acc = _accBySampleThenParam;
  for (int s = 0; s < FBFixedCVBlock::Count(); s++)
  {
    while (eventIndex < _accBySampleThenParam.size() &&
      _accBySampleThenParam[eventIndex].pos == s)
    {
      auto const& event = _accBySampleThenParam[eventIndex];
      if(!output.state->isVoice[event.index])
        output.state->globalAcc[event.index]->proc.
    }
  }

    
  for (int p = 0; p < output.state->isAcc.size(); p++)
    if(output.state->isAcc[p])

}