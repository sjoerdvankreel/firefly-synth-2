#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <algorithm>

FBSmoothProcessor::
FBSmoothProcessor(FBVoiceManager* voiceManager) :
_voiceManager(voiceManager) {}

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{

#if 0
  auto& myAcc = _accBySampleThenParam;
  auto& thatAcc = input.accByParamThenSample;
  myAcc.clear();
  myAcc.insert(myAcc.begin(), thatAcc.begin(), thatAcc.end());
  auto compare = [](auto const& l, auto const& r) {
    if (l.pos < r.pos) return true;
    if (l.pos > r.pos) return false;
    return l.index < r.index; };
  std::sort(myAcc.begin(), myAcc.end(), compare);

  // todo deal with nondestructive and pervoice
  int eventIndex = 0;
  for (int s = 0; s < FBFixedCVBlock::Count(); s++)
  {
    for (int eventIndex = 0; 
      eventIndex < myAcc.size() && myAcc[eventIndex].pos == s; 
      eventIndex++)
    {
      auto const& event = myAcc[eventIndex];
      if (!output.state->Params()[event.index].IsVoice())
      {
        auto& global = output.state->Params()[event.index].GlobalAcc().Automate()
        global->value = event.normalized;
        global->proc.modulated = event.normalized; // TODO
      }
      else
      {
        auto& voice = output.state->voiceAcc[event.index];
        voice->value = event.normalized;
        for (int v = 0; v < FB_MAX_VOICES; v++)
          voice->proc[v].modulated = event.normalized; // TODO
      }
    }

    for (int p = 0; p < output.state->isAcc.size(); p++)
      if (!output.state->isVoice[p])
      {
        auto& global = output.state->globalAcc[p]->proc;
        global.smoothed[s] = global.smoother.Next(global.modulated);
      }
      else
      {
        for (int v = 0; v < FB_MAX_VOICES; v++)
        {
          auto& voice = output.state->voiceAcc[p]->proc[v];
          voice.smoothed[s] = voice.smoother.Next(voice.modulated);
        }
      }
  }
#endif
}