#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

static inline float const StringDCBlockFreq = 20.0f;

float
FFOsciProcessor::StringDraw(
  int uniVoice)
{
  if (_stringUniState[uniVoice].haveNoiseSamples == 0)
  {
    for (int s = 0; s < FFOsciFixedBlockOversamples; s += FFOsciOversampleTimes * _oversampleTimes)
    {
      float noise;
      if (_stringMode == FFOsciStringMode::Uni)
        noise = FBToBipolar(_stringUniState[uniVoice].uniformPrng.NextScalar());
      else
        noise = _stringUniState[uniVoice].normalPrng.NextScalar();
      for (int so = 0; so < FFOsciOversampleTimes * _oversampleTimes; so++)
        _stringUniState[uniVoice].noiseOversampledBlock.setSample(0, s + so, noise);
    }
    auto channelBlockDown = _stringUniState[uniVoice].noiseDownsampledBlock.getSubsetChannelBlock(0, 1);
    _stringUniState[uniVoice].noiseOversampler->processSamplesDown(channelBlockDown);
    _stringUniState[uniVoice].haveNoiseSamples = FBFixedBlockSamples;
  }
  float out = _stringUniState[uniVoice].noiseOutput.Get(FBFixedBlockSamples - _stringUniState[uniVoice].haveNoiseSamples);
  _stringUniState[uniVoice].haveNoiseSamples--;
  return out;
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
  scale /= _oversampleTimes;

  _stringUniState[uniVoice].phaseTowardsX += uniFreq / sampleRate;
  if (_stringUniState[uniVoice].phaseTowardsX < 1.0f - x)
    return _stringUniState[uniVoice].lastDraw * scale;

  _stringUniState[uniVoice].phaseTowardsX = 0.0f;
  if (_stringUniState[uniVoice].xPrng.NextScalar() > y)
    return _stringUniState[uniVoice].lastDraw * scale;

  _stringUniState[uniVoice].lastDraw = StringDraw(uniVoice);
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
FFOsciProcessor::BeginVoiceString(FBModuleProcState& state, bool graph)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  float oversampledRate = sampleRate * _oversampleTimes;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];

  float stringLPOnNorm = params.block.stringLPOn[0].Voice()[voice];
  float stringHPOnNorm = params.block.stringHPOn[0].Voice()[voice];
  float stringModeNorm = params.block.stringMode[0].Voice()[voice];
  float stringSeedNorm = params.block.stringSeed[0].Voice()[voice];
  float stringPolesNorm = params.block.stringPoles[0].Voice()[voice];

  _stringLPOn = topo.NormalizedToBoolFast(FFOsciParam::StringLPOn, stringLPOnNorm);
  _stringHPOn = topo.NormalizedToBoolFast(FFOsciParam::StringHPOn, stringHPOnNorm);
  _stringSeed = topo.NormalizedToDiscreteFast(FFOsciParam::StringSeed, stringSeedNorm);
  _stringPoles = topo.NormalizedToDiscreteFast(FFOsciParam::StringPoles, stringPolesNorm);
  _stringMode = topo.NormalizedToListFast<FFOsciStringMode>(FFOsciParam::StringMode, stringModeNorm);

  _stringLPFilter.Reset();
  _stringHPFilter.Reset();
  for (int u = 0; u < _uniCount; u++)
  {
    _stringUniState[u].phaseGen = {};
    _stringUniState[u].lastDraw = 0.0f;
    _stringUniState[u].prevDelayVal = 0.0f;
    _stringUniState[u].phaseTowardsX = 0.0f;
    _stringUniState[u].haveNoiseSamples = 0;
    _stringUniState[u].colorFilterPosition = 0;
    _stringUniState[u].dcFilter.SetCoeffs(StringDCBlockFreq, oversampledRate);
    _stringUniState[u].xPrng = FFParkMillerPRNG(_stringSeed / (FFOsciStringMaxSeed + 1.0f));
    _stringUniState[u].uniformPrng = FFParkMillerPRNG(_stringSeed / (FFOsciStringMaxSeed + 1.0f));
    _stringUniState[u].normalPrng = FFMarsagliaPRNG<true>(_stringSeed / (FFOsciStringMaxSeed + 1.0f));
    if (graph)
      _stringUniState[u].delayLine.Reset(_stringUniState[u].delayLine.MaxBufferSize());
    else
      _stringUniState[u].delayLine.Reset(_stringUniState[u].delayLine.MaxBufferSize() * _oversampleTimes / FFOsciOversampleTimes);
    for (int p = 0; p < _stringPoles; p++)
      _stringUniState[u].colorFilterBuffer.Set(p, StringDraw(u));
  }
}

