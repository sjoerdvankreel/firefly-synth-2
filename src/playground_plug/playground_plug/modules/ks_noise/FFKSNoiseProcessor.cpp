#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseTopo.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseProcessor.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

#include <bit>
#include <cstdint>

// https://www.reddit.com/r/DSP/comments/8fm3c5/what_am_i_doing_wrong_brown_noise/
// 1/f^a noise https://sampo.kapsi.fi/PinkNoise/
// kps https://dsp.stackexchange.com/questions/12596/synthesizing-harmonic-tones-with-karplus-strong

FFKSNoiseProcessor::
FFKSNoiseProcessor() {}

inline float
FFKSNoiseProcessor::Draw()
{
  if (_type == FFKSNoiseType::Uni)
    return FBToBipolar(_uniformPrng.NextScalar());
  assert(_type == FFKSNoiseType::Norm);
  float result = 0.0f;
  do
  {
    result = _normalPrng.NextScalar();
  } while (result < -3.0f || result > 3.0f);
  return result / 3.0f;
}

void
FFKSNoiseProcessor::AllocateBuffers(
  float sampleRate)
{
  float minHz = 20.0f;
  float fMaxPeriod = sampleRate / minHz;
  auto maxPeriod = static_cast<std::uint32_t>(std::ceil(fMaxPeriod));
  std::size_t nextPow2 = std::bit_ceil(maxPeriod);
  _waveTableBuffer.resize(std::max(_waveTableBuffer.size(), nextPow2));
  _waveTableSize = static_cast<int>(_waveTableBuffer.size());
}

inline float
FFKSNoiseProcessor::Next(
  FBStaticModule const& topo,
  float sampleRate, float baseFreq, 
  float colorNorm, float xNorm, float yNorm)
{
  float const empirical = 0.75f;
  float scale = 1.0f - colorNorm * empirical;
  float x = topo.NormalizedToIdentityFast(FFKSNoiseParam::X, xNorm);
  float y = 0.01f + 0.99f * topo.NormalizedToIdentityFast(FFKSNoiseParam::Y, yNorm);
  float color = 1.99f * topo.NormalizedToIdentityFast(FFKSNoiseParam::Color, colorNorm);

  _phaseTowardsX += baseFreq / sampleRate;
  if (_phaseTowardsX < 1.0f - x)
    return _lastDraw * scale;

  _phaseTowardsX = 0.0f;
  if (_uniformPrng.NextScalar() > y)
    return _lastDraw * scale;

  _lastDraw = Draw();
  float a = 1.0f;
  for (int i = 0; i < _poles; i++)
  {
    a = (i - color / 2.0f) * a / (i + 1.0f);
    int colorFilterPos = (_colorFilterPosition + _poles - i - 1) % _poles;
    _lastDraw -= a * _colorFilterBuffer.Get(colorFilterPos);
  }
  _colorFilterBuffer.Set(_colorFilterPosition, _lastDraw);
  _colorFilterPosition = (_colorFilterPosition + 1) % _poles;
  return _lastDraw * scale;
}

void
FFKSNoiseProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.ksNoise[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Noise];

  auto const& xNorm = params.acc.x[0].Voice()[voice];
  auto const& yNorm = params.acc.y[0].Voice()[voice];
  auto const& colorNorm = params.acc.color[0].Voice()[voice];
  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& seedNorm = params.block.seed[0].Voice()[voice];
  auto const& polesNorm = params.block.poles[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];

  // TODO -- should these be block params or do we do pitch shifting karplus-strong?
  auto const& fineNorm = params.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = params.acc.coarse[0].Voice()[voice];

  _seed = topo.NormalizedToDiscreteFast(FFKSNoiseParam::Seed, seedNorm);
  _poles = topo.NormalizedToDiscreteFast(FFKSNoiseParam::Poles, polesNorm);
  _type = topo.NormalizedToListFast<FFKSNoiseType>(FFKSNoiseParam::Type, typeNorm);
  FFOsciProcessorBase::BeginVoice(state, topo.NormalizedToDiscreteFast(FFKSNoiseParam::UniCount, uniCountNorm));

  _lastDraw = 0.0f;
  _graphPosition = 0;
  _phaseTowardsX = 0.0f;
  _colorFilterPosition = 0;
  
  _phaseGen = {};
  _normalPrng = FBMarsagliaPRNG(_seed / (FFKSNoiseMaxSeed + 1.0f));
  _uniformPrng = FBParkMillerPRNG(_seed / (FFKSNoiseMaxSeed + 1.0f));

  if (_type == FFKSNoiseType::Off)
    return;

  for (int p = 0; p < _poles; p++)
    _colorFilterBuffer.Set(p, Draw());

  float fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm.CV().Get(0));
  float coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm.CV().Get(0));
  float pitch = _key + coarse + fine;
  float baseFreq = FBPitchToFreq(pitch);
  for (int i = 0; i < _waveTableSize; i++)
    _waveTableBuffer[i] = Next(
      topo, sampleRate, baseFreq, 
      colorNorm.CV().Get(0), xNorm.CV().Get(0), yNorm.CV().Get(0));
}

int
FFKSNoiseProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.ksNoise[state.moduleSlot].output;

  output.Fill(0.0f);
  if (_type == FFKSNoiseType::Off)
    return 0;
  
  float sampleRate = state.input->sampleRate;
  auto const& procParams = procState->param.voice.ksNoise[state.moduleSlot];
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
    _gainPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::Gain, gainNorm, s));
    _uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::UniBlend, uniBlendNorm, s));
    _uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::UniSpread, uniSpreadNorm, s));
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    baseFreqPlain.Store(s, baseFreq);
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float incr = baseFreqPlain.Get(s) / sampleRate;
    float phase = _phaseGen.Next(incr);
    float waveTablePos = phase * _waveTableSize;
    int waveTablePos0 = static_cast<int>(waveTablePos);
    int waveTablePos1 = (waveTablePos0 + 1) % _waveTableSize;
    float fraction = waveTablePos - waveTablePos0;
    float val0 = _waveTableBuffer[waveTablePos0];
    float val1 = _waveTableBuffer[waveTablePos1];
    float val = (1.0f - fraction) * val0 + fraction * val1;

   // if(false)
//    _waveTableBuffer[_waveTablePosition] = Next(
  //    topo, sampleRate,
    //  baseFreqPlain.Get(s), colorNorm.CV().Get(s),
      //xNorm.CV().Get(s), yNorm.CV().Get(s));

    output[0].Set(s, val);
    output[1].Set(s, val);
    _graphPosition++;
  }

  ProcessGainSpreadBlend(output);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    int graphSamples = FBFreqToSamples(baseFreqPlain.Last(), sampleRate) * FFKSNoiseGraphRounds;
    return std::clamp(graphSamples - _graphPosition, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = exchangeToGUI->voice[voice].ksNoise[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreqPlain.Get(FBFixedBlockSamples - 1), state.input->sampleRate);

  auto& exchangeParams = exchangeToGUI->param.voice.ksNoise[state.moduleSlot];
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