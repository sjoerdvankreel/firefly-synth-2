#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciTopo.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

#include <bit>
#include <cstdint>

inline float const DCBlockFreq = 20.0f;
inline float const StringOsciMinFreq = 20.0f;

// https://www.reddit.com/r/DSP/comments/8fm3c5/what_am_i_doing_wrong_brown_noise/
// 1/f^a noise https://sampo.kapsi.fi/PinkNoise/
// kps https://dsp.stackexchange.com/questions/12596/synthesizing-harmonic-tones-with-karplus-strong

FFStringOsciProcessor::
FFStringOsciProcessor() {}

void
FFStringOsciProcessor::InitializeBuffers(bool graph, float sampleRate)
{
  int delayLineSize = static_cast<int>(std::ceil(sampleRate / StringOsciMinFreq));
  for (int i = 0; i < FFOsciBaseUniMaxCount; i++)
  {
    if (_uniState[i].delayLine.Count() < delayLineSize)
      _uniState[i].delayLine.Resize(delayLineSize);
    _uniState[i].dcFilter.SetCoeffs(DCBlockFreq, sampleRate);
  }
}

inline float
FFStringOsciProcessor::Draw()
{
  if (_type == FFStringOsciType::Uni)
    return FBToBipolar(_uniformPrng.NextScalar());
  assert(_type == FFStringOsciType::Norm);
  float result = 0.0f;
  do
  {
    result = _normalPrng.NextScalar();
  } while (result < -3.0f || result > 3.0f);
  return result / 3.0f;
}

inline float
FFStringOsciProcessor::Next(
  FBStaticModule const& topo, int uniVoice,
  float sampleRate, float uniFreq, 
  float excite, float colorPlain, 
  float xPlain, float yPlain)
{
  float const empirical1 = 0.75f;
  float const empirical2 = 4.0f;
  float x = xPlain;
  float y = 0.01f + 0.99f * yPlain;
  float color = 1.99f * (1.0f - colorPlain);
  float scale = 1.0f - ((1.0f - colorPlain) * empirical1);
  scale *= (1.0f + empirical2 * (1.0f - excite));

  _uniState[uniVoice].phaseTowardsX += uniFreq / sampleRate;
  if (_uniState[uniVoice].phaseTowardsX < 1.0f - x)
    return _uniState[uniVoice].lastDraw * scale;

  _uniState[uniVoice].phaseTowardsX = 0.0f;
  if (_uniformPrng.NextScalar() > y) 
    return _uniState[uniVoice].lastDraw * scale;

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
  return _uniState[uniVoice].lastDraw * scale;
}

