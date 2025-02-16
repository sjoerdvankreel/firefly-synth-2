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

int 
FFEnvProcessor::PlotLengthSamples(
  FBStaticTopo const& topo, FFScalarState const& state, 
  int moduleSlot, float sampleRate, float bpm) const
{
  int result = 0;
  auto const& envState = state.param.voice.env[moduleSlot];
  auto const& envTopo = topo.modules[(int)FFModuleType::Env];
  bool sync = envTopo.audioParams[(int)FFEnvParam::Sync].Boolean().NormalizedToPlain(envState.block.sync[0]);
  if (sync)
  {
    result += envTopo.audioParams[(int)FFEnvParam::HoldBars].Bars().NormalizedBarsToSamples(envState.block.holdBars[0], sampleRate, bpm);
    result += envTopo.audioParams[(int)FFEnvParam::DecayBars].Bars().NormalizedBarsToSamples(envState.block.decayBars[0], sampleRate, bpm);
    result += envTopo.audioParams[(int)FFEnvParam::DelayBars].Bars().NormalizedBarsToSamples(envState.block.delayBars[0], sampleRate, bpm);
    result += envTopo.audioParams[(int)FFEnvParam::AttackBars].Bars().NormalizedBarsToSamples(envState.block.attackBars[0], sampleRate, bpm);
    result += envTopo.audioParams[(int)FFEnvParam::SmoothBars].Bars().NormalizedBarsToSamples(envState.block.smoothBars[0], sampleRate, bpm);
    result += envTopo.audioParams[(int)FFEnvParam::ReleaseBars].Bars().NormalizedBarsToSamples(envState.block.releaseBars[0], sampleRate, bpm);
  }
  else
  {
    result += envTopo.audioParams[(int)FFEnvParam::HoldTime].Linear().NormalizedTimeToSamples(envState.block.holdTime[0], sampleRate);
    result += envTopo.audioParams[(int)FFEnvParam::DecayTime].Linear().NormalizedTimeToSamples(envState.block.decayTime[0], sampleRate);
    result += envTopo.audioParams[(int)FFEnvParam::DelayTime].Linear().NormalizedTimeToSamples(envState.block.delayTime[0], sampleRate);
    result += envTopo.audioParams[(int)FFEnvParam::AttackTime].Linear().NormalizedTimeToSamples(envState.block.attackTime[0], sampleRate);
    result += envTopo.audioParams[(int)FFEnvParam::SmoothTime].Linear().NormalizedTimeToSamples(envState.block.smoothTime[0], sampleRate);
    result += envTopo.audioParams[(int)FFEnvParam::ReleaseTime].Linear().NormalizedTimeToSamples(envState.block.releaseTime[0], sampleRate);
  }
  return result;
}

