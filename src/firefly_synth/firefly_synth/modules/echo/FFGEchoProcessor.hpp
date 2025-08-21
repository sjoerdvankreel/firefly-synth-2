#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

inline float constexpr FFGEchoPlotLengthSeconds = 5.0f;

struct FFGEchoDelayState
{
  void Reset();
  FFStateVariableFilter<2> lpFilter = {};
  FFStateVariableFilter<2> hpFilter = {};
  std::array<FFDelayLine, 2> delayLine = {};
};

class FFGEchoProcessor final
{
  bool _sync = {};
  FFGEchoTarget _target = {};
  FFGEchoFeedbackType _feedbackType = {};
  float _feedbackDelayBarsSamples = {};
  std::array<bool, FFGEchoTapCount> _tapOn = {};
  std::array<float, FFGEchoTapCount> _tapDelayBarsSamples = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};
  float _graphStVarFilterFreqMultiplier = {};

  FBBasicLPFilter _feedbackDelayTimeSmoother = {};
  FFGEchoDelayState _feedbackDelayGlobalState = {};
  std::array<FFGEchoDelayState, FFGEchoTapCount> _feedbackDelayPerTapStates = {};
  std::array<FFGEchoDelayState, FFGEchoTapCount> _tapDelayStates = {};
  std::array<FBBasicLPFilter, FFGEchoTapCount> _tapDelayTimeSmoothers = {};

  void ProcessTaps(
    FBModuleProcState& state,
    FBSArray2<float, FBFixedBlockSamples, 2>& inout,
    bool processAudioOrExchangeState);

  void ProcessFeedback(
    FBModuleProcState& state,
    FFGEchoDelayState& delayState,
    FBSArray2<float, FBFixedBlockSamples, 2>& inout,
    bool processAudioOrExchangeState);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoProcessor);

  int Process(FBModuleProcState& state);
  void BeginBlock(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);
  void ReleaseOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state);
  void AllocOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state, float sampleRate);
};