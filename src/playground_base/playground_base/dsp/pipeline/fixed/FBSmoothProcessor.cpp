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
FBSmoothProcessor(FBVoiceManager* voiceManager, int paramCount) :
_voiceManager(voiceManager)
{
  _activeGlobalSmoothingSamples.resize(paramCount);
  for (int v = 0; v < FBMaxVoices; v++)
    _activeVoiceSmoothingSamples[v].resize(paramCount);
}

void 
FBSmoothProcessor::InsertIfNotExists(std::vector<int>& params, int param)
{
  params.insert(std::upper_bound(params.begin(), params.end(), param), param);
}

void
FBSmoothProcessor::FinishGlobalSmoothing(int param)
{
  _activeGlobalSmoothing.erase(param);
  _finishedGlobalSmoothing.insert(param);
  _activeGlobalSmoothingSamples[param] = 0;
}

void 
FBSmoothProcessor::BeginGlobalSmoothing(int param, int smoothingSamples)
{
  _activeGlobalSmoothing.insert(param);
  _finishedGlobalSmoothing.erase(param);
  _activeGlobalSmoothingSamples[param] = smoothingSamples;
}

void
FBSmoothProcessor::FinishVoiceSmoothing(int voice, int param)
{
  _activeVoiceSmoothing[voice].erase(param);
  _finishedVoiceSmoothing[voice].insert(param);
  _activeVoiceSmoothingSamples[voice][param] = 0;
}

void 
FBSmoothProcessor::BeginVoiceSmoothing(int voice, int param, int smoothingSamples)
{
  _activeVoiceSmoothing[voice].insert(param);
  _finishedVoiceSmoothing[voice].erase(param);
  _activeVoiceSmoothingSamples[voice][param] = smoothingSamples;
}

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output, int smoothingSamples)
{
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

  SortSampleLastToSampleFirst(input.accAutoByParamThenSample,
    _accAutoBySampleThenParam, FBAccAutoEventOrderByPosThenParam);
  SortSampleLastToSampleFirst(input.accModByParamThenNoteThenSample, 
    _accModBySampleThenParamThenNote, FBAccModEventOrderByPosThenParamThenNote);

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
      {
        params[event.param].GlobalAcc().Value(event.value);
        BeginGlobalSmoothing(event.param, smoothingSamples);
      }
      else
      {
        params[event.param].VoiceAcc().Value(event.value);
        for (int v = 0; v < FBMaxVoices; v++)
          if (_voiceManager->IsActive(v))
            BeginVoiceSmoothing(v, event.param, smoothingSamples);
      }
    }

    for (int eventIndex = 0;
      eventIndex < myAccMod.size() && myAccMod[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccMod[eventIndex];
      if (!params[event.param].IsVoice())
      {
        params[event.param].GlobalAcc().Modulate(event.value);
        BeginGlobalSmoothing(event.param, smoothingSamples);
      }
      else
        for (int v = 0; v < FBMaxVoices; v++)
        {
          auto const& voice = _voiceManager->Voices()[v];
          if (_voiceManager->IsActive(v) && event.note.Matches(voice.event.note))
          {
            params[event.param].VoiceAcc().Modulate(v, event.value);
            BeginVoiceSmoothing(v, event.param, smoothingSamples);
          }
        }
    }

    for (int param : _activeGlobalSmoothing)
    {
      params[param].GlobalAcc().SmoothNext(s);
      if (--_activeGlobalSmoothingSamples[param] <= 0)
        FinishGlobalSmoothing(param);
    }

    for (int v = 0; v < FBMaxVoices; v++)
      if (_voiceManager->IsActive(v))
        for (int param : _activeVoiceSmoothing[v])
        {
          params[param].VoiceAcc().SmoothNext(v, s);
          if (--_activeVoiceSmoothingSamples[v][param] <= 0)
            FinishVoiceSmoothing(v, param);
        }
  }
}