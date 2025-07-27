#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <cmath>

void
FFEnvProcessor::BeginVoice(FBModuleProcState& state)
{
  _smoother = {};
  _finished = false;
  _released = false;
  _lastOverall = 0.0f;
  _smoothPosition = 0;
  _positionSamples = 0;
  _stagePositions.fill(0);
  _lastBeforeRelease = 0.0f;

  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Env];
  auto const& stageBarsNorm = params.block.stageBars;
  auto const& stageTimeNorm = params.block.stageTime;
  auto const& syncNorm = params.block.sync[0].Voice()[voice];
  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& releaseNorm = params.block.release[0].Voice()[voice];
  auto const& loopStartNorm = params.block.loopStart[0].Voice()[voice];
  auto const& loopLengthNorm = params.block.loopLength[0].Voice()[voice];
  auto const& smoothBarsNorm = params.block.smoothBars[0].Voice()[voice];
  auto const& smoothTimeNorm = params.block.smoothTime[0].Voice()[voice];

  _sync = topo.NormalizedToBoolFast(FFEnvParam::Sync, syncNorm);
  _type = topo.NormalizedToListFast<FFEnvType>(FFEnvParam::Type, typeNorm);
  _releasePoint = topo.NormalizedToDiscreteFast(FFEnvParam::Release, releaseNorm);
  _loopStart = topo.NormalizedToDiscreteFast(FFEnvParam::LoopStart, loopStartNorm);
  _loopLength = topo.NormalizedToDiscreteFast(FFEnvParam::LoopStart, loopLengthNorm);

  if (_sync)
  {
    _smoothSamples = topo.NormalizedToBarsSamplesFast(
      FFEnvParam::SmoothBars, smoothBarsNorm, state.input->sampleRate, state.input->bpm);
    for (int i = 0; i < FFEnvStageCount; i++)
      _stageSamples[i] = topo.NormalizedToBarsSamplesFast(
        FFEnvParam::StageBars, stageBarsNorm[i].Voice()[voice], 
        state.input->sampleRate, state.input->bpm);
  }
  else
  {
    _smoothSamples = topo.NormalizedToLinearTimeSamplesFast(
      FFEnvParam::SmoothTime, smoothTimeNorm, state.input->sampleRate);
    for (int i = 0; i < FFEnvStageCount; i++)
      _stageSamples[i] = topo.NormalizedToLinearTimeSamplesFast(
        FFEnvParam::StageTime, stageTimeNorm[i].Voice()[voice], state.input->sampleRate);
  }

  _lengthSamples = 0;
  _lengthSamplesUpToRelease = 0;
  _lengthSamplesUpToStage = {};
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    _lengthSamplesUpToStage[i] = _lengthSamples;
    _lengthSamples += _stageSamples[i];
    if (i < _releasePoint)
      _lengthSamplesUpToRelease += _stageSamples[i];    
  }
  _lengthSamples += _smoothSamples;

  for (int i = 0; i < FFEnvStageCount; i++)
    if (_stageSamples[i] > 0)
    {
      if (i == 0)
      {
        _lastOverall = 0.0f;
        _lastBeforeRelease = 0.0f;
      }
      else
      {
        float lvlNorm = params.acc.stageLevel[i - 1].Voice()[voice].CV().Get(0);
        float lvlPlain = topo.NormalizedToIdentityFast(FFEnvParam::StageLevel, lvlNorm);
        _lastOverall = lvlPlain;
        _lastBeforeRelease = lvlPlain;
      }
      break;
    }

  _smoother.SetCoeffs(_smoothSamples);
  _smoother.State(_lastOverall);
}

