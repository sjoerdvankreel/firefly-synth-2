#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

static inline float const StringDCBlockFreq = 20.0f;

float
FFOsciProcessor::StringDraw()
{
  if (_stringMode == FFOsciStringMode::Uni)
    return FBToBipolar(_uniformPrng.NextScalar());
  FB_ASSERT(_stringMode == FFOsciStringMode::Norm);
  float result = 0.0f;
  do
  {
    result = _stringNormalPrng.NextScalar();
  } while (result < -3.0f || result > 3.0f);
  return result / 3.0f;
}

float
FFOsciProcessor::StringNext(
  int uniVoice,
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

  _stringUniState[uniVoice].phaseTowardsX += uniFreq / sampleRate;
  if (_stringUniState[uniVoice].phaseTowardsX < 1.0f - x)
    return _stringUniState[uniVoice].lastDraw * scale;

  _stringUniState[uniVoice].phaseTowardsX = 0.0f;
  if (_uniformPrng.NextScalar() > y)
    return _stringUniState[uniVoice].lastDraw * scale;

  _stringUniState[uniVoice].lastDraw = StringDraw();
  float a = 1.0f;
  for (int i = 0; i < _stringPoles; i++)
  {
    a = (i - color / 2.0f) * a / (i + 1.0f);
    int colorFilterPos = (_stringUniState[uniVoice].colorFilterPosition + _stringPoles - i - 1) % _stringPoles;
    _stringUniState[uniVoice].lastDraw -= a * _stringUniState[uniVoice].colorFilterBuffer.Get(colorFilterPos);
  }
  _stringUniState[uniVoice].colorFilterBuffer.Set(_stringUniState[uniVoice].colorFilterPosition, _stringUniState[uniVoice].lastDraw);
  _stringUniState[uniVoice].colorFilterPosition = (_stringUniState[uniVoice].colorFilterPosition + 1) % _stringPoles;
  return _stringUniState[uniVoice].lastDraw * scale;
}

