#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_plug/modules/noise/FFNoiseProcessor.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

// 1/f^a noise https://sampo.kapsi.fi/PinkNoise/

FFNoiseProcessor::
FFNoiseProcessor() {}

void
FFNoiseProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.noise[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Noise];

  auto const& qNorm = params.block.q[0].Voice()[voice];
  auto const& onNorm = params.block.on[0].Voice()[voice];
  auto const& seedNorm = params.block.seed[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];

  _q = topo.NormalizedToDiscreteFast(FFNoiseParam::Q, qNorm);
  _on = topo.NormalizedToBoolFast(FFNoiseParam::On, onNorm);
  _seed = topo.NormalizedToDiscreteFast(FFNoiseParam::Seed, seedNorm);
  _uniCount = topo.NormalizedToDiscreteFast(FFNoiseParam::UniCount, uniCountNorm);

  _key = static_cast<float>(state.voice->event.note.key);
  _prng = FBParkMillerPRNG(static_cast<float>(_seed) / FFNoiseMaxSeed);
}

int
FFNoiseProcessor::Process(FBModuleProcState& state)
{
  return FBFixedBlockSamples;
}