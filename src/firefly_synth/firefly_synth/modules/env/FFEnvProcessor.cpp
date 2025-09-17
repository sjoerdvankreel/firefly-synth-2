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
FFEnvProcessor::BeginVoice( 
  FBModuleProcState& state, 
  FFEnvExchangeState const* exchangeFromDSP,
  bool graph)
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
  auto const& stageTimeNorm = params.voiceStart.stageTime;

  float syncNorm = params.block.sync[0].Voice()[voice];
  float typeNorm = params.block.type[0].Voice()[voice];
  float releaseNorm = params.block.release[0].Voice()[voice];
  float loopStartNorm = params.block.loopStart[0].Voice()[voice];
  float loopLengthNorm = params.block.loopLength[0].Voice()[voice];
  float smoothBarsNorm = params.block.smoothBars[0].Voice()[voice];
  float smoothTimeNorm = params.block.smoothTime[0].Voice()[voice];

  // Shorten attack/release stages if we're in a portamento section.
  // Note we can't apply release multiplier here but only during process.
  // Reason: can't look into the future - otherVoiceSubSectionTookOver flag
  // is only set when an other voice actually took over (i.e., we are not the
  // last subsection in a portamento section).
  _portaSectionAmpAttackNorm = 1.0f;
  _portaSectionAmpReleaseNorm = 1.0f;
  _thisVoiceIsSubSectionStart = false;
  _otherVoiceSubSectionTookOver = false;

  // Nullptr + graph means we're drawing primary.
  if (!graph)
  {
    _portaSectionAmpAttackNorm = procState->dsp.voice[voice].voiceModule.portaSectionAmpAttack;
    _portaSectionAmpReleaseNorm = procState->dsp.voice[voice].voiceModule.portaSectionAmpRelease;
    _thisVoiceIsSubSectionStart = procState->dsp.voice[voice].voiceModule.thisVoiceIsSubSectionStart;
    _otherVoiceSubSectionTookOver = procState->dsp.voice[voice].voiceModule.otherVoiceSubSectionTookOver;
  }
  else if (exchangeFromDSP != nullptr)
  {
    _portaSectionAmpAttackNorm = exchangeFromDSP->portaSectionAmpAttack;
    _portaSectionAmpReleaseNorm = exchangeFromDSP->portaSectionAmpRelease;
    _thisVoiceIsSubSectionStart = exchangeFromDSP->thisVoiceIsSubSectionStart;
    _otherVoiceSubSectionTookOver = exchangeFromDSP->otherVoiceSubSectionTookOver;
  }

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
    {
      float stageSamples = topo.NormalizedToBarsFloatSamplesFast(
        FFEnvParam::StageBars, stageBarsNorm[i].Voice()[voice],
        state.input->sampleRate, state.input->bpm);
      if (state.moduleSlot == FFAmpEnvSlot && i == 0 && _thisVoiceIsSubSectionStart)
        stageSamples *= _portaSectionAmpAttackNorm;
      _stageSamples[i] = static_cast<int>(std::round(stageSamples));
    }
  }
  else
  {
    _smoothSamples = topo.NormalizedToLinearTimeSamplesFast(
      FFEnvParam::SmoothTime, smoothTimeNorm, state.input->sampleRate);
    for (int i = 0; i < FFEnvStageCount; i++)
    {
      _voiceStartSnapshotNorm.stageTime[i] = stageTimeNorm[i].Voice()[voice].CV().Get(state.voice->offsetInBlock);
      float stageSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
        FFEnvParam::StageTime, _voiceStartSnapshotNorm.stageTime[i], state.input->sampleRate);
      if (state.moduleSlot == FFAmpEnvSlot && i == 0 && _thisVoiceIsSubSectionStart)
        stageSamples *= _portaSectionAmpAttackNorm;
      _stageSamples[i] = static_cast<int>(std::round(stageSamples));
    }
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
FFEnvProcessor::Process(
  FBModuleProcState& state, FFEnvExchangeState const* exchangeFromDSP, 
  bool graph, int releaseAt)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.env[state.moduleSlot];
  auto& output = procState->dsp.voice[voice].env[state.moduleSlot].output;
  auto const& stageLevel = procParams.acc.stageLevel;
  auto const& stageSlope = procParams.acc.stageSlope;
  
  _thisVoiceIsSubSectionStart = false;
  _otherVoiceSubSectionTookOver = false;
  if (!graph)
  {
    _thisVoiceIsSubSectionStart = procState->dsp.voice[voice].voiceModule.thisVoiceIsSubSectionStart;
    _otherVoiceSubSectionTookOver = procState->dsp.voice[voice].voiceModule.otherVoiceSubSectionTookOver;
  }
  else if (exchangeFromDSP != nullptr)
  {
    // ugly & debuggable
    if (exchangeFromDSP->thisVoiceIsSubSectionStart)
      _thisVoiceIsSubSectionStart = true;
    if (exchangeFromDSP->otherVoiceSubSectionTookOver)
      _otherVoiceSubSectionTookOver = true;
  }

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
  float const minSlope = 0.001f;
  float const slopeRange = 1.0f - 2.0f * minSlope;
  float const invLogHalf = 1.0f / std::log(0.5f);

  int loopEnd = _loopStart == 0 ? -1 : _loopStart - 1 + _loopLength;
  loopEnd = std::min(loopEnd, FFEnvStageCount);
  bool graphing = state.renderType != FBRenderType::Audio;
  bool graphingExchange = state.renderType == FBRenderType::GraphExchange || state.anyExchangeActive;
  bool forcedRelease = _releasePoint == 0 && _loopStart != 0 && state.moduleSlot == 0;
  int releasePoint = forcedRelease ? FFEnvStageCount : _releasePoint;

  if (releasePoint == 0 || graphingExchange)
    releaseAt = -1;

  // Deal with voice start offset.
  // In other words, the entire audio engine operates on
  // 16-sample blocks, and we fake starting a voice at
  // subblock position by delaying the voice amp envelope.
  if (state.moduleSlot == 0)
    for (; s < state.voice->offsetInBlock; s++)
      output.Set(s, 0.0f);

  int stage = 0;
  while (stage < FFEnvStageCount)
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

      if (_type == FFEnvType::Linear)
        _lastOverall = stageStart + (stageEnd - stageStart) * pos;
      else
      {
        float slope = minSlope + stageSlope[stage].Voice()[voice].CV().Get(s) * slopeRange;
        _lastOverall = stageStart + (stageEnd - stageStart) * std::pow(pos, std::log(slope) * invLogHalf);
      }

      // Dealing with portamento subsection release.
      // The idea is to log fade out everything after release section.
      // I thought about linear but that requires 2 control parameters:
      // where to start fade, and how long should it last.
      // Instead, log fade out gives (1, 0, 0, 0, ...) to (1, 1, 1, 1 ... 0) with linear in between.
      if (_otherVoiceSubSectionTookOver && state.moduleSlot == FFAmpEnvSlot && _released && _positionSamples >= _lengthSamplesUpToRelease)
      {
        int totalSamplesAfterRelease = _lengthSamples - _lengthSamplesUpToRelease;
        int currentSamplesAfterRelease = _positionSamples - _lengthSamplesUpToRelease;
        float positionPortaAmpRelease = std::clamp(currentSamplesAfterRelease / (float)totalSamplesAfterRelease, 0.0f, 1.0f);
        float portaAmpReleaseMultiplier = std::pow(positionPortaAmpRelease, std::log(0.001f + (_portaSectionAmpReleaseNorm * 0.999f)) * invLogHalf);
        _lastOverall *= portaAmpReleaseMultiplier;
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
  exchangeDSP.portaSectionAmpAttack = _portaSectionAmpAttackNorm;
  exchangeDSP.portaSectionAmpRelease = _portaSectionAmpReleaseNorm;
  exchangeDSP.thisVoiceIsSubSectionStart = _thisVoiceIsSubSectionStart;
  exchangeDSP.otherVoiceSubSectionTookOver = _otherVoiceSubSectionTookOver;

  auto& exchangeParams = exchangeToGUI->param.voice.env[state.moduleSlot];
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    exchangeParams.acc.stageLevel[i][voice] = stageLevel[i].Voice()[voice].Last();
    exchangeParams.acc.stageSlope[i][voice] = stageSlope[i].Voice()[voice].Last();
    exchangeParams.voiceStart.stageTime[i][voice] = _voiceStartSnapshotNorm.stageTime[i];
  }
  return processed;
}