void 
FFOsciProcessor::BeginVoiceString(bool graph, FBModuleProcState& state)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  float oversampledRate = sampleRate * _oversampleTimes;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  auto const& fineNorm = params.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = params.acc.coarse[0].Voice()[voice];
  auto const& uniDetuneNorm = params.acc.uniDetune[0].Voice()[voice];

  float finePlain = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm.CV().Get(0));
  float coarsePlain = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm.CV().Get(0));
  float uniDetunePlain = topo.NormalizedToIdentityFast(FFOsciParam::UniDetune, uniDetuneNorm.CV().Get(0));

  auto const& stringLPOnNorm = params.block.stringLPOn[0].Voice()[voice];
  auto const& stringHPOnNorm = params.block.stringHPOn[0].Voice()[voice];
  auto const& stringModeNorm = params.block.stringMode[0].Voice()[voice];
  auto const& stringSeedNorm = params.block.stringSeed[0].Voice()[voice];
  auto const& stringPolesNorm = params.block.stringPoles[0].Voice()[voice];

  auto const& stringXNorm = params.acc.stringX[0].Voice()[voice];
  auto const& stringYNorm = params.acc.stringY[0].Voice()[voice];
  auto const& stringLPResNorm = params.acc.stringLPRes[0].Voice()[voice];
  auto const& stringHPResNorm = params.acc.stringHPRes[0].Voice()[voice];
  auto const& stringLPFreqNorm = params.acc.stringLPFreq[0].Voice()[voice];
  auto const& stringHPFreqNorm = params.acc.stringHPFreq[0].Voice()[voice];
  auto const& stringLPKTrkNorm = params.acc.stringLPKTrk[0].Voice()[voice];
  auto const& stringHPKTrkNorm = params.acc.stringHPKTrk[0].Voice()[voice];
  auto const& stringColorNorm = params.acc.stringColor[0].Voice()[voice];
  auto const& stringExciteNorm = params.acc.stringExcite[0].Voice()[voice];
  auto const& stringTrackingKeyNorm = params.acc.stringTrackingKey[0].Voice()[voice];

  _stringGraphPosition = 0;
  _stringGraphStVarFilterFreqMultiplier = FFGraphFilterFreqMultiplier(
    graph, state.input->sampleRate, FFMaxStateVariableFilterFreq);

  _stringLPOn = topo.NormalizedToBoolFast(FFOsciParam::StringLPOn, stringLPOnNorm);
  _stringHPOn = topo.NormalizedToBoolFast(FFOsciParam::StringHPOn, stringHPOnNorm);
  _stringSeed = topo.NormalizedToDiscreteFast(FFOsciParam::StringSeed, stringSeedNorm);
  _stringPoles = topo.NormalizedToDiscreteFast(FFOsciParam::StringPoles, stringPolesNorm);
  _stringMode = topo.NormalizedToListFast<FFOsciStringMode>(FFOsciParam::StringMode, stringModeNorm);

  float stringLPResPlain = topo.NormalizedToIdentityFast(FFOsciParam::StringLPRes, stringLPResNorm.CV().Get(0));
  float stringHPResPlain = topo.NormalizedToIdentityFast(FFOsciParam::StringHPRes, stringHPResNorm.CV().Get(0));
  float stringLPFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::StringLPFreq, stringLPFreqNorm.CV().Get(0));
  float stringHPFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::StringHPFreq, stringHPFreqNorm.CV().Get(0));
  float stringLPKTrkPlain = topo.NormalizedToLinearFast(FFOsciParam::StringLPKTrk, stringLPKTrkNorm.CV().Get(0));
  float stringHPKTrkPlain = topo.NormalizedToLinearFast(FFOsciParam::StringHPKTrk, stringHPKTrkNorm.CV().Get(0));
  float stringXPlain = topo.NormalizedToIdentityFast(FFOsciParam::StringX, stringXNorm.CV().Get(0));
  float stringYPlain = topo.NormalizedToIdentityFast(FFOsciParam::StringY, stringYNorm.CV().Get(0));
  float stringExcitePlain = topo.NormalizedToLog2Fast(FFOsciParam::StringExcite, stringExciteNorm.CV().Get(0));
  float stringColorPlain = topo.NormalizedToIdentityFast(FFOsciParam::StringColor, stringColorNorm.CV().Get(0));
  float stringTrackingKeyPlain = topo.NormalizedToLinearFast(FFOsciParam::StringTrackingKey, stringTrackingKeyNorm.CV().Get(0));

  _stringLPFilter.Reset();
  _stringHPFilter.Reset();
  _stringNormalPrng = FFMarsagliaPRNG(_stringSeed / (FFOsciStringMaxSeed + 1.0f));
  _uniformPrng = FFParkMillerPRNG(_stringSeed / (FFOsciStringMaxSeed + 1.0f));

  if (_stringLPOn)
  {
    stringLPFreqPlain = MultiplyClamp(stringLPFreqPlain,
      KeyboardTrackingMultiplier(_key, stringTrackingKeyPlain, stringLPKTrkPlain),
      FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
    stringLPFreqPlain *= _stringGraphStVarFilterFreqMultiplier;
    _stringLPFilter.Set(FFStateVariableFilterMode::LPF, oversampledRate, stringLPFreqPlain, stringLPResPlain, 0.0f);
  }

  if (_stringHPOn)
  {
    stringHPFreqPlain = MultiplyClamp(stringHPFreqPlain,
      KeyboardTrackingMultiplier(_key, stringTrackingKeyPlain, -stringHPKTrkPlain),
      FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
    stringHPFreqPlain *= _stringGraphStVarFilterFreqMultiplier;
    _stringHPFilter.Set(FFStateVariableFilterMode::HPF, oversampledRate, stringHPFreqPlain, stringHPResPlain, 0.0f);
  }

  int delayLineSize = static_cast<int>(std::ceil(oversampledRate / FFOsciStringMinFreq));
  for (int u = 0; u < _uniCount; u++)
  {
    _stringUniState[u].phaseGen = {};
    _stringUniState[u].lastDraw = 0.0f;
    _stringUniState[u].prevDelayVal = 0.0f;
    _stringUniState[u].phaseTowardsX = 0.0f;
    _stringUniState[u].colorFilterPosition = 0;
    _stringUniState[u].dcFilter.SetCoeffs(StringDCBlockFreq, oversampledRate);
    _stringUniState[u].delayLine.Reset(_stringUniState[u].delayLine.MaxBufferSize() * _oversampleTimes / FFOsciOversampleTimes);

    for (int p = 0; p < _stringPoles; p++)
      _stringUniState[u].colorFilterBuffer.Set(p, StringDraw());

    float basePitch = _key + coarsePlain + finePlain;
    float uniPitch = basePitch + _uniPosMHalfToHalf.Get(u) * uniDetunePlain;
    float uniFreq = FBPitchToFreq(uniPitch);
    for (int i = 0; i < delayLineSize; i++)
    {
      double dNextVal = StringNext(u, oversampledRate, uniFreq, stringExcitePlain, stringColorPlain, stringXPlain, stringYPlain);
      if (_stringHPOn)
        dNextVal = _stringHPFilter.Next(u, dNextVal);
      if (_stringLPOn)
        dNextVal = _stringLPFilter.Next(u, dNextVal);
      _stringUniState[u].delayLine.Push(static_cast<float>(dNextVal));
    }
  }
}

void 
FFOsciProcessor::ProcessString(
  FBModuleProcState& state,
  FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
  FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  float sampleRate = state.input->sampleRate;
  float oversampledRate = sampleRate * _oversampleTimes;
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto& voiceState = procState->dsp.voice[voice];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  auto const& stringLPResNorm = procParams.acc.stringLPRes[0].Voice()[voice];
  auto const& stringHPResNorm = procParams.acc.stringHPRes[0].Voice()[voice];
  auto const& stringLPFreqNorm = procParams.acc.stringLPFreq[0].Voice()[voice];
  auto const& stringHPFreqNorm = procParams.acc.stringHPFreq[0].Voice()[voice];
  auto const& stringLPKTrkNorm = procParams.acc.stringLPKTrk[0].Voice()[voice];
  auto const& stringHPKTrkNorm = procParams.acc.stringHPKTrk[0].Voice()[voice];
  auto const& stringXNorm = procParams.acc.stringX[0].Voice()[voice];
  auto const& stringYNorm = procParams.acc.stringY[0].Voice()[voice];
  auto const& stringColorNorm = procParams.acc.stringColor[0].Voice()[voice];
  auto const& stringTrackingRangeNorm = procParams.acc.stringTrackingRange[0].Voice()[voice];
  auto const& stringTrackingKeyNorm = procParams.acc.stringTrackingKey[0].Voice()[voice];
  auto const& stringExciteNorm = procParams.acc.stringExcite[0].Voice()[voice];
  auto const& stringDampNorm = procParams.acc.stringDamp[0].Voice()[voice];
  auto const& stringDampKTrkNorm = procParams.acc.stringDampKTrk[0].Voice()[voice];
  auto const& stringFeedbackNorm = procParams.acc.stringFeedback[0].Voice()[voice];
  auto const& stringFeedbackKTrkNorm = procParams.acc.stringFeedbackKTrk[0].Voice()[voice];

  FBSArray<float, FFOsciFixedBlockOversamples> stringXPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringYPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringLPResPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringHPResPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringLPFreqPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringHPFreqPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringLPKTrkPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringHPKTrkPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringDampPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringColorPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringExcitePlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringFeedbackPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringDampKTrkPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringFeedbackKTrkPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringTrackingKeyPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringTrackingRangePlain = {};
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    if (_stringLPOn)
    {
      stringLPResPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::StringLPRes, stringLPResNorm, s));
      stringLPFreqPlain.Store(s, topo.NormalizedToLog2Fast(FFOsciParam::StringLPFreq, stringLPFreqNorm, s));
      stringLPKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::StringLPKTrk, stringLPKTrkNorm, s));
    }

    if (_stringHPOn)
    {
      stringHPResPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::StringHPRes, stringHPResNorm, s));
      stringHPFreqPlain.Store(s, topo.NormalizedToLog2Fast(FFOsciParam::StringHPFreq, stringHPFreqNorm, s));
      stringHPKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::StringHPKTrk, stringHPKTrkNorm, s));
    }

    stringXPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::StringX, stringXNorm, s));
    stringYPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::StringY, stringYNorm, s));
    stringDampPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::StringDamp, stringDampNorm, s));
    stringColorPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::StringColor, stringColorNorm, s));
    stringExcitePlain.Store(s, topo.NormalizedToLog2Fast(FFOsciParam::StringExcite, stringExciteNorm, s));
    stringTrackingRangePlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::StringTrackingRange, stringTrackingRangeNorm, s));
    stringTrackingKeyPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::StringTrackingKey, stringTrackingKeyNorm, s));
    stringFeedbackPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::StringFeedback, stringFeedbackNorm, s));
    stringDampKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::StringDampKTrk, stringDampKTrkNorm, s));
    stringFeedbackKTrkPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::StringFeedbackKTrk, stringFeedbackKTrkNorm, s));
  }
  if (_oversampleTimes != 1)
  {
    if (_stringLPOn)
    {
      stringLPResPlain.UpsampleStretch<FFOsciOversampleTimes>();
      stringLPFreqPlain.UpsampleStretch<FFOsciOversampleTimes>();
      stringLPKTrkPlain.UpsampleStretch<FFOsciOversampleTimes>();
    }

    if (_stringHPOn)
    {
      stringHPResPlain.UpsampleStretch<FFOsciOversampleTimes>();
      stringHPFreqPlain.UpsampleStretch<FFOsciOversampleTimes>();
      stringHPKTrkPlain.UpsampleStretch<FFOsciOversampleTimes>();
    }

    stringXPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringYPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringDampPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringColorPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringExcitePlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringTrackingRangePlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringTrackingKeyPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringFeedbackPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringDampKTrkPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringFeedbackKTrkPlain.UpsampleStretch<FFOsciOversampleTimes>();
  }

  for (int s = 0; s < totalSamples; s++)
  {
    float x = stringXPlain.Get(s);
    float y = stringYPlain.Get(s);
    float damp = stringDampPlain.Get(s);
    float trackingKey = stringTrackingKeyPlain.Get(s);
    float trackingRange = stringTrackingRangePlain.Get(s);
    float color = stringColorPlain.Get(s);
    float excite = stringExcitePlain.Get(s);
    float feedback = stringFeedbackPlain.Get(s);
    float basePitch = basePitchPlain.Get(s);
    float dampKTrk = stringDampKTrkPlain.Get(s);
    float uniDetune = uniDetunePlain.Get(s);
    float centerPitch = 60.0f + trackingKey;
    float feedbackKTrk = stringFeedbackKTrkPlain.Get(s);
    float pitchDiffSemis = _key - centerPitch;
    float pitchDiffNorm = std::clamp(pitchDiffSemis / trackingRange, -1.0f, 1.0f);
    damp = std::clamp(damp - 0.5f * dampKTrk * pitchDiffNorm, 0.0f, 1.0f);
    feedback = std::clamp(feedback + 0.5f * feedbackKTrk * pitchDiffNorm, 0.0f, 1.0f);
    float realFeedback = 0.9f + 0.1f * feedback;

    if (_stringLPOn)
    {
      float lpRes = stringLPResPlain.Get(s);
      float lpFreq = stringLPFreqPlain.Get(s);
      float lpKTrk = stringLPKTrkPlain.Get(s);
      lpFreq = MultiplyClamp(lpFreq,
        KeyboardTrackingMultiplier(_key, trackingKey, lpKTrk),
        FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
      lpFreq *= _stringGraphStVarFilterFreqMultiplier;
      _stringLPFilter.Set(FFStateVariableFilterMode::LPF, oversampledRate, lpFreq, lpRes, 0.0f);
    }

    if (_stringHPOn)
    {
      float hpRes = stringHPResPlain.Get(s);
      float hpFreq = stringHPFreqPlain.Get(s);
      float hpKTrk = stringHPKTrkPlain.Get(s);
      hpFreq = MultiplyClamp(hpFreq,
        KeyboardTrackingMultiplier(_key, trackingKey, -hpKTrk),
        FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
      hpFreq *= _stringGraphStVarFilterFreqMultiplier;
      _stringHPFilter.Set(FFStateVariableFilterMode::HPF, oversampledRate, hpFreq, hpRes, 0.0f);
    }

    for (int ub = 0; ub < _uniCount; ub += FBSIMDFloatCount)
    {
      auto uniPitchBatch = basePitch + _uniPosMHalfToHalf.Load(ub) * uniDetune;
      auto uniFreqBatch = FBPitchToFreq(uniPitchBatch);
      FBSArray<float, FBSIMDFloatCount> uniFreqArray(uniFreqBatch);
      for (int u = ub; u < ub + FBSIMDFloatCount && u < _uniCount; u++)
      {
        float uniFreq = uniFreqArray.Get(u - ub);
        _stringUniState[u].delayLine.Delay(oversampledRate / uniFreq);
        float thisVal = _stringUniState[u].delayLine.Pop();
        float prevVal = _stringUniState[u].prevDelayVal;
        float newVal = (1.0f - damp) * thisVal + damp * (prevVal + thisVal) * 0.5f;
        float outVal = _stringUniState[u].dcFilter.Next(newVal);
        newVal *= realFeedback;
        _stringUniState[u].prevDelayVal = newVal;

        double dNextVal = StringNext(u, oversampledRate, uniFreq, excite, color, x, y);
        if (_stringHPOn)
          dNextVal = _stringHPFilter.Next(u, dNextVal);
        if (_stringLPOn)
          dNextVal = _stringLPFilter.Next(u, dNextVal);

        float nextVal = static_cast<float>(dNextVal);
        newVal = (1.0f - excite) * newVal + excite * nextVal;
        _stringUniState[u].delayLine.Push(newVal);
        uniOutputOversampled[u].Set(s, outVal);
      }
    }
    _stringGraphPosition++;
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.stringX[0][voice] = stringXNorm.Last();
  exchangeParams.acc.stringY[0][voice] = stringYNorm.Last();
  exchangeParams.acc.stringLPRes[0][voice] = stringLPResNorm.Last();
  exchangeParams.acc.stringHPRes[0][voice] = stringHPResNorm.Last();
  exchangeParams.acc.stringLPFreq[0][voice] = stringLPFreqNorm.Last();
  exchangeParams.acc.stringHPFreq[0][voice] = stringHPFreqNorm.Last();
  exchangeParams.acc.stringLPKTrk[0][voice] = stringLPKTrkNorm.Last();
  exchangeParams.acc.stringHPKTrk[0][voice] = stringHPKTrkNorm.Last();
  exchangeParams.acc.stringExcite[0][voice] = stringExciteNorm.Last();
  exchangeParams.acc.stringDamp[0][voice] = stringDampNorm.Last();
  exchangeParams.acc.stringDampKTrk[0][voice] = stringDampKTrkNorm.Last();
  exchangeParams.acc.stringTrackingRange[0][voice] = stringTrackingRangeNorm.Last();
  exchangeParams.acc.stringTrackingKey[0][voice] = stringTrackingKeyNorm.Last();
  exchangeParams.acc.stringFeedback[0][voice] = stringFeedbackNorm.Last();
  exchangeParams.acc.stringFeedbackKTrk[0][voice] = stringFeedbackKTrkNorm.Last();
  exchangeParams.acc.stringColor[0][voice] = stringColorNorm.Last();
}