void
FFStringOsciProcessor::BeginVoice(bool graph, FBModuleProcState& state)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.stringOsci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::StringOsci];

  auto const& xNorm = params.acc.x[0].Voice()[voice];
  auto const& yNorm = params.acc.y[0].Voice()[voice];
  auto const& lpResNorm = params.acc.lpRes[0].Voice()[voice];
  auto const& hpResNorm = params.acc.hpRes[0].Voice()[voice];
  auto const& lpFreqNorm = params.acc.lpFreq[0].Voice()[voice];
  auto const& hpFreqNorm = params.acc.hpFreq[0].Voice()[voice];
  auto const& lpKTrkNorm = params.acc.lpKTrk[0].Voice()[voice];
  auto const& hpKTrkNorm = params.acc.hpKTrk[0].Voice()[voice];
  auto const& colorNorm = params.acc.color[0].Voice()[voice];
  auto const& exciteNorm = params.acc.excite[0].Voice()[voice];
  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& seedNorm = params.block.seed[0].Voice()[voice];
  auto const& polesNorm = params.block.poles[0].Voice()[voice];
  auto const& fineNorm = params.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = params.acc.coarse[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];
  auto const& uniDetuneNorm = params.acc.uniDetune[0].Voice()[voice];
  auto const& trackingKeyNorm = params.acc.trackingKey[0].Voice()[voice];

  _seed = topo.NormalizedToDiscreteFast(FFStringOsciParam::Seed, seedNorm);
  _poles = topo.NormalizedToDiscreteFast(FFStringOsciParam::Poles, polesNorm);
  _type = topo.NormalizedToListFast<FFStringOsciType>(FFStringOsciParam::Type, typeNorm);
  FFOsciProcessorBase::BeginVoice(state, topo.NormalizedToDiscreteFast(FFStringOsciParam::UniCount, uniCountNorm));

  if (_type == FFStringOsciType::Off)
    return;

  float lpResPlain = topo.NormalizedToIdentityFast(FFStringOsciParam::LPRes, lpResNorm.CV().Get(0));
  float hpResPlain = topo.NormalizedToIdentityFast(FFStringOsciParam::HPRes, hpResNorm.CV().Get(0));
  float lpFreqPlain = topo.NormalizedToLog2Fast(FFStringOsciParam::LPFreq, lpFreqNorm.CV().Get(0));
  float hpFreqPlain = topo.NormalizedToLog2Fast(FFStringOsciParam::HPFreq, hpFreqNorm.CV().Get(0));
  float lpKTrkPlain = topo.NormalizedToLinearFast(FFStringOsciParam::LPKTrk, lpKTrkNorm.CV().Get(0));
  float hpKTrkPlain = topo.NormalizedToLinearFast(FFStringOsciParam::HPKTrk, hpKTrkNorm.CV().Get(0));
  float xPlain = topo.NormalizedToIdentityFast(FFStringOsciParam::X, xNorm.CV().Get(0));
  float yPlain = topo.NormalizedToIdentityFast(FFStringOsciParam::Y, yNorm.CV().Get(0));
  float finePlain = topo.NormalizedToLinearFast(FFStringOsciParam::Fine, fineNorm.CV().Get(0));
  float coarsePlain = topo.NormalizedToLinearFast(FFStringOsciParam::Coarse, coarseNorm.CV().Get(0));
  float excitePlain = topo.NormalizedToLog2Fast(FFStringOsciParam::Excite, exciteNorm.CV().Get(0));
  float colorPlain = topo.NormalizedToIdentityFast(FFStringOsciParam::Color, colorNorm.CV().Get(0));
  float uniDetunePlain = topo.NormalizedToIdentityFast(FFStringOsciParam::UniDetune, uniDetuneNorm.CV().Get(0));
  float trackingKeyPlain = topo.NormalizedToLinearFast(FFStringOsciParam::TrackingKey, trackingKeyNorm.CV().Get(0));

  _lpFilter = {};
  _hpFilter = {};
  _graphPosition = 0;
  _normalPrng = FFMarsagliaPRNG(_seed / (FFStringOsciMaxSeed + 1.0f));
  _uniformPrng = FFParkMillerPRNG(_seed / (FFStringOsciMaxSeed + 1.0f));

  lpFreqPlain = WithKeyboardTracking(lpFreqPlain, _key, trackingKeyPlain, lpKTrkPlain, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  hpFreqPlain = WithKeyboardTracking(hpFreqPlain, _key, trackingKeyPlain, -hpKTrkPlain, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
  _graphStVarFilterFreqMultiplier = FFGraphFilterFreqMultiplier(graph, state.input->sampleRate, FFMaxStateVariableFilterFreq);
  _lpFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreqPlain * _graphStVarFilterFreqMultiplier, lpResPlain, 0.0f);
  _hpFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, hpFreqPlain * _graphStVarFilterFreqMultiplier, hpResPlain, 0.0f);

  int delayLineSize = static_cast<int>(std::ceil(sampleRate / StringOsciMinFreq));
  for (int u = 0; u < _uniCount; u++)
  {
    _uniState[u].phaseGen = {};
    _uniState[u].lastDraw = 0.0f;
    _uniState[u].prevDelayVal = 0.0f;
    _uniState[u].phaseTowardsX = 0.0f;
    _uniState[u].colorFilterPosition = 0;
    _uniState[u].delayLine.Reset();

    for (int p = 0; p < _poles; p++)
      _uniState[u].colorFilterBuffer.Set(p, Draw());

    float basePitch = _key + coarsePlain + finePlain;
    float uniPitch = basePitch + _uniPosMHalfToHalf.Get(u) * uniDetunePlain;
    float uniFreq = FBPitchToFreq(uniPitch);
    for (int i = 0; i < delayLineSize; i++)
    {
      float nextVal = Next(topo, u, sampleRate, uniFreq, excitePlain, colorPlain, xPlain, yPlain);
      _uniState[u].delayLine.Push(static_cast<float>(_lpFilter.Next(u, _hpFilter.Next(u, nextVal))));
    }
  }
}

