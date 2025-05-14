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
// kps https://dsp.stackexchange.com/questions/12596/synthesizing-harmonic-tones-with-karplus-strong

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

  // TODO -- should these be block params or do we do pitch shifting karplus-strong?
  auto const& fineNorm = params.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = params.acc.coarse[0].Voice()[voice];

  _q = topo.NormalizedToDiscreteFast(FFNoiseParam::Q, qNorm);
  _on = topo.NormalizedToBoolFast(FFNoiseParam::On, onNorm);
  _seed = topo.NormalizedToDiscreteFast(FFNoiseParam::Seed, seedNorm);
  _uniCount = topo.NormalizedToDiscreteFast(FFNoiseParam::UniCount, uniCountNorm);

  _position = 0;
  _key = static_cast<float>(state.voice->event.note.key);
  _prng = FBParkMillerPRNG(_seed / (FFNoiseMaxSeed + 1.0f));
  float fine = topo.NormalizedToLinearFast(FFNoiseParam::Fine, fineNorm.CV().First());
  float coarse = topo.NormalizedToLinearFast(FFNoiseParam::Coarse, coarseNorm.CV().First());
  _baseFreq = FBPitchToFreq(_key + coarse + fine);

  for (int q = 0; q < _q; q++)
    for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      _x[q].Store(u, _prng.NextVector());
}

int
FFNoiseProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.noise[state.moduleSlot].output;

  output.Fill(0.0f);
  if (!_on)
    return 0;

  auto const& procParams = procState->param.voice.noise[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Noise];

  auto const& aNorm = procParams.acc.a[0].Voice()[voice];
  auto const& xNorm = procParams.acc.x[0].Voice()[voice];
  auto const& yNorm = procParams.acc.y[0].Voice()[voice];
  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.uniBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.uniDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.uniSpread[0].Voice()[voice];

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float v = FBToBipolar(_prng.NextScalar());
    assert(-1.0f <= v && v <= 1.0f);
    output[0].Set(s, v);
    output[1].Set(s, v);
    _position++;
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return std::clamp(1000 - _position, 0, FBFixedBlockSamples); // TODO

  auto& exchangeDSP = exchangeToGUI->voice[voice].noise[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(_baseFreq, state.input->sampleRate);
  exchangeDSP.positionSamples = _position % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.noise[state.moduleSlot];
  exchangeParams.acc.a[0][voice] = aNorm.Last();
  exchangeParams.acc.x[0][voice] = xNorm.Last();
  exchangeParams.acc.y[0][voice] = yNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainNorm.Last();
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.uniBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.uniDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.uniSpread[0][voice] = uniSpreadNorm.Last();
  return FBFixedBlockSamples;
}