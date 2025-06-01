#pragma once

#include <playground_plug/dsp/shared/FFStateVariableFilter.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

inline int constexpr EffectOversampleFactor = 2;
inline int constexpr EffectOversampleTimes = 1 << EffectOversampleFactor;
inline int constexpr EffectFixedBlockOversamples = FBFixedBlockSamples * EffectOversampleTimes;

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
  juce::dsp::Oversampling<float> _oversampler;
  std::array<FFStateVariableFilter<2>, FFEffectBlockCount> _stVarFilters = {};

  void ProcessStVar(
    int block, float oversampledRate,
    FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
    FBSArray<float, EffectFixedBlockOversamples> const& trackingKeyPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarResPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarFreqPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarGainPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& stVarKeyTrkPlain);
  void ProcessSkew(
    int block,
    FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain);
  void ProcessClip(
    int block,
    FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain);
  void ProcessFold(
    int block,
    FBSArray2<float, EffectFixedBlockOversamples, 2>& oversampled,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distAmtPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distMixPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distBiasPlain,
    FBSArray2<float, EffectFixedBlockOversamples, FFEffectBlockCount> const& distDrivePlain);

public:
  FFEffectProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFEffectProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);
};