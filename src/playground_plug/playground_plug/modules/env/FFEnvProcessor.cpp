#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

#include <cmath>

void
FFEnvProcessor::Reset()
{
  _position = 0;
  _stage = FFEnvStage::Delay;
}

void 
FFEnvProcessor::Process(FFModuleProcState const& state)
{
  int voice = state.voice->slot;
  auto const& topo = state.topo->modules[(int)FFModuleType::Env];
  auto const& params = state.proc->param.voice.env[state.moduleSlot];
  bool on = topo.params[(int)FFEnvParam::On].boolean.NormalizedToPlain(params.block.on[0].Voice()[voice]);
  int type = topo.params[(int)FFEnvParam::Type].list.NormalizedToPlain(params.block.type[0].Voice()[voice]);
  int holdSamples = (int)std::round(topo.params[(int)FFEnvParam::HoldTime].linear.NormalizedToPlain(params.block.holdTime[0].Voice()[voice]) * state.sampleRate);
  int delaySamples = (int)std::round(topo.params[(int)FFEnvParam::DelayTime].linear.NormalizedToPlain(params.block.delayTime[0].Voice()[voice]) * state.sampleRate);
  int decaySamples = (int)std::round(topo.params[(int)FFEnvParam::DecayTime].linear.NormalizedToPlain(params.block.decayTime[0].Voice()[voice]) * state.sampleRate);
  int attackSamples = (int)std::round(topo.params[(int)FFEnvParam::AttackTime].linear.NormalizedToPlain(params.block.attackTime[0].Voice()[voice]) * state.sampleRate);
  int releaseSamples = (int)std::round(topo.params[(int)FFEnvParam::ReleaseTime].linear.NormalizedToPlain(params.block.releaseTime[0].Voice()[voice]) * state.sampleRate);

  FBFixedFloatArray scratch = {};
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {

  }
}