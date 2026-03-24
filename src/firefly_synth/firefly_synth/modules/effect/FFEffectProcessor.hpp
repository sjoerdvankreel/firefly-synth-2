#pragma once

#include <firefly_synth/dsp/shared/FFCombFilter.hpp>
#include <firefly_synth/dsp/shared/FFStateVariableFilter.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;
struct FFEffectExchangeState;

// Show at least 1 sample of the comb filter.
inline float constexpr FFEffectPlotLengthSeconds = 2.0f / FFMinCombFilterFreq;
inline int constexpr FFEffectOversampleFactor = 2;
inline int constexpr FFEffectOversampleTimes = 1 << FFEffectOversampleFactor;
inline int constexpr FFEffectFixedBlockOversamples = FBFixedBlockSamples * FFEffectOversampleTimes;

class FFCompressor
{
public:
  FFCompressor() {}
  ~FFCompressor() {}

  void prepare(float sr)
  {
    sampleRate = sr;

    alphaAttack = 1.0f - timeToGain(attackTime);
    alphaRelease = 1.0f - timeToGain(releaseTime);

    prepared = true;
  }

  void setAttackTime(float attackTimeInSeconds)
  {
    attackTime = attackTimeInSeconds;
    alphaAttack = 1.0f - timeToGain(attackTime);
  }

  void setReleaseTime(float releaseTimeInSeconds)
  {
    releaseTime = releaseTimeInSeconds;
    alphaRelease = 1.0f - timeToGain(releaseTime);
  }

  float timeToGain(float timeInSeconds) { return exp(-1.0f / (sampleRate * timeInSeconds)); }

  void setKnee(float kneeInDecibels)
  {
    knee = kneeInDecibels;
    kneeHalf = knee / 2.0f;
  }

  float getKnee() { return knee; }

  void setThreshold(float thresholdInDecibels) { threshold = thresholdInDecibels; }

  float getTreshold() { return threshold; }

  void setMakeUpGain(float makeUpGainInDecibels) { makeUpGain = makeUpGainInDecibels; }

  float getMakeUpGain() { return makeUpGain; }

  void setRatio(float ratio) { slope = 1.0f / ratio - 1.0f; }

  float getMaxLevelInDecibels() { return maxLevel; }

  void applyCharacteristicToOverShoot(float& overShoot)
  {
    if (overShoot <= -kneeHalf)
      overShoot = 0.0f; //y_G = levelInDecibels;
    else if (overShoot > -kneeHalf && overShoot <= kneeHalf)
      overShoot =
      0.5f * slope * juce::square(overShoot + kneeHalf)
      / knee; //y_G = levelInDecibels + 0.5f * slope * square(overShoot + kneeHalf) / knee;
    else
      overShoot = slope * overShoot;
  }

  void getGainFromSidechainSignal(const float* sideChainSignal,
    float* destination,
    const int numSamples)
  {
    maxLevel = -std::numeric_limits<float>::infinity();
    for (int i = 0; i < numSamples; ++i)
    {
      // convert sample to decibels
      float levelInDecibels = juce::Decibels::gainToDecibels(abs(sideChainSignal[i]));
      if (levelInDecibels > maxLevel)
        maxLevel = levelInDecibels;
      // calculate overshoot and apply knee and ratio
      float overShoot = levelInDecibels - threshold;
      applyCharacteristicToOverShoot(overShoot); //y_G = levelInDecibels + slope * overShoot;

      // ballistics
      const float diff = overShoot - state;
      if (diff < 0.0f)
        state += alphaAttack * diff;
      else
        state += alphaRelease * diff;

      destination[i] = juce::Decibels::decibelsToGain(state + makeUpGain);
    }
  }

