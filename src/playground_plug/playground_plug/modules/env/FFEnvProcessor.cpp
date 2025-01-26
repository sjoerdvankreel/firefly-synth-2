#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <cmath>

void 
FFEnvProcessor::Process(FFModuleProcState const& state, int voice)
{
  auto& output = state.proc->dsp.voice[voice].env[state.moduleSlot].output;
  auto const& topo = state.topo->modules[(int)FFModuleType::Env];
  auto const& params = state.proc->param.voice.env[state.moduleSlot];
  bool on = topo.params[(int)FFEnvParam::On].boolean.NormalizedToPlain(params.block.on[0].Voice()[voice]);
  int type = topo.params[(int)FFEnvParam::Type].list.NormalizedToPlain(params.block.type[0].Voice()[voice]);
  int holdTime = topo.params[(int)FFEnvParam::HoldTime].linear.NormalizedToPlain(params.block.holdTime[0].Voice()[voice]);
  float delayTime = topo.params[(int)FFEnvParam::DelayTime].linear.NormalizedToPlain(params.block.delayTime[0].Voice()[voice]);
  float decayTime = topo.params[(int)FFEnvParam::DecayTime].linear.NormalizedToPlain(params.block.decayTime[0].Voice()[voice]);
  float attackTime = topo.params[(int)FFEnvParam::AttackTime].linear.NormalizedToPlain(params.block.attackTime[0].Voice()[voice]);
  float releaseTime = topo.params[(int)FFEnvParam::ReleaseTime].linear.NormalizedToPlain(params.block.releaseTime[0].Voice()[voice]);
}