int 
FFEnvProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.env[state.moduleSlot];
  auto& output = procState->dsp.voice[voice].env[state.moduleSlot].output;
  auto const& stageLevel = procParams.acc.stageLevel;
  auto const& stageSlope = procParams.acc.stageSlope;

  if (_type == FFEnvType::Off)
  {
    output.Fill(0.0f);
    return 0;
  }

  if (_finished)
  {
    output.Fill(_smoother.State());
    return 0;
  }

  int s = 0;
  int releaseAt = -1;
  float const minSlope = 0.001f;
  float const slopeRange = 1.0f - 2.0f * minSlope;
  float const invLogHalf = 1.0f / std::log(0.5f);

  auto const& noteEvents = *state.input->noteEvents;
  auto const& myVoiceNoteEvent = state.input->voiceManager->Voices()[voice].event;
  int loopEnd = _loopStart == 0 ? -1 : _loopStart - 1 + _loopLength;
  loopEnd = std::min(loopEnd, FFEnvStageCount);
  bool graphing = state.renderType != FBRenderType::Audio;
  bool graphingExchange = state.renderType == FBRenderType::GraphExchange || state.anyExchangeActive;
  bool forcedRelease = _releasePoint == 0 && _loopStart != 0 && state.moduleSlot == 0;
  int releasePoint = forcedRelease ? FFEnvStageCount : _releasePoint;

  // Be sure to only release a voice by a note off event that comes strictly 
  // after (in time) the note on event that triggered the current voice.
  if (releasePoint != 0 && !graphingExchange)
    for (int i = 0; i < noteEvents.size(); i++)
      if (!noteEvents[i].on && 
        noteEvents[i].note.Matches(myVoiceNoteEvent.note) &&
        (noteEvents[i].timeStampSamples == 0 || // running outside transport, just kill any match
         myVoiceNoteEvent.timeStampSamples == 0 ||
        noteEvents[i].timeStampSamples > myVoiceNoteEvent.timeStampSamples))
      {
        releaseAt = noteEvents[i].pos;
        break;
      }

  // Deal with voice start offset.
  // In other words, the entire audio engine operates on
  // 16-sample blocks, and we fake starting a voice at
  // subblock position by delaying the voice amp envelope.
  if (state.moduleSlot == 0)
    for (; s < state.voice->offsetInBlock; s++)
      output.Set(s, 0.0f);
  
  int stage = 0;
  while(stage < FFEnvStageCount)
  {
    bool stageReset = false;
    int& stagePos = _stagePositions[stage];
    int stageSamples = _stageSamples[stage];
    while (s < FBFixedBlockSamples && stagePos < stageSamples)
    {
      float stageStart;
      float pos = stagePos / static_cast<float>(stageSamples);
      float stageEnd = stageLevel[stage].Voice()[voice].CV().Get(s);
      if (releasePoint != 0 && stage == releasePoint && _released)
        stageStart = _lastBeforeRelease;
      else
        stageStart = stage == 0 ? 0.0f : stageLevel[stage - 1].Voice()[voice].CV().Get(s);

      if(_type == FFEnvType::Linear)
        _lastOverall = stageStart + (stageEnd - stageStart) * pos;
      else
      {
        float slope = minSlope + stageSlope[stage].Voice()[voice].CV().Get(s) * slopeRange;
        _lastOverall = stageStart + (stageEnd - stageStart) * std::pow(pos, std::log(slope) * invLogHalf);
      }
      output.Set(s, _smoother.Next(_lastOverall));

      bool isReleaseNow = s == releaseAt;
      s++;

      if (releasePoint != 0 && !_released)
      {
        _lastBeforeRelease = _lastOverall;
        if (isReleaseNow)
        {
          _released = true;
          if (stage < releasePoint)
          {
            stage = releasePoint;
            for (int ps = 0; ps < stage; ps++)
              _stagePositions[ps] = _stageSamples[ps];
            for (int ps = stage; ps < FFEnvStageCount; ps++)
              _stagePositions[ps] = 0;
            _positionSamples = _lengthSamplesUpToRelease;
            stageReset = true;
            break;
          }
        }
      }

      if (!graphing && !_released)
      {
        if (stage == _loopStart - 1 && _loopLength == 0)
        {
          _stagePositions[stage] = 0;
          _positionSamples = _lengthSamplesUpToStage[stage];
          stageReset = true;
          break;
        }
        else if (stagePos == stageSamples - 1 && stage == loopEnd - 1)
        {
          for (int ps = _loopStart - 1; ps < loopEnd; ps++)
            _stagePositions[ps] = 0;
          _positionSamples = _lengthSamplesUpToStage[_loopStart - 1];
          stage = _loopStart - 1;
          stageReset = true;
          break;
        }
      }

      stagePos++;
      _positionSamples++;
    }      

    if (!stageReset)
      stage++;
  }

  for (; s < FBFixedBlockSamples && _smoothPosition < _smoothSamples; s++, _smoothPosition++, _positionSamples++)
    output.Set(s, _smoother.Next(_lastOverall));

  int processed = s;
  if (s < FBFixedBlockSamples)
    _finished = true;
  for (; s < FBFixedBlockSamples; s++)
    output.Set(s, _smoother.State());

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return processed;

  auto& exchangeDSP = exchangeToGUI->voice[voice].env[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = _lengthSamples;
  exchangeDSP.positionSamples = _positionSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.env[state.moduleSlot];
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    exchangeParams.acc.stageLevel[i][voice] = stageLevel[i].Voice()[voice].Last();
    exchangeParams.acc.stageSlope[i][voice] = stageSlope[i].Voice()[voice].Last();
  }
  return processed;
}