void 
FFOsciProcessor::ProcessString(
  FBModuleProcState& state,
  float coarsePlain0, float finePlain0, float uniDetunePlain0,
  FBSArray<float, FFOsciFixedBlockOversamples> const& voiceBasePitch,
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
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];
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
  FBSArray<float, FFOsciFixedBlockOversamples> stringTrackingKeyPlain = {};
  FBSArray<float, FFOsciFixedBlockOversamples> stringFeedbackKTrkPlain = {};
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
    stringTrackingKeyPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringFeedbackPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringDampKTrkPlain.UpsampleStretch<FFOsciOversampleTimes>();
    stringFeedbackKTrkPlain.UpsampleStretch<FFOsciOversampleTimes>();
  }

  if (_firstProcess)
  {
    // Need access to modulated voice base pitch for this.

    if (_stringLPOn)
    {
      float stringLPFreqPlain0 = FFMultiplyClamp(stringLPFreqPlain.Get(0),
        FFKeyboardTrackingMultiplier(voiceBasePitch.Get(0), stringTrackingKeyPlain.Get(0), stringLPKTrkPlain.Get(0)),
        FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
      stringLPFreqPlain0 *= _graphStVarFilterFreqMultiplier;
      _stringLPFilter.Set(FFStateVariableFilterMode::LPF, oversampledRate, stringLPFreqPlain0, stringLPResPlain.Get(0), 0.0f);
    }

    if (_stringHPOn)
    {
      float stringHPFreqPlain0 = FFMultiplyClamp(stringHPFreqPlain.Get(0),
        FFKeyboardTrackingMultiplier(voiceBasePitch.Get(0), stringTrackingKeyPlain.Get(0), -stringHPKTrkPlain.Get(0)),
        FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
      stringHPFreqPlain0 *= _graphStVarFilterFreqMultiplier;
      _stringHPFilter.Set(FFStateVariableFilterMode::HPF, oversampledRate, stringHPFreqPlain0, stringHPResPlain.Get(0), 0.0f);
    }

    int delayLineSize = static_cast<int>(std::ceil(oversampledRate / FFOsciStringMinFreq));
    for (int u = 0; u < _uniCount; u++)
    {
      float basePitch = voiceBasePitch.Get(0) + coarsePlain0 + finePlain0;
      float uniPitch = basePitch + _uniPosMHalfToHalf.Get(u) * uniDetunePlain0;
      float uniFreq = FBPitchToFreq(uniPitch);
      for (int i = 0; i < delayLineSize; i++)
      {
        double dNextVal = StringNext(u, oversampledRate, uniFreq, stringExcitePlain.Get(0), stringColorPlain.Get(0), stringXPlain.Get(0), stringYPlain.Get(0));
        if (_stringHPOn)
          dNextVal = _stringHPFilter.Next(u, dNextVal);
        if (_stringLPOn)
          dNextVal = _stringLPFilter.Next(u, dNextVal);
        _stringUniState[u].delayLine.Push(static_cast<float>(dNextVal));
      }
    }
  }

  float lastDampNorm = 0.0f;
  float lastFdbkNorm = 0.0f;
  float lastLPFreqPlain = 0.0f;
  float lastHPFreqPlain = 0.0f;
  for (int s = 0; s < totalSamples; s++)
  {
    float x = stringXPlain.Get(s);
    float y = stringYPlain.Get(s);
    float damp = stringDampPlain.Get(s);
    float trackingKey = stringTrackingKeyPlain.Get(s);
    float color = stringColorPlain.Get(s);
    float excite = stringExcitePlain.Get(s);
    float feedback = stringFeedbackPlain.Get(s);
    float basePitch = basePitchPlain.Get(s);
    float dampKTrk = stringDampKTrkPlain.Get(s);
    float uniDetune = uniDetunePlain.Get(s);
    float centerPitch = 60.0f + trackingKey;
    float feedbackKTrk = stringFeedbackKTrkPlain.Get(s);
    float pitchDiffSemis = voiceBasePitch.Get(s) - centerPitch;
    float pitchDiffNorm = std::clamp(pitchDiffSemis / 24.0f, -1.0f, 1.0f);
    damp = std::clamp(damp - 0.5f * dampKTrk * pitchDiffNorm, 0.0f, 1.0f);
    feedback = std::clamp(feedback + 0.5f * feedbackKTrk * pitchDiffNorm, 0.0f, 1.0f);

    float realFeedback = 0.9f + 0.1f * feedback;
    lastDampNorm = damp;
    lastFdbkNorm = feedback;

    if (_stringLPOn)
    {
      float lpRes = stringLPResPlain.Get(s);
      float lpFreq = stringLPFreqPlain.Get(s);
      float lpKTrk = stringLPKTrkPlain.Get(s);
      lpFreq = FFMultiplyClamp(lpFreq,
        FFKeyboardTrackingMultiplier(voiceBasePitch.Get(s), trackingKey, lpKTrk),
        FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
      lpFreq *= _graphStVarFilterFreqMultiplier;
      _stringLPFilter.Set(FFStateVariableFilterMode::LPF, oversampledRate, lpFreq, lpRes, 0.0f);
      lastLPFreqPlain = lpFreq;
    }

    if (_stringHPOn)
    {
      float hpRes = stringHPResPlain.Get(s);
      float hpFreq = stringHPFreqPlain.Get(s);
      float hpKTrk = stringHPKTrkPlain.Get(s);
      hpFreq = FFMultiplyClamp(hpFreq,
        FFKeyboardTrackingMultiplier(voiceBasePitch.Get(s), trackingKey, -hpKTrk),
        FFMinStateVariableFilterFreq, FFMaxStateVariableFilterFreq);
      hpFreq *= _graphStVarFilterFreqMultiplier;
      _stringHPFilter.Set(FFStateVariableFilterMode::HPF, oversampledRate, hpFreq, hpRes, 0.0f);
      lastHPFreqPlain = hpFreq;
    }

    for (int ub = 0; ub < _uniCount; ub += FBSIMDFloatCount)
    {
      auto uniPitchBatch = basePitch + _uniPosMHalfToHalf.Load(ub) * uniDetune;
      auto uniFreqBatch = FBPitchToFreq(uniPitchBatch);
      FBSArray<float, FBSIMDFloatCount> uniFreqArray(uniFreqBatch);
      for (int u = ub; u < ub + FBSIMDFloatCount && u < _uniCount; u++)
      {
        float uniFreq = uniFreqArray.Get(u - ub);
        _stringUniState[u].delayLine.Delay(0, oversampledRate / uniFreq);
        float thisVal = _stringUniState[u].delayLine.GetLinearInterpolate(0);
        _stringUniState[u].delayLine.Pop();
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

        // dont go out of bounds unlimited
        outVal = FFSoftClip10(outVal);
        uniOutputOversampled[u].Set(s, outVal);
      }
    }
    _graphPosition++;
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];

  // Need to translate filter freqs back to normalized because keytracking is applied on plain, not normalized.
  if (_stringLPOn)
    exchangeParams.acc.stringLPFreq[0][voice] = 
      topo.params[(int)FFOsciParam::StringLPFreq].Log2().PlainToNormalizedFast(lastLPFreqPlain);
  if (_stringHPOn)
    exchangeParams.acc.stringHPFreq[0][voice] =
      topo.params[(int)FFOsciParam::StringHPFreq].Log2().PlainToNormalizedFast(lastHPFreqPlain);

  // For these keytracking is applied to normalized.
  exchangeParams.acc.stringDamp[0][voice] = lastDampNorm;
  exchangeParams.acc.stringFeedback[0][voice] = lastFdbkNorm;

  exchangeParams.acc.stringX[0][voice] = stringXNorm.Last();
  exchangeParams.acc.stringY[0][voice] = stringYNorm.Last();
  exchangeParams.acc.stringLPRes[0][voice] = stringLPResNorm.Last();
  exchangeParams.acc.stringHPRes[0][voice] = stringHPResNorm.Last();
  exchangeParams.acc.stringLPKTrk[0][voice] = stringLPKTrkNorm.Last();
  exchangeParams.acc.stringHPKTrk[0][voice] = stringHPKTrkNorm.Last();
  exchangeParams.acc.stringExcite[0][voice] = stringExciteNorm.Last();
  exchangeParams.acc.stringDampKTrk[0][voice] = stringDampKTrkNorm.Last();
  exchangeParams.acc.stringTrackingKey[0][voice] = stringTrackingKeyNorm.Last();
  exchangeParams.acc.stringFeedbackKTrk[0][voice] = stringFeedbackKTrkNorm.Last();
  exchangeParams.acc.stringColor[0][voice] = stringColorNorm.Last();
}