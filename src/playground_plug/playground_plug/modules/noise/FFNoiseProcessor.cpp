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

// https://www.reddit.com/r/DSP/comments/8fm3c5/what_am_i_doing_wrong_brown_noise/
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

  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& seedNorm = params.block.seed[0].Voice()[voice];
  auto const& polesNorm = params.block.poles[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];

  // TODO -- should these be block params or do we do pitch shifting karplus-strong?
  auto const& fineNorm = params.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = params.acc.coarse[0].Voice()[voice];

  _seed = topo.NormalizedToDiscreteFast(FFNoiseParam::Seed, seedNorm);
  _poles = topo.NormalizedToDiscreteFast(FFNoiseParam::Poles, polesNorm);
  _type = topo.NormalizedToListFast<FFNoiseType>(FFNoiseParam::Type, typeNorm);
  FFOsciProcessorBase::BeginVoice(state, topo.NormalizedToDiscreteFast(FFNoiseParam::UniCount, uniCountNorm));

  _graphPosition = 0;
  _historyPosition = 0;
  _lastDraw = 0.0f;
  _correctionMax = 0.0f;
  _correctionTotal = 0.0f;
  _correctionPosition = 0;
  _correctionBuffer.Fill(0.0f);

  _phaseGen = {};
  _phaseIncremented = 0.0f;
  _normalPrng = FBMarsagliaPRNG(_seed / (FFNoiseMaxSeed + 1.0f));
  _uniformPrng = FBParkMillerPRNG(_seed / (FFNoiseMaxSeed + 1.0f));

  for (int p = 0; p < _poles; p++)
    if(_type == FFNoiseType::Norm)
      _historyBuffer.Set(p, _normalPrng.NextScalar());
    else
      _historyBuffer.Set(p, _uniformPrng.NextScalar());
}

int
FFNoiseProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.noise[state.moduleSlot].output;

  output.Fill(0.0f);
  if (_type == FFNoiseType::Off)
    return 0;
  
  float sampleRate = state.input->sampleRate;
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

  FBSIMDArray<float, FBFixedBlockSamples> baseFreqPlain = {};
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm, s);
    auto coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, s);
    _gainPlain.Store(s, topo.NormalizedToIdentityFast(FFNoiseParam::Gain, gainNorm, s));
    _uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFNoiseParam::UniBlend, uniBlendNorm, s));
    _uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFNoiseParam::UniSpread, uniSpreadNorm, s));
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    baseFreqPlain.Store(s, baseFreq);
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float baseFreq = baseFreqPlain.Get(s);
    float period = FBFreqToSamples(baseFreq, sampleRate);
    float xPlain = topo.NormalizedToIdentityFast(FFNoiseParam::X, xNorm.CV().Get(s));
    float yPlain = topo.NormalizedToIdentityFast(FFNoiseParam::Y, yNorm.CV().Get(s));

    _phaseIncremented += baseFreq / sampleRate;
    //if(_phaseIncremented >= xPlain)
    //{
      _phaseIncremented = 0.0f;
      if (_type == FFNoiseType::Norm)
        _lastDraw = _normalPrng.NextScalar();
      else
        _lastDraw = _uniformPrng.NextScalar();
      _lastDraw = FBToBipolar(_lastDraw);
    //}
    //}
    assert(!std::isnan(_lastDraw));

    float a = 1.0f;
    float color = 1.99f * topo.NormalizedToIdentityFast(FFNoiseParam::Color, colorNorm.CV().Get(s));
    for (int i = 0; i < _poles; i++)
    {
      a = (i - color / 2.0f) * a / (i + 1.0f);
      int historyPos = (_historyPosition + _poles - i - 1) % _poles;
      assert(0 <= historyPos && historyPos < _poles);
      _lastDraw -= a * _historyBuffer.Get(historyPos);
      assert(!std::isnan(_lastDraw));
    }

    _correctionTotal += _lastDraw;
    assert(!std::isnan(_correctionTotal));
    _correctionTotal -= _correctionBuffer.Get(_correctionPosition);
    assert(!std::isnan(_correctionTotal));
    _correctionBuffer.Set(_correctionPosition, _lastDraw);
    _correctionPosition = (_correctionPosition + 1) % FFNoiseCorrectionBufferSize;
    _correctionMax = std::max(std::abs(_lastDraw), _correctionMax);

    _lastDraw -= _correctionTotal / FFNoiseCorrectionBufferSize;
    assert(!std::isnan(_lastDraw));
    _historyBuffer.Set(_historyPosition, _lastDraw);
    _historyPosition = (_historyPosition + 1) % _poles;

    float outVal = _lastDraw / _correctionMax;
    assert(!std::isnan(outVal));
    output[0].Set(s, outVal);
    output[1].Set(s, outVal);
    _graphPosition++;
  }

#if 0
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
#endif


  ProcessGainSpreadBlend(output);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    float graphSamples = FBFreqToSamples(baseFreqPlain.Last(), sampleRate) * FFNoiseGraphRounds;
    return std::clamp(static_cast<int>(graphSamples) - _graphPosition, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = exchangeToGUI->voice[voice].noise[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreqPlain.Get(FBFixedBlockSamples - 1), state.input->sampleRate);

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