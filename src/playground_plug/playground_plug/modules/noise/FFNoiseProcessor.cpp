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

  auto const& onNorm = params.block.on[0].Voice()[voice];
  auto const& seedNorm = params.block.seed[0].Voice()[voice];
  auto const& polesNorm = params.block.poles[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];

  // TODO -- should these be block params or do we do pitch shifting karplus-strong?
  auto const& fineNorm = params.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = params.acc.coarse[0].Voice()[voice];

  _on = topo.NormalizedToBoolFast(FFNoiseParam::On, onNorm);
  _seed = topo.NormalizedToDiscreteFast(FFNoiseParam::Seed, seedNorm);
  _poles = topo.NormalizedToDiscreteFast(FFNoiseParam::Poles, polesNorm);
  FFOsciProcessorBase::BeginVoice(state, topo.NormalizedToDiscreteFast(FFNoiseParam::UniCount, uniCountNorm));

  _totalPosition = 0;
  _bufferPosition = 0;
  _prng = FBParkMillerPRNG(_seed / (FFNoiseMaxSeed + 1.0f));
  float fine = topo.NormalizedToLinearFast(FFNoiseParam::Fine, fineNorm.CV().First());
  float coarse = topo.NormalizedToLinearFast(FFNoiseParam::Coarse, coarseNorm.CV().First());
  _baseFreq = FBPitchToFreq(_key + coarse + fine);

  for (int p = 0; p < _poles; p++)
    for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      _x[p].Store(u, _prng.NextVector());
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

  auto const& xNorm = procParams.acc.x[0].Voice()[voice];
  auto const& yNorm = procParams.acc.y[0].Voice()[voice];
  auto const& colorNorm = procParams.acc.color[0].Voice()[voice];
  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.uniBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.uniDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.uniSpread[0].Voice()[voice];

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    _gainPlain.Store(s, topo.NormalizedToIdentityFast(FFNoiseParam::Gain, gainNorm, s));
    _uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFNoiseParam::UniBlend, uniBlendNorm, s));
    _uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFNoiseParam::UniSpread, uniSpreadNorm, s));
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float a = 1.0f;
    float colorPlain = topo.NormalizedToIdentityFast(FFNoiseParam::Color, colorNorm.CV().Get(s));
    float color = 2.0f * (1.0f - colorPlain);
    for (int i = 0; i < _poles; i++)
    {
      a = (i - color / 2.0f) * a / (i + 1.0f);
      _w.Set(i, a);
    }

    for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
    {
      int pos;
      auto val = FBToBipolar(_prng.NextVector());      
      for (int i = 0; i < _poles; i++)
      {
        pos = (_bufferPosition + _poles - i - 1) % _poles;
        assert(0 <= pos && pos < _poles);
        val -= _w.Get(i) * _x[pos].Load(u);
      }
      pos = (_bufferPosition + _poles - 1) % _poles;
      _x[pos].Store(u, val);

      FBSIMDArray<float, FBSIMDFloatCount> outputArray;
      outputArray.Store(0, val);
      for (int v = 0; v < FBSIMDFloatCount; v++)
        _uniOutput[u + v].Set(s, outputArray.Get(v));
    }

    _totalPosition++;
    _bufferPosition = (_bufferPosition + 1) % _poles;
  }

  ProcessGainSpreadBlend(output);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return std::clamp(1000 - _totalPosition, 0, FBFixedBlockSamples); // TODO

  auto& exchangeDSP = exchangeToGUI->voice[voice].noise[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(_baseFreq, state.input->sampleRate);
  exchangeDSP.positionSamples = _totalPosition % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.noise[state.moduleSlot];
  exchangeParams.acc.x[0][voice] = xNorm.Last();
  exchangeParams.acc.y[0][voice] = yNorm.Last();
  exchangeParams.acc.color[0][voice] = colorNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainNorm.Last();
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.uniBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.uniDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.uniSpread[0][voice] = uniSpreadNorm.Last();
  return FBFixedBlockSamples;
}