void
FFEnvProcessor::BeginVoice(FBModuleProcState const& state)
{
  _finished = false;
  _lastDAHDSR = 0.0f;
  _positionSamples = 0;
  _stagePositions.fill(0);
  _lastBeforeRelease = 0.0f;

  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  _voiceState.on = topo.audioParams[(int)FFEnvParam::On].Boolean().NormalizedToPlain(params.block.on[0].Voice()[voice]);
  _voiceState.sync = topo.audioParams[(int)FFEnvParam::Sync].Boolean().NormalizedToPlain(params.block.sync[0].Voice()[voice]);
  _voiceState.type = (FFEnvType)topo.audioParams[(int)FFEnvParam::Type].List().NormalizedToPlain(params.block.type[0].Voice()[voice]);
  _voiceState.mode = (FFEnvMode)topo.audioParams[(int)FFEnvParam::Mode].List().NormalizedToPlain(params.block.mode[0].Voice()[voice]);
  if (_voiceState.sync)
  {
    _voiceState.holdSamples = topo.audioParams[(int)FFEnvParam::HoldBars].Bars().NormalizedBarsToSamples(params.block.holdBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.decaySamples = topo.audioParams[(int)FFEnvParam::DecayBars].Bars().NormalizedBarsToSamples(params.block.decayBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.delaySamples = topo.audioParams[(int)FFEnvParam::DelayBars].Bars().NormalizedBarsToSamples(params.block.delayBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.attackSamples = topo.audioParams[(int)FFEnvParam::AttackBars].Bars().NormalizedBarsToSamples(params.block.attackBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.releaseSamples = topo.audioParams[(int)FFEnvParam::ReleaseBars].Bars().NormalizedBarsToSamples(params.block.releaseBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
    _voiceState.smoothingSamples = topo.audioParams[(int)FFEnvParam::SmoothBars].Bars().NormalizedBarsToSamples(params.block.smoothBars[0].Voice()[voice], state.input->sampleRate, state.input->bpm);
  } else
  {
    _voiceState.holdSamples = topo.audioParams[(int)FFEnvParam::HoldTime].Linear().NormalizedTimeToSamples(params.block.holdTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.decaySamples = topo.audioParams[(int)FFEnvParam::DecayTime].Linear().NormalizedTimeToSamples(params.block.decayTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.delaySamples = topo.audioParams[(int)FFEnvParam::DelayTime].Linear().NormalizedTimeToSamples(params.block.delayTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.attackSamples = topo.audioParams[(int)FFEnvParam::AttackTime].Linear().NormalizedTimeToSamples(params.block.attackTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.releaseSamples = topo.audioParams[(int)FFEnvParam::ReleaseTime].Linear().NormalizedTimeToSamples(params.block.releaseTime[0].Voice()[voice], state.input->sampleRate);
    _voiceState.smoothingSamples = topo.audioParams[(int)FFEnvParam::SmoothTime].Linear().NormalizedTimeToSamples(params.block.smoothTime[0].Voice()[voice], state.input->sampleRate);
  }
  _lengthSamples = _voiceState.delaySamples + _voiceState.attackSamples + _voiceState.holdSamples + _voiceState.decaySamples + _voiceState.releaseSamples + _voiceState.smoothingSamples;

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
  for (int i = 0; i < noteEvents.size(); i++)
    if (!noteEvents[i].on && noteEvents[i].note.Matches(myVoiceNote))
      releaseAt = noteEvents[i].pos;

  int& delayPos = _stagePositions[(int)FFEnvStage::Delay];
  for (; s < FBFixedBlockSamples && delayPos < _voiceState.delaySamples; s++, delayPos++)
  {
    _lastDAHDSR = 0.0f;
    _lastBeforeRelease = _lastDAHDSR;
    scratch.data[s] = _smoother.Next(_lastDAHDSR);
  }

  int& attackPos = _stagePositions[(int)FFEnvStage::Attack];
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && attackPos < _voiceState.attackSamples; s++, attackPos++)
    {
      _lastDAHDSR = attackPos / (float)_voiceState.attackSamples;
      _lastBeforeRelease = _lastDAHDSR;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }
  else
    for (; s < FBFixedBlockSamples && attackPos < _voiceState.attackSamples; s++, attackPos++)
    {
      float slope = minSlope + attackSlope.CV().data[s] * slopeRange;
      float pos = attackPos / (float)_voiceState.attackSamples;
      _lastDAHDSR = std::pow(pos, std::log(slope) * invLogHalf);
      _lastBeforeRelease = _lastDAHDSR;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }

  int& holdPos = _stagePositions[(int)FFEnvStage::Hold];
  for (; s < FBFixedBlockSamples && holdPos < _voiceState.holdSamples; s++, holdPos++)
  {
    _lastDAHDSR = 1.0f;
    _lastBeforeRelease = _lastDAHDSR;
    scratch.data[s] = _smoother.Next(_lastDAHDSR);
  }

  int& decayPos = _stagePositions[(int)FFEnvStage::Decay];
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && decayPos < _voiceState.decaySamples; s++, decayPos++)
    {
      float pos = decayPos / (float)_voiceState.decaySamples;
      _lastDAHDSR = sustainLevel.CV().data[s] + (1.0f - sustainLevel.CV().data[s]) * (1.0f - pos);
      _lastBeforeRelease = _lastDAHDSR;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }
  else
    for (; s < FBFixedBlockSamples && decayPos < _voiceState.decaySamples; s++, decayPos++)
    {
      float slope = minSlope + decaySlope.CV().data[s] * slopeRange;
      float pos = decayPos / (float)_voiceState.decaySamples;
      _lastDAHDSR = sustainLevel.CV().data[s] + (1.0f - sustainLevel.CV().data[s]) * (1.0f - std::pow(pos, std::log(slope) * invLogHalf));
      _lastBeforeRelease = _lastDAHDSR;
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }

  int& releasePos = _stagePositions[(int)FFEnvStage::Release];
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && releasePos < _voiceState.releaseSamples; s++, releasePos++)
    {
      float pos = releasePos / (float)_voiceState.releaseSamples;
      _lastDAHDSR = _lastBeforeRelease * (1.0f - pos);
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }
  else
    for (; s < FBFixedBlockSamples && releasePos < _voiceState.releaseSamples; s++, releasePos++)
    {
      float slope = minSlope + releaseSlope.CV().data[s] * slopeRange;
      float pos = releasePos / (float)_voiceState.releaseSamples;
      _lastDAHDSR = _lastBeforeRelease * (1.0f - std::pow(pos, std::log(slope) * invLogHalf));
      scratch.data[s] = _smoother.Next(_lastDAHDSR);
    }

  int& smoothPos = _stagePositions[(int)FFEnvStage::Smooth];
  for (; s < FBFixedBlockSamples && smoothPos < _voiceState.smoothingSamples; s++, smoothPos++)
    scratch.data[s] = _smoother.Next(_lastDAHDSR);

  int processed = s;
  _positionSamples += processed;
  if (s < FBFixedBlockSamples)
    _finished = true;
  for (; s < FBFixedBlockSamples; s++)
    scratch.data[s] = _smoother.State();
  output.LoadFromFloatArray(scratch);

  auto* exchangeState = state.ExchangeAs<FFExchangeState>();
  if (exchangeState == nullptr || processed == 0)
    return processed;
  auto& exchangeDSP = exchangeState->voice[voice].env[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = _lengthSamples;
  exchangeDSP.positionSamples = _positionSamples;
  auto& exchangeParams = exchangeState->param.voice.env[state.moduleSlot];
  exchangeParams.acc.decaySlope[0][voice] = decaySlope.CV().data[processed - 1];
  exchangeParams.acc.attackSlope[0][voice] = attackSlope.CV().data[processed - 1];
  exchangeParams.acc.releaseSlope[0][voice] = releaseSlope.CV().data[processed - 1];
  exchangeParams.acc.sustainLevel[0][voice] = sustainLevel.CV().data[processed - 1];
  return processed;
}