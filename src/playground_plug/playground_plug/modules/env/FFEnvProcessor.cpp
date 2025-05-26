#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <cmath>

void
FFEnvProcessor::BeginVoice(FBModuleProcState& state)
{
  _smoother = {};
  _finished = false;
  _released = false;
  _lastOverall = 0.0f;
  _positionSamples = 0;
  _stagePositions.fill(0);
  _lastBeforeRelease = 0.0f;

  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  _on = topo.NormalizedToBoolFast(FFEnvParam::On, params.block.on[0].Voice()[voice]);
  _exp = topo.NormalizedToBoolFast(FFEnvParam::Exp, params.block.exp[0].Voice()[voice]);
  _sync = topo.NormalizedToBoolFast(FFEnvParam::Sync, params.block.sync[0].Voice()[voice]);
  _sustain = topo.NormalizedToBoolFast(FFEnvParam::Sustain, params.block.sustain[0].Voice()[voice]);
  _releasePoint = topo.NormalizedToDiscreteFast(FFEnvParam::Release, params.block.release[0].Voice()[voice]);
  _loopStart = topo.NormalizedToDiscreteFast(FFEnvParam::LoopStart, params.block.loopStart[0].Voice()[voice]);
  _loopLength = topo.NormalizedToDiscreteFast(FFEnvParam::LoopStart, params.block.loopLength[0].Voice()[voice]);

  if (_sync)
  {
    _smoothSamples = topo.NormalizedToBarsSamplesFast(
      FFEnvParam::SmoothBars, params.block.smoothBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    for (int i = 0; i < FFEnvStageCount; i++)
      _stageSamples[i] = topo.NormalizedToBarsSamplesFast(
        FFEnvParam::StageBars, params.block.stageBars[i].Voice()[voice], state.input->sampleRate, state.input->bpm);
  }
  else
  {
    _smoothSamples = topo.NormalizedToLinearTimeSamplesFast(
      FFEnvParam::SmoothTime, params.block.smoothTime[0].Voice()[voice], state.input->sampleRate);
    for (int i = 0; i < FFEnvStageCount; i++)
      _stageSamples[i] = topo.NormalizedToLinearTimeSamplesFast(
        FFEnvParam::StageTime, params.block.stageTime[i].Voice()[voice], state.input->sampleRate);
  }

  _lengthSamples = 0;
  _lengthSamplesUpToRelease = 0;
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    _lengthSamples += _stageSamples[i];
    if (i < _releasePoint - 1)
      _lengthSamplesUpToRelease += _stageSamples[i]; // todo debug
  }
  _lengthSamples += _smoothSamples;

#if 0 // todo
  auto const& sustain = params.acc.sustainLevel[0].Voice()[voice].CV();
  if (_delaySamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 0.0f;
  else if (_attackSamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 0.0f;
  else if (_holdSamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 1.0f;
  else if (_decaySamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 1.0f;
  else if (_releaseSamples > 0)
    _lastBeforeRelease = _lastDAHDSR = sustain.Get(0);
#endif

  _smoother.SetCoeffs(_smoothSamples);
  _smoother.State(_lastOverall);
}

int 
FFEnvProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  auto const& procParams = procState->param.voice.env[state.moduleSlot];
  auto& output = procState->dsp.voice[voice].env[state.moduleSlot].output;
  auto const& stageLevel = procParams.acc.stageLevel;
  auto const& stageSlope = procParams.acc.stageSlope;

  if (!_on)
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
  
  for (int stage = 0; stage < FFEnvStageCount; stage++)
  {
    int& stagePos = _stagePositions[stage];
    int stageSamples = _stageSamples[stage];
    if (!_exp)
      for (; s < FBFixedBlockSamples && stagePos < stageSamples; s++, stagePos++, _positionSamples++)
      {
        float pos = stagePos / static_cast<float>(stageSamples);
        float stageEnd = stageLevel[stage].Voice()[voice].CV().Get(s);
        float stageStart = stage == 0 ? 0.0f : stageLevel[stage - 1].Voice()[voice].CV().Get(s);
        _lastOverall = stageStart + (stageEnd - stageStart) * pos;
        //_lastBeforeRelease = _lastDAHDSR;
        //_released |= s == releaseAt;
        output.Set(s, _smoother.Next(_lastOverall));
      }
    else
      for (; s < FBFixedBlockSamples /* && !_released */ && stagePos < stageSamples; s++, stagePos++, _positionSamples++)
      {
        float pos = stagePos / static_cast<float>(stageSamples);
        float stageEnd = stageLevel[stage].Voice()[voice].CV().Get(s);
        float stageStart = stage == 0 ? 0.0f : stageLevel[stage - 1].Voice()[voice].CV().Get(s);
        float slope = minSlope + stageSlope[stage].Voice()[voice].CV().Get(s) * slopeRange;
        _lastOverall = std::pow(pos, std::log(slope) * invLogHalf);
        //_lastBeforeRelease = _lastDAHDSR;
        //_released |= s == releaseAt;
        output.Set(s, _smoother.Next(_lastOverall));
      }
  }

#if 0 //todo
  auto const& noteEvents = *state.input->note;
  auto const& myVoiceNote = state.input->voiceManager->Voices()[voice].event.note;
  if(_type != FFEnvType::Follow && 
    state.renderType != FBRenderType::GraphExchange &&
    !state.anyExchangeActive)
    for (int i = 0; i < noteEvents.size(); i++)
      if (!noteEvents[i].on && noteEvents[i].note.Matches(myVoiceNote))
        releaseAt = noteEvents[i].pos;
#endif

#if 0

  int& delayPos = _stagePositions[(int)FFEnvStage::Delay];
  for (; s < FBFixedBlockSamples && !_released && delayPos < _delaySamples; s++, delayPos++, _positionSamples++)
  {
    _lastDAHDSR = 0.0f;
    _lastBeforeRelease = _lastDAHDSR;
    _released |= s == releaseAt;
    output.Set(s, _smoother.Next(_lastDAHDSR));
  }

  int& attackPos = _stagePositions[(int)FFEnvStage::Attack];
  if (_mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && !_released && attackPos < _attackSamples; s++, attackPos++, _positionSamples++)
    {
      _lastDAHDSR = attackPos / static_cast<float>(_attackSamples);
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      output.Set(s, _smoother.Next(_lastDAHDSR));
    }
  else
    for (; s < FBFixedBlockSamples && !_released && attackPos < _attackSamples; s++, attackPos++, _positionSamples++)
    {
      float slope = minSlope + attackSlope.CV().Get(s) * slopeRange;
      float pos = attackPos / static_cast<float>(_attackSamples);
      _lastDAHDSR = std::pow(pos, std::log(slope) * invLogHalf);
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      output.Set(s, _smoother.Next(_lastDAHDSR));
    }

  int& holdPos = _stagePositions[(int)FFEnvStage::Hold];
  for (; s < FBFixedBlockSamples && !_released && holdPos < _holdSamples; s++, holdPos++, _positionSamples++)
  {
    _lastDAHDSR = 1.0f;
    _lastBeforeRelease = _lastDAHDSR;
    _released |= s == releaseAt;
    output.Set(s, _smoother.Next(_lastDAHDSR));
  }

  int& decayPos = _stagePositions[(int)FFEnvStage::Decay];
  if (_mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && !_released && decayPos < _decaySamples; s++, decayPos++, _positionSamples++)
    {
      float pos = decayPos / static_cast<float>(_decaySamples);
      _lastDAHDSR = sustainLevel.CV().Get(s) + (1.0f - sustainLevel.CV().Get(s)) * (1.0f - pos);
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      output.Set(s, _smoother.Next(_lastDAHDSR));
    }
  else
    for (; s < FBFixedBlockSamples && !_released && decayPos < _decaySamples; s++, decayPos++, _positionSamples++)
    {
      float slope = minSlope + decaySlope.CV().Get(s) * slopeRange;
      float pos = decayPos / static_cast<float>(_decaySamples);
      _lastDAHDSR = sustainLevel.CV().Get(s) + (1.0f - sustainLevel.CV().Get(s)) * (1.0f - std::pow(pos, std::log(slope) * invLogHalf));
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      output.Set(s, _smoother.Next(_lastDAHDSR));
    }

  if(_type == FFEnvType::Sustain && 
    state.renderType != FBRenderType::GraphExchange && 
    !state.anyExchangeActive)
    for (; s < FBFixedBlockSamples && !_released; s++)
    {
      _lastDAHDSR = sustainLevel.CV().Get(s);
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      output.Set(s, _smoother.Next(_lastDAHDSR));
    }

  if (s < FBFixedBlockSamples)
    _positionSamples = std::max(_positionSamples, _lengthSamplesUpToRelease);

  int& releasePos = _stagePositions[(int)FFEnvStage::Release];
  if (_mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && releasePos < _releaseSamples; s++, releasePos++, _positionSamples++)
    {
      float pos = releasePos / static_cast<float>(_releaseSamples);
      _lastDAHDSR = _lastBeforeRelease * (1.0f - pos);
      output.Set(s, _smoother.Next(_lastDAHDSR));
    }
  else
    for (; s < FBFixedBlockSamples && releasePos < _releaseSamples; s++, releasePos++, _positionSamples++)
    {
      float slope = minSlope + releaseSlope.CV().Get(s) * slopeRange;
      float pos = releasePos / static_cast<float>(_releaseSamples);
      _lastDAHDSR = _lastBeforeRelease * (1.0f - std::pow(pos, std::log(slope) * invLogHalf));
      output.Set(s, _smoother.Next(_lastDAHDSR));
    }


  int& smoothPos = _smo[(int)FFEnvStage::Smooth];
  for (; s < FBFixedBlockSamples && smoothPos < _smoothingSamples; s++, smoothPos++, _positionSamples++)
    output.Set(s, _smoother.Next(_lastDAHDSR));

#endif

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