int
FFStringOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.stringOsci[state.moduleSlot].output;

  output.Fill(0.0f);
  if (_type == FFStringOsciType::Off)
    return 0;
  
  float sampleRate = state.input->sampleRate;
  auto const& procParams = procState->param.voice.stringOsci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::StringOsci];

  auto const& lpResNorm = procParams.acc.lpRes[0].Voice()[voice];
  auto const& hpResNorm = procParams.acc.hpRes[0].Voice()[voice];
  auto const& lpFreqNorm = procParams.acc.lpFreq[0].Voice()[voice];
  auto const& hpFreqNorm = procParams.acc.hpFreq[0].Voice()[voice];
  auto const& lpKTrkNorm = procParams.acc.lpKTrk[0].Voice()[voice];
  auto const& hpKTrkNorm = procParams.acc.hpKTrk[0].Voice()[voice];
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.uniBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.uniDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.uniSpread[0].Voice()[voice];
  auto const& xNorm = procParams.acc.x[0].Voice()[voice];
  auto const& yNorm = procParams.acc.y[0].Voice()[voice];
  auto const& colorNorm = procParams.acc.color[0].Voice()[voice];
  auto const& trackingRangeNorm = procParams.acc.trackingRange[0].Voice()[voice];
  auto const& trackingKeyNorm = procParams.acc.trackingKey[0].Voice()[voice];
  auto const& exciteNorm = procParams.acc.excite[0].Voice()[voice];
  auto const& dampNorm = procParams.acc.damp[0].Voice()[voice];
  auto const& dampKTrkNorm = procParams.acc.dampKTrk[0].Voice()[voice];
  auto const& feedbackNorm = procParams.acc.feedback[0].Voice()[voice];
  auto const& feedbackKTrkNorm = procParams.acc.feedbackKTrk[0].Voice()[voice];

  FBSArray<float, FBFixedBlockSamples> xPlain = {};
  FBSArray<float, FBFixedBlockSamples> yPlain = {};
  FBSArray<float, FBFixedBlockSamples> lpResPlain = {};
  FBSArray<float, FBFixedBlockSamples> hpResPlain = {};
  FBSArray<float, FBFixedBlockSamples> lpFreqPlain = {};
  FBSArray<float, FBFixedBlockSamples> hpFreqPlain = {};
  FBSArray<float, FBFixedBlockSamples> lpKTrkPlain = {};
  FBSArray<float, FBFixedBlockSamples> hpKTrkPlain = {};
  FBSArray<float, FBFixedBlockSamples> dampPlain = {};
  FBSArray<float, FBFixedBlockSamples> colorPlain = {};
  FBSArray<float, FBFixedBlockSamples> excitePlain = {};
  FBSArray<float, FBFixedBlockSamples> feedbackPlain = {};
  FBSArray<float, FBFixedBlockSamples> baseFreqPlain = {};
  FBSArray<float, FBFixedBlockSamples> basePitchPlain = {};
  FBSArray<float, FBFixedBlockSamples> uniDetunePlain = {};
  FBSArray<float, FBFixedBlockSamples> dampKTrkPlain = {};
  FBSArray<float, FBFixedBlockSamples> feedbackKTrkPlain = {};
  FBSArray<float, FBFixedBlockSamples> trackingKeyPlain = {};
  FBSArray<float, FBFixedBlockSamples> trackingRangePlain = {};
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm, s);
    auto coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, s);
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    basePitchPlain.Store(s, pitch);
    baseFreqPlain.Store(s, baseFreq);

    lpResPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::LPRes, lpResNorm, s));
    hpResPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::HPRes, hpResNorm, s));
    lpFreqPlain.Store(s, topo.NormalizedToLog2Fast(FFStringOsciParam::LPFreq, lpFreqNorm, s));
    hpFreqPlain.Store(s, topo.NormalizedToLog2Fast(FFStringOsciParam::HPFreq, hpFreqNorm, s));
    lpKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFStringOsciParam::LPKTrk, lpKTrkNorm, s));
    hpKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFStringOsciParam::HPKTrk, hpKTrkNorm, s));
    xPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::X, xNorm, s));
    yPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::Y, yNorm, s));
    dampPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::Damp, dampNorm, s));
    colorPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::Color, colorNorm, s));
    excitePlain.Store(s, topo.NormalizedToLog2Fast(FFStringOsciParam::Excite, exciteNorm, s));
    trackingRangePlain.Store(s, topo.NormalizedToLinearFast(FFStringOsciParam::TrackingRange, trackingRangeNorm, s));
    trackingKeyPlain.Store(s, topo.NormalizedToLinearFast(FFStringOsciParam::TrackingKey, trackingKeyNorm, s));
    feedbackPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::Feedback, feedbackNorm, s));
    dampKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFStringOsciParam::DampKTrk, dampKTrkNorm, s));
    uniDetunePlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::UniDetune, uniDetuneNorm, s));
    feedbackKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFStringOsciParam::FeedbackKTrk, feedbackKTrkNorm, s));
    _gainPlain.Store(s, topo.NormalizedToLinearFast(FFStringOsciParam::Gain, gainNorm, s));
    _uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::UniBlend, uniBlendNorm, s));
    _uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFStringOsciParam::UniSpread, uniSpreadNorm, s));
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float x = xPlain.Get(s);
    float y = yPlain.Get(s);
    float lpRes = lpResPlain.Get(s);
    float hpRes = hpResPlain.Get(s);
    float lpFreq = lpFreqPlain.Get(s);
    float hpFreq = hpFreqPlain.Get(s);
    float lpKTrk = lpKTrkPlain.Get(s);
    float hpKTrk = hpKTrkPlain.Get(s);
    float damp = dampPlain.Get(s);
    float trackingKey = trackingKeyPlain.Get(s);
    float trackingRange = trackingRangePlain.Get(s);
    float color = colorPlain.Get(s);
    float excite = excitePlain.Get(s);
    float feedback = feedbackPlain.Get(s);
    float basePitch = basePitchPlain.Get(s);
    float dampKTrk = dampKTrkPlain.Get(s);
    float uniDetune = uniDetunePlain.Get(s);
    float centerPitch = 60.0f + trackingKey;
    float feedbackKTrk = feedbackKTrkPlain.Get(s);

    float pitchDiffSemis = _key - centerPitch;
    float pitchDiffNorm = std::clamp(pitchDiffSemis / trackingRange, -1.0f, 1.0f);
    damp = std::clamp(damp - 0.5f * dampKTrk * pitchDiffNorm, 0.0f, 1.0f);
    feedback = std::clamp(feedback + 0.5f * feedbackKTrk * pitchDiffNorm, 0.0f, 1.0f);
    float realFeedback = 0.9f + 0.1f * feedback;

    lpFreq = WithKeyboardTracking(lpFreq, _key, trackingKey, lpKTrk, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
    hpFreq = WithKeyboardTracking(hpFreq, _key, trackingKey, -hpKTrk, FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
    _lpFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreq * _graphStVarFilterFreqMultiplier, lpRes, 0.0f);
    _hpFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, hpFreq * _graphStVarFilterFreqMultiplier, hpRes, 0.0f);
    for (int ub = 0; ub < _uniCount; ub += FBSIMDFloatCount)
    {
      auto uniPitchBatch = basePitch + _uniPosMHalfToHalf.Load(ub) * uniDetune;
      auto uniFreqBatch = FBPitchToFreq(uniPitchBatch);
      FBSArray<float, FBSIMDFloatCount> uniFreqArray(uniFreqBatch);

      for (int u = ub; u < ub + FBSIMDFloatCount && u < _uniCount; u++)
      {
        float uniFreq = uniFreqArray.Get(u - ub);
        _uniState[u].delayLine.Delay(sampleRate / uniFreq);
        float thisVal = _uniState[u].delayLine.Pop();
        float prevVal = _uniState[u].prevDelayVal;
        float newVal = (1.0f - damp) * thisVal + damp * (prevVal + thisVal) * 0.5f;
        float outVal = _uniState[u].dcFilter.Next(newVal);
        newVal *= realFeedback;
        _uniState[u].prevDelayVal = newVal;
        float nextVal = Next(topo, u, sampleRate, uniFreq, excite, color, x, y);
        nextVal = static_cast<float>(_lpFilter.Next(u, _hpFilter.Next(u, nextVal)));
        newVal = (1.0f - excite) * newVal + excite * nextVal;
        _uniState[u].delayLine.Push(newVal);
        _uniOutput[u].Set(s, outVal);
      }
    }

    _graphPosition++;
  }

  ProcessGainSpreadBlend(output);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    int graphSamples = FBFreqToSamples(baseFreqPlain.Last(), sampleRate) * FFStringOsciGraphRounds;
    return std::clamp(graphSamples - _graphPosition, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = exchangeToGUI->voice[voice].stringOsci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreqPlain.Get(FBFixedBlockSamples - 1), state.input->sampleRate);

  auto& exchangeParams = exchangeToGUI->param.voice.stringOsci[state.moduleSlot];
  exchangeParams.acc.x[0][voice] = xNorm.Last();
  exchangeParams.acc.y[0][voice] = yNorm.Last();
  exchangeParams.acc.lpRes[0][voice] = lpResNorm.Last();
  exchangeParams.acc.hpRes[0][voice] = hpResNorm.Last();
  exchangeParams.acc.lpFreq[0][voice] = lpFreqNorm.Last();
  exchangeParams.acc.hpFreq[0][voice] = hpFreqNorm.Last();
  exchangeParams.acc.lpKTrk[0][voice] = lpKTrkNorm.Last();
  exchangeParams.acc.hpKTrk[0][voice] = hpKTrkNorm.Last();
  exchangeParams.acc.excite[0][voice] = exciteNorm.Last();
  exchangeParams.acc.damp[0][voice] = dampNorm.Last();
  exchangeParams.acc.dampKTrk[0][voice] = dampKTrkNorm.Last();
  exchangeParams.acc.trackingRange[0][voice] = trackingRangeNorm.Last();
  exchangeParams.acc.trackingKey[0][voice] = trackingKeyNorm.Last();
  exchangeParams.acc.feedback[0][voice] = feedbackNorm.Last();
  exchangeParams.acc.feedbackKTrk[0][voice] = feedbackKTrkNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainNorm.Last();
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.color[0][voice] = colorNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.uniBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.uniDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.uniSpread[0][voice] = uniSpreadNorm.Last();
  return FBFixedBlockSamples;
}