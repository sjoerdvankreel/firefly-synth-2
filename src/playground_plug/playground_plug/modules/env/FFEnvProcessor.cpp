#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

#include <cmath>

void
FFEnvProcessor::BeginVoice(FBModuleProcState const& state)
{
  _finished = false;
  _stagePositions.fill(0);
  int voice = state.voice->slot;
  auto* procState = state.ProcState<FFProcState>();
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  _voiceState.holdSamples = (int)std::round(topo.params[(int)FFEnvParam::HoldTime].linear.NormalizedToPlain(
    params.block.holdTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.decaySamples = (int)std::round(topo.params[(int)FFEnvParam::DecayTime].linear.NormalizedToPlain(
    params.block.decayTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.delaySamples = (int)std::round(topo.params[(int)FFEnvParam::DelayTime].linear.NormalizedToPlain(
    params.block.delayTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.attackSamples = (int)std::round(topo.params[(int)FFEnvParam::AttackTime].linear.NormalizedToPlain(
    params.block.attackTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.releaseSamples = (int)std::round(topo.params[(int)FFEnvParam::ReleaseTime].linear.NormalizedToPlain(
    params.block.releaseTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.on = topo.params[(int)FFEnvParam::On].boolean.NormalizedToPlain(params.block.on[0].Voice()[voice]);
  _voiceState.type = (FFEnvType)topo.params[(int)FFEnvParam::Type].list.NormalizedToPlain(params.block.type[0].Voice()[voice]);
  _voiceState.mode = (FFEnvMode)topo.params[(int)FFEnvParam::Mode].list.NormalizedToPlain(params.block.mode[0].Voice()[voice]);
  float smoothDurationSec = topo.params[(int)FFEnvParam::Smooth].linear.NormalizedToPlain(params.block.smooth[0].Voice()[voice]);
  _voiceState.smoother.SetCoeffs(state.sampleRate, smoothDurationSec);
}

int 
FFEnvProcessor::Process(FBModuleProcState const& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcState<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto& output = procState->dsp.voice[voice].env[state.moduleSlot].output;

  if (!_voiceState.on || _finished)
  {
    output.Clear();
    return 0;
  }

  int s = 0;
  FBFixedFloatArray scratch = {};
  float const minSlope = 0.001f;
  float const slopeRange = 1.0f - 2.0f * minSlope;
  float const invLogHalf = 1.0f / std::log(0.5f);
  auto const& sustain = params.acc.sustain[0].Voice()[voice].CV();

  for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Delay] < _voiceState.delaySamples; 
    s++, _stagePositions[(int)FFEnvStage::Delay]++)
    scratch.data[s] = _voiceState.smoother.Next(0.0f);

  auto const& attackSlope = params.acc.attackSlope[0].Voice()[voice].CV();
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Attack] < _voiceState.attackSamples;
      s++, _stagePositions[(int)FFEnvStage::Attack]++)
        scratch.data[s] = _voiceState.smoother.Next(
          _stagePositions[(int)FFEnvStage::Attack] / (float)_voiceState.attackSamples);
  else
    for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Attack] < _voiceState.attackSamples;
      s++, _stagePositions[(int)FFEnvStage::Attack]++)
    {
      float slope = minSlope + attackSlope.data[s] * slopeRange;
      float pos = _stagePositions[(int)FFEnvStage::Attack] / (float)_voiceState.attackSamples;
      scratch.data[s] = _voiceState.smoother.Next(
        std::pow(pos, std::log(slope) * invLogHalf));
    }

  for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Hold] < _voiceState.holdSamples; 
    s++, _stagePositions[(int)FFEnvStage::Hold]++)
    scratch.data[s] = _voiceState.smoother.Next(1.0f);

  auto const& decaySlope = params.acc.decaySlope[0].Voice()[voice].CV();
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Decay] < _voiceState.decaySamples;
      s++, _stagePositions[(int)FFEnvStage::Decay]++)
    {
      float pos = _stagePositions[(int)FFEnvStage::Decay] / (float)_voiceState.decaySamples;
      scratch.data[s] = _voiceState.smoother.Next(
        sustain.data[s] + (1.0f - sustain.data[s]) * (1.0f - pos));
    }
  else
    for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Decay] < _voiceState.decaySamples;
      s++, _stagePositions[(int)FFEnvStage::Decay]++)
    {
      float slope = minSlope + decaySlope.data[s] * slopeRange;
      float pos = _stagePositions[(int)FFEnvStage::Decay] / (float)_voiceState.decaySamples;
      scratch.data[s] = _voiceState.smoother.Next(
        sustain.data[s] + (1.0f - sustain.data[s]) * (1.0f - std::pow(pos, std::log(slope) * invLogHalf)));
    }

  auto const& releaseSlope = params.acc.releaseSlope[0].Voice()[voice].CV();
  if (_voiceState.mode == FFEnvMode::Linear)
    for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Release] < _voiceState.releaseSamples;
      s++, _stagePositions[(int)FFEnvStage::Release]++)
    {
      float pos = _stagePositions[(int)FFEnvStage::Release] / (float)_voiceState.releaseSamples;
      scratch.data[s] = _voiceState.smoother.Next(sustain.data[s] * (1.0f - pos));
    }
  else
    for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Release] < _voiceState.releaseSamples;
      s++, _stagePositions[(int)FFEnvStage::Release]++)
    {
      float slope = minSlope + releaseSlope.data[s] * slopeRange;
      float pos = _stagePositions[(int)FFEnvStage::Release] / (float)_voiceState.releaseSamples;
      scratch.data[s] = _voiceState.smoother.Next(
        sustain.data[s] * (1.0f - std::pow(pos, std::log(slope) * invLogHalf)));
    }

  int processed = s;
  if (s < FBFixedBlockSamples)
    _finished = true;
  for (; s < FBFixedBlockSamples; s++)
    scratch.data[s] = 0.0f;
  output.LoadFromFloatArray(scratch);
  return processed;
}