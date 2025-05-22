#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseTopo.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoiseProcessor.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

#include <bit>
#include <cstdint>

inline float const DCBlockFreq = 20.0f;
inline int constexpr DelayLineSize = 8192;

// https://www.reddit.com/r/DSP/comments/8fm3c5/what_am_i_doing_wrong_brown_noise/
// 1/f^a noise https://sampo.kapsi.fi/PinkNoise/
// kps https://dsp.stackexchange.com/questions/12596/synthesizing-harmonic-tones-with-karplus-strong

FFKSNoiseProcessor::
FFKSNoiseProcessor() {}

void
FFKSNoiseProcessor::Initialize(float sampleRate)
{
  for (int i = 0; i < FFOsciBaseUniMaxCount; i++)
  {
    if (_uniState[i].delayLine.Count() == 0)
      _uniState[i].delayLine.Resize(DelayLineSize);
    _uniState[i].dcFilter.SetCoeffs(DCBlockFreq, sampleRate);
  }
}

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

inline float
FFKSNoiseProcessor::Next(
  FBStaticModule const& topo, int uniVoice,
  float sampleRate, float uniFreq, 
  float excite, float colorPlain, 
  float xPlain, float yPlain,
  float lpFreqHz, float hpFreqHz)
{
  float const empirical1 = 0.75f;
  float const empirical2 = 4.0f;
  float x = xPlain;
  float y = 0.01f + 0.99f * yPlain;
  float color = 1.99f * (1.0f - colorPlain);
  float scale = 1.0f - ((1.0f - colorPlain) * empirical1);
  scale *= (1.0f + empirical2 * (1.0f - excite));

  _uniState[uniVoice].lpFilter.Set(FBCytomicFilterMode::LPF, sampleRate, lpFreqHz, 0.0f, 0.0f);
  _uniState[uniVoice].hpFilter.Set(FBCytomicFilterMode::HPF, sampleRate, hpFreqHz, 0.0f, 0.0f);

  _uniState[uniVoice].phaseTowardsX += uniFreq / sampleRate;
  if (_uniState[uniVoice].phaseTowardsX < 1.0f - x)
    return static_cast<float>(
      _uniState[uniVoice].lpFilter.Next(0, 
        _uniState[uniVoice].hpFilter.Next(0, 
          _uniState[uniVoice].lastDraw * scale)));

  _uniState[uniVoice].phaseTowardsX = 0.0f;
  if (_uniformPrng.NextScalar() > y) 
    return static_cast<float>(
      _uniState[uniVoice].lpFilter.Next(0,
        _uniState[uniVoice].hpFilter.Next(0,
          _uniState[uniVoice].lastDraw * scale)));

  _uniState[uniVoice].lastDraw = Draw();
  float a = 1.0f;
  for (int i = 0; i < _poles; i++)
  {
    a = (i - color / 2.0f) * a / (i + 1.0f);
    int colorFilterPos = (_uniState[uniVoice].colorFilterPosition + _poles - i - 1) % _poles;
    _uniState[uniVoice].lastDraw -= a * _uniState[uniVoice].colorFilterBuffer.Get(colorFilterPos);
  }
  _uniState[uniVoice].colorFilterBuffer.Set(_uniState[uniVoice].colorFilterPosition, _uniState[uniVoice].lastDraw);
  _uniState[uniVoice].colorFilterPosition = (_uniState[uniVoice].colorFilterPosition + 1) % _poles;

  return static_cast<float>(
    _uniState[uniVoice].lpFilter.Next(0,
      _uniState[uniVoice].hpFilter.Next(0,
        _uniState[uniVoice].lastDraw * scale)));
}

