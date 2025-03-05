#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>

#include <cmath>

void
FFEnvProcessor::BeginVoice(FBModuleProcState const& state)
{
  _finished = false;
  _released = false;
  _lastDAHDSR = 0.0f;
  _positionSamples = 0;
  _stagePositions.fill(0);
  _lastBeforeRelease = 0.0f;

  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  _voiceState.on = topo.params[(int)FFEnvParam::On].Boolean().NormalizedToPlainFast(params.block.on[0].Voice()[voice]);
  _voiceState.sync = topo.params[(int)FFEnvParam::Sync].Boolean().NormalizedToPlainFast(params.block.sync[0].Voice()[voice]);
  _voiceState.type = (FFEnvType)topo.params[(int)FFEnvParam::Type].List().NormalizedToPlainFast(params.block.type[0].Voice()[voice]);
  _voiceState.mode = (FFEnvMode)topo.params[(int)FFEnvParam::Mode].List().NormalizedToPlainFast(params.block.mode[0].Voice()[voice]);
  if (_voiceState.sync)
  {
    _voiceState.holdSamples = topo.params[(int)FFEnvParam::HoldBars].Bars().NormalizedBarsToSamplesFast(params.block.holdBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.decaySamples = topo.params[(int)FFEnvParam::DecayBars].Bars().NormalizedBarsToSamplesFast(params.block.decayBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.delaySamples = topo.params[(int)FFEnvParam::DelayBars].Bars().NormalizedBarsToSamplesFast(params.block.delayBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.attackSamples = topo.params[(int)FFEnvParam::AttackBars].Bars().NormalizedBarsToSamplesFast(params.block.attackBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.releaseSamples = topo.params[(int)FFEnvParam::ReleaseBars].Bars().NormalizedBarsToSamplesFast(params.block.releaseBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.smoothingSamples = topo.params[(int)FFEnvParam::SmoothBars].Bars().NormalizedBarsToSamplesFast(params.block.smoothBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
  } else
  {
    _voiceState.holdSamples = topo.params[(int)FFEnvParam::HoldTime].Linear().NormalizedTimeToSamplesFast(params.block.holdTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.decaySamples = topo.params[(int)FFEnvParam::DecayTime].Linear().NormalizedTimeToSamplesFast(params.block.decayTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.delaySamples = topo.params[(int)FFEnvParam::DelayTime].Linear().NormalizedTimeToSamplesFast(params.block.delayTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.attackSamples = topo.params[(int)FFEnvParam::AttackTime].Linear().NormalizedTimeToSamplesFast(params.block.attackTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.releaseSamples = topo.params[(int)FFEnvParam::ReleaseTime].Linear().NormalizedTimeToSamplesFast(params.block.releaseTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.smoothingSamples = topo.params[(int)FFEnvParam::SmoothTime].Linear().NormalizedTimeToSamplesFast(params.block.smoothTime[0].Voice()[voice], state.input->sampleRate);
  }
  _lengthSamplesUpToRelease = _voiceState.delaySamples + _voiceState.attackSamples + _voiceState.holdSamples + _voiceState.decaySamples;
  _lengthSamples = _lengthSamplesUpToRelease + _voiceState.releaseSamples + _voiceState.smoothingSamples;

  auto const& sustain = params.acc.sustainLevel[0].Voice()[voice].CV();
  if (_voiceState.delaySamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 0.0f;
  else if (_voiceState.attackSamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 0.0f;
  else if (_voiceState.holdSamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 1.0f;
  else if (_voiceState.decaySamples > 0)
    _lastBeforeRelease = _lastDAHDSR = 1.0f;
  else if (_voiceState.releaseSamples > 0)
    _lastBeforeRelease = _lastDAHDSR = sustain.data[0];

  _smoother.SetCoeffs(_voiceState.smoothingSamples);
  _smoother.State(_lastDAHDSR);
}

int 
FFEnvProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  auto const& procParams = procState->param.voice.env[state.moduleSlot];
  auto& output = procState->dsp.voice[voice].env[state.moduleSlot].output;
  auto const& decaySlope = procParams.acc.decaySlope[0].Voice()[voice];
  auto const& attackSlope = procParams.acc.attackSlope[0].Voice()[voice];
  auto const& releaseSlope = procParams.acc.releaseSlope[0].Voice()[voice];
  auto const& sustainLevel = procParams.acc.sustainLevel[0].Voice()[voice];

  if (!_voiceState.on)
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
  FBFixedFloatArray scratch = {};
  float const minSlope = 0.001f;
  float const slopeRange = 1.0f - 2.0f * minSlope;
  float const invLogHalf = 1.0f / std::log(0.5f);

  auto const& noteEvents = *state.input->note;
  auto const& myVoiceNote = state.input->voiceManager->Voices()[voice].event.note;
  if(_voiceState.type != FFEnvType::Follow && 
    state.renderType != FBRenderType::GraphExchange &&
    !state.anyExchangeActive)
    for (int i = 0; i < noteEvents.size(); i++)
      if (!noteEvents[i].on && noteEvents[i].note.Matches(myVoiceNote))
        releaseAt = noteEvents[i].pos;

  int& delayPos = _stagePositions[(int)FFEnvStage::Delay];
  for (; s < FBFixedBlockSamples && !_released && delayPos < _voiceState.delaySamples; s++, delayPos++, _positionSamples++)
  {
    _lastDAHDSR = 0.0f;
    _lastBeforeRelease = _lastDAHDSR;
    _released |= s == releaseAt;
    scratch.data[s] = _smoother.Next(_lastDAHDSR);
  }

  int& attackPos = _stagePositions[(int)FFEnvStage::Attack];
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && !_released && attackPos < _voiceState.attackSamples; s++, attackPos++, _positionSamples++)
    {
      _lastDAHDSR = attackPos / static_cast<float>(_voiceState.attackSamples);
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }
  else
    for (; s < FBFixedBlockSamples && !_released && attackPos < _voiceState.attackSamples; s++, attackPos++, _positionSamples++)
    {
      float slope = minSlope + attackSlope.CV().data[s] * slopeRange;
      float pos = attackPos / static_cast<float>(_voiceState.attackSamples);
      _lastDAHDSR = std::pow(pos, std::log(slope) * invLogHalf);
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }

  int& holdPos = _stagePositions[(int)FFEnvStage::Hold];
  for (; s < FBFixedBlockSamples && !_released && holdPos < _voiceState.holdSamples; s++, holdPos++, _positionSamples++)
  {
    _lastDAHDSR = 1.0f;
    _lastBeforeRelease = _lastDAHDSR;
    _released |= s == releaseAt;
    scratch.data[s] = _smoother.Next(_lastDAHDSR);
  }

  int& decayPos = _stagePositions[(int)FFEnvStage::Decay];
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && !_released && decayPos < _voiceState.decaySamples; s++, decayPos++, _positionSamples++)
    {
      float pos = decayPos / static_cast<float>(_voiceState.decaySamples);
      _lastDAHDSR = sustainLevel.CV().data[s] + (1.0f - sustainLevel.CV().data[s]) * (1.0f - pos);
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }
  else
    for (; s < FBFixedBlockSamples && !_released && decayPos < _voiceState.decaySamples; s++, decayPos++, _positionSamples++)
    {
      float slope = minSlope + decaySlope.CV().data[s] * slopeRange;
      float pos = decayPos / static_cast<float>(_voiceState.decaySamples);
      _lastDAHDSR = sustainLevel.CV().data[s] + (1.0f - sustainLevel.CV().data[s]) * (1.0f - std::pow(pos, std::log(slope) * invLogHalf));
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }

  if(_voiceState.type == FFEnvType::Sustain && 
    state.renderType != FBRenderType::GraphExchange && 
    !state.anyExchangeActive)
    for (; s < FBFixedBlockSamples && !_released; s++)
    {
      _lastDAHDSR = sustainLevel.CV().data[s];
      _lastBeforeRelease = _lastDAHDSR;
      _released |= s == releaseAt;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }

  if (s < FBFixedBlockSamples)
    _positionSamples = std::max(_positionSamples, _lengthSamplesUpToRelease);

  int& releasePos = _stagePositions[(int)FFEnvStage::Release];
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && releasePos < _voiceState.releaseSamples; s++, releasePos++, _positionSamples++)
    {
      float pos = releasePos / static_cast<float>(_voiceState.releaseSamples);
      _lastDAHDSR = _lastBeforeRelease * (1.0f - pos);
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }
  else
    for (; s < FBFixedBlockSamples && releasePos < _voiceState.releaseSamples; s++, releasePos++, _positionSamples++)
    {
      float slope = minSlope + releaseSlope.CV().data[s] * slopeRange;
      float pos = releasePos / static_cast<float>(_voiceState.releaseSamples);
      _lastDAHDSR = _lastBeforeRelease * (1.0f - std::pow(pos, std::log(slope) * invLogHalf));
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }

  int& smoothPos = _stagePositions[(int)FFEnvStage::Smooth];
  for (; s < FBFixedBlockSamples && smoothPos < _voiceState.smoothingSamples; s++, smoothPos++, _positionSamples++)
    scratch.data[s] = _smoother.Next(_lastDAHDSR);

  int processed = s;
  if (s < FBFixedBlockSamples)
    _finished = true;
  for (; s < FBFixedBlockSamples; s++)
    scratch.data[s] = _smoother.State();
  output.LoadFromFloatArray(scratch);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr || processed == 0)
    return processed;
  auto& exchangeDSP = exchangeToGUI->voice[voice].env[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lastOutput = output.Last();
  exchangeDSP.lengthSamples = _lengthSamples;
  exchangeDSP.positionSamples = _positionSamples;
  auto& exchangeParams = exchangeToGUI->param.voice.env[state.moduleSlot];
  exchangeParams.acc.decaySlope[0][voice] = decaySlope.CV().data[processed - 1];
  exchangeParams.acc.attackSlope[0][voice] = attackSlope.CV().data[processed - 1];
  exchangeParams.acc.releaseSlope[0][voice] = releaseSlope.CV().data[processed - 1];
  exchangeParams.acc.sustainLevel[0][voice] = sustainLevel.CV().data[processed - 1];
  return processed;
}