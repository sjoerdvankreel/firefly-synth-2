#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

#include <cmath>

void
FFEnvProcessor::BeginVoice(FFModuleProcState const& state)
{
  _position = 0;
  _stage = FFEnvStage::Delay;
  int voice = state.voice->slot;
  auto const& topo = state.topo->modules[(int)FFModuleType::Env];
  auto const& params = state.proc->param.voice.env[state.moduleSlot];
  _voiceState.on = topo.params[(int)FFEnvParam::On].boolean.NormalizedToPlain(params.block.on[0].Voice()[voice]);
  _voiceState.type = (FFEnvType)topo.params[(int)FFEnvParam::Type].list.NormalizedToPlain(params.block.type[0].Voice()[voice]);
  _voiceState.holdSamples = (int)std::round(topo.params[(int)FFEnvParam::HoldTime].linear.NormalizedToPlain(params.block.holdTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.delaySamples = (int)std::round(topo.params[(int)FFEnvParam::DelayTime].linear.NormalizedToPlain(params.block.delayTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.decaySamples = (int)std::round(topo.params[(int)FFEnvParam::DecayTime].linear.NormalizedToPlain(params.block.decayTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.attackSamples = (int)std::round(topo.params[(int)FFEnvParam::AttackTime].linear.NormalizedToPlain(params.block.attackTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.releaseSamples = (int)std::round(topo.params[(int)FFEnvParam::ReleaseTime].linear.NormalizedToPlain(params.block.releaseTime[0].Voice()[voice]) * state.sampleRate);
}

void 
FFEnvProcessor::Process(FFModuleProcState const& state)
{
  FBFixedFloatArray scratch = {};
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {

  }
}