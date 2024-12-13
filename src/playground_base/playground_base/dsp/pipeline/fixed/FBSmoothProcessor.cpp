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
  // TODO mod
  auto& myAccAuto = _accAutoBySampleThenParam;
  auto& thatAccAuto = input.accAutoByParamThenSample;
  myAccAuto.clear();
  myAccAuto.insert(myAccAuto.begin(), thatAccAuto.begin(), thatAccAuto.end());
  auto compare = [](auto const& l, auto const& r) {
    if (l.pos < r.pos) return true;
    if (l.pos > r.pos) return false;
    return l.index < r.index; };
  std::sort(myAccAuto.begin(), myAccAuto.end(), compare);

  // todo deal with nondestructive and pervoice
  int eventIndex = 0;
  auto& params = output.state->Params();
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    for (int eventIndex = 0; 
      eventIndex < myAccAuto.size() && myAccAuto[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccAuto[eventIndex];
      if (!params[event.index].IsVoice())
        params[event.index].GlobalAcc().Value(event.normalized);
      else
        params[event.index].VoiceAcc().Value(event.normalized);
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