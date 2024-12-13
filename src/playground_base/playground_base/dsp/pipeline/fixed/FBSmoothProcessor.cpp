#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <algorithm>

template <class Event>
static void 
SortParamThenSampleToSampleThenParam(
  std::vector<Event> const& input,
  std::vector<Event>& output)
{
  auto compare = [](auto const& l, auto const& r) {
    if (l.pos < r.pos) return true;
    if (l.pos > r.pos) return false;
    return l.index < r.index; };
  output.clear();
  output.insert(output.begin(), input.begin(), input.end());
  std::sort(output.begin(), output.end(), compare);
}

FBSmoothProcessor::
FBSmoothProcessor(FBVoiceManager* voiceManager) :
_voiceManager(voiceManager) {}

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  SortParamThenSampleToSampleThenParam(
    input.accModByParamThenSample, _accModBySampleThenParam);
  SortParamThenSampleToSampleThenParam(
    input.accAutoByParamThenSample, _accAutoBySampleThenParam);

  auto& params = output.state->Params();
  auto const& myAccMod = _accModBySampleThenParam;
  auto const& myAccAuto = _accAutoBySampleThenParam;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    for (int eventIndex = 0;
      eventIndex < myAccMod.size() && myAccMod[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccMod[eventIndex];
      if (!params[event.index].IsVoice())
        params[event.index].GlobalAcc().Modulate(event.value);
      else // now it gets interesting TODO
        params[event.index].VoiceAcc().Value(event.value);
    }

    for (int eventIndex = 0;
      eventIndex < myAccAuto.size() && myAccAuto[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccAuto[eventIndex];
      if (!params[event.index].IsVoice())
        params[event.index].GlobalAcc().Value(event.value);
      else
        params[event.index].VoiceAcc().Value(event.value);
    }

    for (int p = 0; p < params.size(); p++)
      if(params[p].IsAcc())
        if (!params[p].IsVoice())
          params[p].GlobalAcc().SmoothNext(s);
        else
          for(int v = 0; v < FBMaxVoices; v++)
            if (_voiceManager->Voices()[v].active)
              params[p].VoiceAcc().SmoothNext(v, s);
  }
}