void
FFKSNoiseProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.ksNoise[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::KSNoise];

  auto const& xNorm = params.acc.x[0].Voice()[voice];
  auto const& yNorm = params.acc.y[0].Voice()[voice];
  auto const& lpNorm = params.acc.lp[0].Voice()[voice];
  auto const& hpNorm = params.acc.hp[0].Voice()[voice];
  auto const& colorNorm = params.acc.color[0].Voice()[voice];
  auto const& exciteNorm = params.acc.excite[0].Voice()[voice];
  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& seedNorm = params.block.seed[0].Voice()[voice];
  auto const& polesNorm = params.block.poles[0].Voice()[voice];
  auto const& fineNorm = params.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = params.acc.coarse[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];
  auto const& uniDetuneNorm = params.acc.uniDetune[0].Voice()[voice];

  _seed = topo.NormalizedToDiscreteFast(FFKSNoiseParam::Seed, seedNorm);
  _poles = topo.NormalizedToDiscreteFast(FFKSNoiseParam::Poles, polesNorm);
  _type = topo.NormalizedToListFast<FFKSNoiseType>(FFKSNoiseParam::Type, typeNorm);
  FFOsciProcessorBase::BeginVoice(state, topo.NormalizedToDiscreteFast(FFKSNoiseParam::UniCount, uniCountNorm));

  if (_type == FFKSNoiseType::Off)
    return;

  _graphPosition = 0;
  _normalPrng = FBMarsagliaPRNG(_seed / (FFKSNoiseMaxSeed + 1.0f));
  _uniformPrng = FBParkMillerPRNG(_seed / (FFKSNoiseMaxSeed + 1.0f));

  for (int u = 0; u < _uniCount; u++)
  {
    _uniState[u].phaseGen = {};
    _uniState[u].lpFilter = {};
    _uniState[u].hpFilter = {};
    _uniState[u].lastDraw = 0.0f;
    _uniState[u].prevDelayVal = 0.0f;
    _uniState[u].phaseTowardsX = 0.0f;
    _uniState[u].colorFilterPosition = 0;

    for (int p = 0; p < _poles; p++)
      _uniState[u].colorFilterBuffer.Set(p, Draw());

    float lpPlain = topo.NormalizedToLog2Fast(FFKSNoiseParam::LP, lpNorm.CV().Get(0));
    float hpPlain = topo.NormalizedToLog2Fast(FFKSNoiseParam::HP, hpNorm.CV().Get(0));
    float xPlain = topo.NormalizedToIdentityFast(FFKSNoiseParam::X, xNorm.CV().Get(0));
    float yPlain = topo.NormalizedToIdentityFast(FFKSNoiseParam::Y, yNorm.CV().Get(0));
    float finePlain = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm.CV().Get(0));
    float coarsePlain = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm.CV().Get(0));
    float excitePlain = topo.NormalizedToLog2Fast(FFKSNoiseParam::Excite, exciteNorm.CV().Get(0));
    float colorPlain = topo.NormalizedToIdentityFast(FFKSNoiseParam::Color, colorNorm.CV().Get(0));
    float uniDetunePlain = topo.NormalizedToIdentityFast(FFKSNoiseParam::UniDetune, uniDetuneNorm.CV().Get(0));

    float basePitch = _key + coarsePlain + finePlain;
    float uniPitch = basePitch + _uniPosMHalfToHalf.Get(u) * uniDetunePlain;
    float uniFreq = FBPitchToFreq(uniPitch);

    for (int i = 0; i < DelayLineSize; i++)
      _uniState[u].delayLine.Push(Next(
        topo, u, sampleRate, uniFreq, excitePlain,
        colorPlain, xPlain, yPlain, lpPlain, hpPlain));
  }
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
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::KSNoise];

  auto const& lpNorm = procParams.acc.lp[0].Voice()[voice];
  auto const& hpNorm = procParams.acc.hp[0].Voice()[voice];
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.uniBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.uniDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.uniSpread[0].Voice()[voice];
  auto const& xNorm = procParams.acc.x[0].Voice()[voice];
  auto const& yNorm = procParams.acc.y[0].Voice()[voice];
  auto const& colorNorm = procParams.acc.color[0].Voice()[voice];
  auto const& rangeNorm = procParams.acc.range[0].Voice()[voice];
  auto const& centerNorm = procParams.acc.center[0].Voice()[voice];
  auto const& exciteNorm = procParams.acc.excite[0].Voice()[voice];
  auto const& dampNorm = procParams.acc.damp[0].Voice()[voice];
  auto const& dampScaleNorm = procParams.acc.dampScale[0].Voice()[voice];
  auto const& feedbackNorm = procParams.acc.feedback[0].Voice()[voice];
  auto const& feedbackScaleNorm = procParams.acc.feedbackScale[0].Voice()[voice];

  FBSArray<float, FBFixedBlockSamples> xPlain = {};
  FBSArray<float, FBFixedBlockSamples> yPlain = {};
  FBSArray<float, FBFixedBlockSamples> lpPlain = {};
  FBSArray<float, FBFixedBlockSamples> hpPlain = {};
  FBSArray<float, FBFixedBlockSamples> dampPlain = {};
  FBSArray<float, FBFixedBlockSamples> colorPlain = {};
  FBSArray<float, FBFixedBlockSamples> rangePlain = {};
  FBSArray<float, FBFixedBlockSamples> centerPlain = {};
  FBSArray<float, FBFixedBlockSamples> excitePlain = {};
  FBSArray<float, FBFixedBlockSamples> feedbackPlain = {};
  FBSArray<float, FBFixedBlockSamples> baseFreqPlain = {};
  FBSArray<float, FBFixedBlockSamples> basePitchPlain = {};
  FBSArray<float, FBFixedBlockSamples> uniDetunePlain = {};
  FBSArray<float, FBFixedBlockSamples> dampScalePlain = {};
  FBSArray<float, FBFixedBlockSamples> feedbackScalePlain = {};
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm, s);
    auto coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, s);
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    basePitchPlain.Store(s, pitch);
    baseFreqPlain.Store(s, baseFreq);

    lpPlain.Store(s, topo.NormalizedToLog2Fast(FFKSNoiseParam::LP, lpNorm, s));
    hpPlain.Store(s, topo.NormalizedToLog2Fast(FFKSNoiseParam::HP, hpNorm, s));
    xPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::X, xNorm, s));
    yPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::Y, yNorm, s));
    dampPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::Damp, dampNorm, s));
    colorPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::Color, colorNorm, s));
    rangePlain.Store(s, topo.NormalizedToLinearFast(FFKSNoiseParam::Range, rangeNorm, s));
    excitePlain.Store(s, topo.NormalizedToLog2Fast(FFKSNoiseParam::Excite, exciteNorm, s));
    centerPlain.Store(s, topo.NormalizedToLinearFast(FFKSNoiseParam::Center, centerNorm, s));
    feedbackPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::Feedback, feedbackNorm, s));
    dampScalePlain.Store(s, topo.NormalizedToLinearFast(FFKSNoiseParam::DampScale, dampScaleNorm, s));
    uniDetunePlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::UniDetune, uniDetuneNorm, s));
    feedbackScalePlain.Store(s, topo.NormalizedToLinearFast(FFKSNoiseParam::FeedbackScale, feedbackScaleNorm, s));
    _gainPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::Gain, gainNorm, s));
    _uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::UniBlend, uniBlendNorm, s));
    _uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFKSNoiseParam::UniSpread, uniSpreadNorm, s));
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float x = xPlain.Get(s);
    float y = yPlain.Get(s);
    float lp = lpPlain.Get(s);
    float hp = hpPlain.Get(s);
    float damp = dampPlain.Get(s);
    float range = rangePlain.Get(s);
    float color = colorPlain.Get(s);
    float excite = excitePlain.Get(s);
    float feedback = feedbackPlain.Get(s);
    float basePitch = basePitchPlain.Get(s);
    float dampScale = dampScalePlain.Get(s);
    float uniDetune = uniDetunePlain.Get(s);
    float centerPitch = 60.0f + centerPlain.Get(s);
    float feedbackScale = feedbackScalePlain.Get(s);

    float pitchDiffSemis = _key - centerPitch;
    float pitchDiffNorm = std::clamp(pitchDiffSemis / range, -1.0f, 1.0f);
    damp = std::clamp(damp - 0.5f * dampScale * pitchDiffNorm, 0.0f, 1.0f);
    feedback = std::clamp(feedback + 0.5f * feedbackScale * pitchDiffNorm, 0.0f, 1.0f);
    float realFeedback = 0.9f + 0.1f * feedback;

    for (int u = 0; u < _uniCount; u++)
    {
      float uniPitch = basePitch + _uniPosMHalfToHalf.Get(u) * uniDetune;
      float uniFreq = FBPitchToFreq(uniPitch);
      _uniState[u].delayLine.Delay(sampleRate / uniFreq);
      float nextVal = _uniState[u].delayLine.Pop();
      float prevVal = _uniState[u].prevDelayVal;
      float newVal = (1.0f - damp) * nextVal + damp * (prevVal + nextVal) * 0.5f;
      float outVal = _uniState[u].dcFilter.Next(newVal);
      newVal *= realFeedback;
      _uniState[u].prevDelayVal = newVal;
      newVal = (1.0f - excite) * newVal + excite * Next(topo, u, sampleRate, uniFreq, excite, color, x, y, lp, hp);
      _uniState[u].delayLine.Push(newVal);
      _uniOutput[u].Set(s, outVal);
    }

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
  exchangeParams.acc.lp[0][voice] = lpNorm.Last();
  exchangeParams.acc.hp[0][voice] = hpNorm.Last();
  exchangeParams.acc.excite[0][voice] = exciteNorm.Last();
  exchangeParams.acc.damp[0][voice] = dampNorm.Last();
  exchangeParams.acc.dampScale[0][voice] = dampScaleNorm.Last();
  exchangeParams.acc.range[0][voice] = rangeNorm.Last();
  exchangeParams.acc.center[0][voice] = centerNorm.Last();
  exchangeParams.acc.feedback[0][voice] = feedbackNorm.Last();
  exchangeParams.acc.feedbackScale[0][voice] = feedbackScaleNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainNorm.Last();
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.color[0][voice] = colorNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.uniBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.uniDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.uniSpread[0][voice] = uniSpreadNorm.Last();
  return FBFixedBlockSamples;
}