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

class FFEffectProcessor final
{
  bool _on = {};
  int _oversampleTimes = {};
  std::array<float, FFEffectBlockCount> _compRatio = {};
  std::array<float, FFEffectBlockCount> _compKneeDb = {};
  std::array<float, FFEffectBlockCount> _compEnvStateDb = {};
  std::array<float, FFEffectBlockCount> _compThresholdDb = {};
  std::array<float, FFEffectBlockCount> _compEnvCoeffAttack = {};
  std::array<float, FFEffectBlockCount> _compEnvCoeffRelease = {};

#if false
  // https://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c
  std::array<float, FFEffectBlockCount> _compRMSTotal = {};
  std::array<int, FFEffectBlockCount> _compRMSWindowsPos = {};
  std::array<std::vector<float>, FFEffectBlockCount> _compRMSWindows = {};

  std::array<float, FFEffectBlockCount> _prevCompRMSSize = {};
  std::array<FFEffectCompMode, FFEffectBlockCount> _prevCompMode = {};
  std::array<FFGEffectCompSide, FFEffectBlockCount> _prevGCompSide = {};

  std::array<float, FFEffectBlockCount> _compAttack = {};
  std::array<float, FFEffectBlockCount> _compRelease = {};
  std::array<float, FFEffectBlockCount> _compRMSSize = {};
  std::array<FFEffectCompMode, FFEffectBlockCount> _compMode = {};
  std::array<FFVEffectCompSide, FFEffectBlockCount> _vCompSide = {};
  std::array<FFGEffectCompSide, FFEffectBlockCount> _gCompSide = {};
#endif

  std::array<FFEffectKind, FFEffectBlockCount> _kind = {};
  std::array<FFEffectClipMode, FFEffectBlockCount> _clipMode = {};
  std::array<FFEffectFoldMode, FFEffectBlockCount> _foldMode = {};
  std::array<FFEffectSkewMode, FFEffectBlockCount> _skewMode = {};
  std::array<FFEffectFilterMode, FFEffectBlockCount> _filterMode = {};

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