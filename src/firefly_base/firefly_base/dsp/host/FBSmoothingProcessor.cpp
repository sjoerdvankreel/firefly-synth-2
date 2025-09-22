#include <firefly_base/base/state/proc/FBAccParamState.hpp>
#include <firefly_base/base/state/proc/FBVoiceAccParamState.hpp>
#include <firefly_base/base/state/proc/FBGlobalAccParamState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/dsp/buffer/FBBufferBlock.hpp>
#include <firefly_base/dsp/host/FBSmoothingProcessor.hpp>

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

static void
RemoveIfNotExists(std::vector<int>& params, int param)
{
  auto iter = std::find(params.begin(), params.end(), param);
  if (iter != params.end())
    params.erase(iter);
}

static void
InsertIfNotExists(std::vector<int>& params, int param)
{
  if (std::find(params.begin(), params.end(), param) == params.end())
    params.push_back(param);
}

static void
InsertMustNotExist(std::vector<int>& params, int param)
{
#ifndef NDEBUG
  auto iter = std::find(params.begin(), params.end(), param);
  FB_ASSERT(iter == params.end());
#endif
  params.push_back(param);
}

FBSmoothingState::
FBSmoothingState(int paramCount)
{
  activeSamples.resize(paramCount);
}

void 
FBSmoothingState::Begin(int param, int smoothingSamples)
{
  InsertIfNotExists(active, param);
  RemoveIfNotExists(finished, param);
  activeSamples[param] = smoothingSamples;
}

std::vector<int>::iterator 
FBSmoothingState::Finish(std::vector<int>::iterator iter)
{
  activeSamples[*iter] = 0;
  InsertMustNotExist(finished, *iter);
  return active.erase(iter);
}

FBSmoothingProcessor::
FBSmoothingProcessor(FBVoiceManager* voiceManager, int paramCount) :
_voiceManager(voiceManager),
_midi(FBMIDIEvent::MessageCount),
_global(paramCount),
_noteMatrix((int)FBNoteMatrixEntry::Count),
_voice()
{
  for (int i = 0; i < FBMaxVoices; i++)
    _voice[i].activeSamples.resize(paramCount);
}

void 
FBSmoothingProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBPlugOutputBlock& output, int smoothingSamples)
{
  auto& params = output.procState->Params();
  auto& midiParams = output.procState->MIDIParams();
  auto& noteMatrixSmth = output.procState->NoteMatrixSmth();

  for (int midiParam : _midi.finished)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      midiParams[midiParam].SmoothNextHostValue(s);
  _midi.finished.clear();

  for (int param : _global.finished)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      params[param].GlobalAcc().SmoothNextHostValue(s);
  _global.finished.clear();

  for (int noteEntry : _noteMatrix.finished)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      noteMatrixSmth.entries[noteEntry].SmoothNextHostValue(s);
  _noteMatrix.finished.clear();

  for (int v: _voiceManager->ActiveVoices())
    for (int param : _voice[v].finished)
      for (int s = 0; s < FBFixedBlockSamples; s++)
        params[param].VoiceAcc().SmoothNextHostValue(v, s);

  for (int v = 0; v < FBMaxVoices; v++)
    _voice[v].finished.clear();

  SortSampleLastToSampleFirst(input.accAutoByParamThenSample,
    _accAutoBySampleThenParam, FBAccAutoEventOrderByPosThenParam);
  SortSampleLastToSampleFirst(input.midiByMessageThenCCThenSample,
    _midiBySampleThenMessageThenCC, FBMIDIEventOrderByPosThenMessageThenCC);
  SortSampleLastToSampleFirst(input.accModByParamThenNoteThenSample,
    _accModBySampleThenParamThenNote, FBAccModEventOrderByPosThenParamThenNote);

  auto const& myMIDI = _midiBySampleThenMessageThenCC;
  auto const& myAccAuto = _accAutoBySampleThenParam;
  auto const& myAccMod = _accModBySampleThenParamThenNote;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    for (int eventIndex = 0;
      eventIndex < myMIDI.size() && myMIDI[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myMIDI[eventIndex];
      midiParams[event.EventId()].Value(event.value);
      _midi.Begin(event.EventId(), smoothingSamples);
    }

    for (int eventIndex = 0;
      eventIndex < input.noteEvents.size() && input.noteEvents[eventIndex].pos == s;
      eventIndex++)
    {
      // took a shortcut here, every note event invalidates the filter state of the entire matrix
      for (int i = 0; i < (int)FBNoteMatrixEntry::Count; i++)
      {
        noteMatrixSmth.entries[i].Value(input.noteMatrixRaw.entries[i].Get(s));
        _noteMatrix.Begin(i, smoothingSamples);
      }
    }

    for (int eventIndex = 0;
      eventIndex < myAccAuto.size() && myAccAuto[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccAuto[eventIndex];
      if (!params[event.param].IsVoice())
      {
        params[event.param].GlobalAcc().Value(event.value);
        _global.Begin(event.param, smoothingSamples);
      }
      else
      {
        params[event.param].VoiceAcc().GlobalValue(event.value);
        for (int v: _voiceManager->ActiveVoices())
          _voice[v].Begin(event.param, smoothingSamples);
      }
    }

    for (int eventIndex = 0;
      eventIndex < myAccMod.size() && myAccMod[eventIndex].pos == s;
      eventIndex++)
    {
      auto const& event = myAccMod[eventIndex];
      if (!params[event.param].IsVoice())
      {
        params[event.param].GlobalAcc().ModulateByHost(event.value);
        _global.Begin(event.param, smoothingSamples);
      }
      else
        for (int v: _voiceManager->ActiveVoices())
        {
          auto const& voice = _voiceManager->Voices()[v];
          if (event.note.Matches(voice.event.note))
          {
            params[event.param].VoiceAcc().ModulateByHost(v, event.value);
            _voice[v].Begin(event.param, smoothingSamples);
          }
        }
    }

    for (auto iter = _midi.active.begin(); iter != _midi.active.end(); )
    {
      midiParams[*iter].SmoothNextHostValue(s);
      if (--_midi.activeSamples[*iter] <= 0)
        iter = _midi.Finish(iter);
      else
        iter++;
    }
    
    for (auto iter = _noteMatrix.active.begin(); iter != _noteMatrix.active.end(); )
    {
      noteMatrixSmth.entries[*iter].SmoothNextHostValue(s);
      if (--_noteMatrix.activeSamples[*iter] <= 0)
        iter = _noteMatrix.Finish(iter);
      else
        iter++;
    }

    for(auto iter = _global.active.begin(); iter != _global.active.end(); )
    {
      params[*iter].GlobalAcc().SmoothNextHostValue(s);
      if (--_global.activeSamples[*iter] <= 0)
        iter = _global.Finish(iter);
      else
        iter++;      
    }

    for (int v: _voiceManager->ActiveVoices())
      for (auto iter = _voice[v].active.begin(); iter != _voice[v].active.end(); )
      {
        params[*iter].VoiceAcc().SmoothNextHostValue(v, s);
        if (--_voice[v].activeSamples[*iter] <= 0)
          iter = _voice[v].Finish(iter);
        else
          iter++;
      }
  }
}