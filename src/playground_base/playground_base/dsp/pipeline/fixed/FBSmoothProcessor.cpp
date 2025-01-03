#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <algorithm>

template <class Event, class Compare>
static void 
SortSampleLastToSampleFirst(
  std::vector<Event> const& input,
  std::vector<Event>& output,
  Compare compare)
{
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
  SortSampleLastToSampleFirst(input.accAutoByParamThenSample, 
    _accAutoBySampleThenParam, FBAccAutoEventOrderByPosThenParam);
  SortSampleLastToSampleFirst(input.accModByParamThenNoteThenSample, 
    _accModBySampleThenParamThenNote, FBAccModEventOrderByPosThenParamThenNote);

  auto& params = output.state->Params();
  for (int param : _finishedGlobalSmoothing)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      params[param].GlobalAcc().SmoothNext(s);
  _finishedGlobalSmoothing.clear();
  for (int v = 0; v < FBMaxVoices; v++)
  {
    if (_voiceManager->IsActive(v))
      for (int param : _finishedVoiceSmoothing[v])
        for (int s = 0; s < FBFixedBlockSamples; s++)
          params[param].VoiceAcc().SmoothNext(v, s);
    _finishedVoiceSmoothing[v].clear();
  }

  auto const& myAccAuto = _accAutoBySampleThenParam;
  auto const& myAccMod = _accModBySampleThenParamThenNote;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    for (int eventIndex = 0;
      eventIndex < myAccAuto.size() && myAccAuto[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccAuto[eventIndex];
      if (!params[event.param].IsVoice())
        params[event.param].GlobalAcc().Value(event.value);
      else
        params[event.param].VoiceAcc().Value(event.value);
    }

    for (int eventIndex = 0;
      eventIndex < myAccMod.size() && myAccMod[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccMod[eventIndex];
      if (!params[event.param].IsVoice())
        params[event.param].GlobalAcc().Modulate(event.value);
      else
        for (int v = 0; v < FBMaxVoices; v++)
        {
          auto const& voice = _voiceManager->Voices()[v];
          if (_voiceManager->IsActive(v) && event.note.Matches(voice.event.note))
            params[event.param].VoiceAcc().Modulate(v, event.value);
        }
    }

    for (int i : _activeGlobalSmoothing)
      ++boohoo;
    for (int v = 0; v < FBMaxVoices; v++)
      for (int i : _activeVoiceSmoothing[v])
        if (_voiceManager->IsActive(v))
          ++boohoo;
#if 0


    for (int p = 0; p < params.size(); p++)
      if(params[p].IsAcc())
        if (!params[p].IsVoice())
          params[p].GlobalAcc().SmoothNext(s);
        else
          for(int v = 0; v < FBMaxVoices; v++)
            if (_voiceManager->IsActive(v))
              params[p].VoiceAcc().SmoothNext(v, s);
#endif
  }
}