#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

void 
FFGLFOProcessor::Reset(FBModuleProcState& state)
{
  _phase = {};
}

int
FFGLFOProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gLFO[state.moduleSlot].output;
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GLFO];
  auto const& procParams = procState->param.global.gLFO[state.moduleSlot];
  
  if (!topo.NormalizedToBoolFast(FFGLFOParam::On, procParams.block.on[0].Value()))
  {
    output.Fill(0.0f);
    return 0;
  }

  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();

  auto const& rateNorm = procParams.acc.rate[0].Global();
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float ratePlain = topo.NormalizedToLinearFast(FFGLFOParam::Rate, rateNorm.CV()[s]);
    float p = _phase.Next(ratePlain / state.input->sampleRate);
    output[s] = FBToUnipolar(std::sin(p * FBTwoPi));
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeParams = exchangeToGUI->param.global.gLFO[state.moduleSlot];
  exchangeParams.acc.rate[0] = rateNorm.Last();

  auto& exchangeDSP = exchangeToGUI->global.gLFO[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lastOutput = output.Last();
  exchangeDSP.lengthSamples = topo.NormalizedToLinearFreqSamplesFast(FFGLFOParam::Rate, rateNorm.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;
  return FBFixedBlockSamples;
}