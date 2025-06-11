#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/glfo/FFGLFOTopo.hpp>
#include <firefly_synth/modules/glfo/FFGLFOProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/main/FBGraphRenderState.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

void 
FFGLFOProcessor::BeginBlock(bool graph, FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GLFO];
  auto const& procParams = procState->param.global.gLFO[state.moduleSlot];  
  _on = topo.NormalizedToBoolFast(FFGLFOParam::On, procParams.block.on[0].Value());
  if(graph)
    _phase = {};
}

int
FFGLFOProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gLFO[state.moduleSlot].output;
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GLFO];
  auto const& procParams = procState->param.global.gLFO[state.moduleSlot];
  auto const& rateNorm = procParams.acc.rate[0].Global();

  if (!_on)
  {
    output.Fill(0.0f);
    return 0;
  }

  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto ratePlain = topo.NormalizedToLinearFast(FFGLFOParam::Rate, rateNorm, s);
    auto phase = _phase.NextBatch(ratePlain / state.input->sampleRate);
    output.Store(s, FBToUnipolar(xsimd::sin(phase * FBTwoPi)));
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeParams = exchangeToGUI->param.global.gLFO[state.moduleSlot];
  exchangeParams.acc.rate[0] = rateNorm.Last();

  auto& exchangeDSP = exchangeToGUI->global.gLFO[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = topo.NormalizedToLinearFreqSamplesFast(FFGLFOParam::Rate, rateNorm.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;
  return FBFixedBlockSamples;
}