#pragma once

#include <playground_plug/dsp/shared/FFDSPUtility.hpp>
#include <playground_plug/dsp/shared/FFCombFilter.hpp>
#include <playground_plug/dsp/shared/FFStateVariableFilter.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

// Show at least 1 sample of the comb filter.
inline float constexpr FFEffectPlotLengthSeconds = 2.0f / FFMinFilterFreq;
inline int constexpr FFEffectOversampleFactor = 2;
inline int constexpr FFEffectOversampleTimes = 1 << FFEffectOversampleFactor;
inline int constexpr FFEffectFixedBlockOversamples = FBFixedBlockSamples * FFEffectOversampleTimes;

class FFEffectProcessor final
{
  bool _on = {};
  float _key = {};
  int _oversampleTimes = {};
  std::array<FFEffectKind, FFEffectBlockCount> _kind = {};
  std::array<FFEffectClipMode, FFEffectBlockCount> _clipMode = {};
  std::array<FFEffectFoldMode, FFEffectBlockCount> _foldMode = {};
  std::array<FFEffectSkewMode, FFEffectBlockCount> _skewMode = {};
  std::array<FFStateVariableFilterMode, FFEffectBlockCount> _stVarMode = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};
  float _graphFilterFreqMultiplier = {};
  juce::dsp::Oversampling<float> _oversampler;
  std::array<FFCombFilter<2>, FFEffectBlockCount> _combFilters = {};
  std::array<FFStateVariableFilter<2>, FFEffectBlockCount> _stVarFilters = {};

  void ProcessComb(
    int block, float oversampledRate,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
    FBSArray<float, FFEffectFixedBlockOversamples> const& trackingKeyPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combKeyTrkPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combResMinPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combResPlusPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combFreqMinPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& combFreqPlusPlain);

  void ProcessStVar(
    int block, float oversampledRate,
    FBSArray2<float, FFEffectFixedBlockOversamples, 2>& oversampled,
    FBSArray<float, FFEffectFixedBlockOversamples> const& trackingKeyPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarResPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarFreqPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarGainPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& stVarKeyTrkPlain);

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
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
    FBSArray2<float, FFEffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain);

public:
  FFEffectProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEffectProcessor);
  int Process(FBModuleProcState& state);
  void InitializeBuffers(float sampleRate);
  void BeginVoice(bool graph, int graphIndex, int graphSampleCount, float dspSampleRate, FBModuleProcState& state);
};