  void getGainFromSidechainSignalInDecibelsWithoutMakeUpGain(const float* sideChainSignal,
    float* destination,
    const int numSamples)
  {
    maxLevel = -INFINITY;
    for (int i = 0; i < numSamples; ++i)
    {
      // convert sample to decibels
      float levelInDecibels = juce::Decibels::gainToDecibels(abs(sideChainSignal[i]));
      if (levelInDecibels > maxLevel)
        maxLevel = levelInDecibels;
      // calculate overshoot and apply knee and ratio
      float overShoot = levelInDecibels - threshold;
      applyCharacteristicToOverShoot(overShoot); //y_G = levelInDecibels + slope * overShoot;

      // ballistics
      const float diff = overShoot - state;
      if (diff < 0.0f)
        state += alphaAttack * diff;
      else
        state += alphaRelease * diff;

      destination[i] = state;
    }
  }

  void getCharacteristic(float* inputLevels, float* dest, const int numSamples)
  {
    for (int i = 0; i < numSamples; ++i)
    {
      dest[i] = getCharacteristicSample(inputLevels[i]);
    }
  }

  inline float getCharacteristicSample(float inputLevel)
  {
    float overShoot = inputLevel - threshold;
    applyCharacteristicToOverShoot(overShoot);
    return overShoot + inputLevel + makeUpGain;
  }

private:
  float sampleRate{ 0.0f };
  bool prepared;

  float knee{ 0.0f }, kneeHalf{ 0.0f };
  float threshold{ -10.0f };
  float attackTime{ 0.01f };
  float releaseTime{ 0.15f };
  float slope{ 0.0f };
  float makeUpGain{ 0.0f };

  float maxLevel{ -std::numeric_limits<float>::infinity() };

  //state juce::variable
  float state{ 0.0f };

  float alphaAttack;
  float alphaRelease;
};


class FFEffectProcessor final
{
  bool _on = {};
  int _oversampleTimes = {};

  std::array<FFEffectKind, FFEffectBlockCount> _kind = {};
  std::array<FFEffectClipMode, FFEffectBlockCount> _clipMode = {};
  std::array<FFEffectFoldMode, FFEffectBlockCount> _foldMode = {};
  std::array<FFEffectSkewMode, FFEffectBlockCount> _skewMode = {};
  std::array<FFEffectFilterMode, FFEffectBlockCount> _filterMode = {};
  std::array<FFCompressor, FFEffectBlockCount> _compressors = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};
  float _graphCombFilterFreqMultiplier = {};
  float _graphStVarFilterFreqMultiplier = {};

  juce::dsp::Oversampling<float> _oversampler;
  FBBasicLPFilter _basePitchSmoother = {};
  FBSArray<float, FFEffectFixedBlockOversamples> _basePitch = {};
  std::array<FFCombFilter<2>, FFEffectBlockCount> _combFilters = {};
  std::array<FFStateVariableFilter<2>, FFEffectBlockCount> _stVarFilters = {};

  template <bool Global>
  FBBatch<float> NextBasePitchBatch(int pos);

  void ProcessCompress(
    int block, bool global, FBModuleProcState const& state,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled);

  template <bool PlusOn, bool MinOn>
  void ProcessComb(
    int block, float oversampledRate,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combResMinPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combResPlusPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combFreqMinPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combFreqPlusPlain);

  void ProcessStVar(
    int block, float oversampledRate,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarResPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarFreqPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarGainPlain);

  void ProcessSkew(
    int block,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain);

  void ProcessClip(
    int block,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain);

  void ProcessFold(
    int block,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain);

public:
  FFEffectProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEffectProcessor);

  template <bool Global>
  void AllocOnDemandBuffers(
    FBRuntimeTopo const* topo, FBProcStateContainer* state,
    int moduleSlot, bool graph, float sampleRate);
  void ReleaseOnDemandBuffers(
    FBRuntimeTopo const* topo, FBProcStateContainer* state);

  template <bool Global>
  void BeginVoiceOrBlock(
    FBModuleProcState& state, 
    bool graph, bool detailGraphs,
    int graphIndex, int graphSampleCount);
  template <bool Global>
  int Process(
    FBModuleProcState& state,
    FFEffectExchangeState const* exchangeFromDSP,
    bool graph);
};