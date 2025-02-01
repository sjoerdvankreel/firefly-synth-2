#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothingProcessor.hpp>

#include <cassert>
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

FBSmoothingProcessor::
FBSmoothingProcessor(FBVoiceManager* voiceManager, int paramCount) :
_voiceManager(voiceManager)
{
  _activeGlobalSmoothingSamples.resize(paramCount);
  for (int v = 0; v < FBMaxVoices; v++)
    _activeVoiceSmoothingSamples[v].resize(paramCount);
}

void
FBSmoothingProcessor::BeginGlobalSmoothing(int param, int smoothingSamples)
{
  InsertIfNotExists(_activeGlobalSmoothing, param);
  RemoveIfNotExists(_finishedGlobalSmoothing, param);
  _activeGlobalSmoothingSamples[param] = smoothingSamples;
}

std::vector<int>::iterator
FBSmoothingProcessor::FinishGlobalSmoothing(std::vector<int>::iterator iter)
{
  _activeGlobalSmoothingSamples[*iter] = 0;
  InsertMustNotExist(_finishedGlobalSmoothing, *iter);
  return _activeGlobalSmoothing.erase(iter);
}

void
FBSmoothingProcessor::BeginVoiceSmoothing(int voice, int param, int smoothingSamples)
{
  InsertIfNotExists(_activeVoiceSmoothing[voice], param);
  RemoveIfNotExists(_finishedVoiceSmoothing[voice], param);
  _activeVoiceSmoothingSamples[voice][param] = smoothingSamples;
}

std::vector<int>::iterator
FBSmoothingProcessor::FinishVoiceSmoothing(int voice, std::vector<int>::iterator iter)
{
  _activeVoiceSmoothingSamples[voice][*iter] = 0;
  InsertMustNotExist(_finishedVoiceSmoothing[voice], *iter);
  return _activeVoiceSmoothing[voice].erase(iter);
}

void
FBSmoothingProcessor::RemoveIfNotExists(std::vector<int>& params, int param)
{
  auto iter = std::find(params.begin(), params.end(), param);
  if(iter != params.end())
    params.erase(iter);
}

void
FBSmoothingProcessor::InsertIfNotExists(std::vector<int>& params, int param)
{
  if (std::find(params.begin(), params.end(), param) == params.end())
    params.push_back(param);
}

void
FBSmoothingProcessor::InsertMustNotExist(std::vector<int>& params, int param)
{
#ifndef NDEBUG
  auto iter = std::find(params.begin(), params.end(), param);
  assert(iter == params.end());
#endif
  params.push_back(param);
}

void 
FBSmoothingProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output, int smoothingSamples)
{
  auto& params = output.procState->Params();
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

    for(auto iter = _activeGlobalSmoothing.begin(); iter != _activeGlobalSmoothing.end(); )
    {
      params[*iter].GlobalAcc().SmoothNext(s);
      if (--_activeGlobalSmoothingSamples[*iter] <= 0)
        iter = FinishGlobalSmoothing(iter);
      else
        iter++;      
    }

    for (int v = 0; v < FBMaxVoices; v++)
      if (_voiceManager->IsActive(v))
        for (auto iter = _activeVoiceSmoothing[v].begin(); iter != _activeVoiceSmoothing[v].end(); )
        {
          params[*iter].VoiceAcc().SmoothNext(v, s);
          if (--_activeVoiceSmoothingSamples[v][*iter] <= 0)
            iter = FinishVoiceSmoothing(v, iter);
          else
            iter++;
        }
  }
}