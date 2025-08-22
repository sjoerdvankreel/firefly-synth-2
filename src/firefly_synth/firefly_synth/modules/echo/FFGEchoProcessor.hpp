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
  FBBasicLPFilter smoother = {};
  FFStateVariableFilter<2> lpFilter = {};
  FFStateVariableFilter<2> hpFilter = {};
  std::array<FFDelayLine, 2> delayLine = {};
};

class FFGEchoProcessor final
{
  bool _sync = {};
  bool _tapsOn = {};
  bool _feedbackOn = {};
  FFGEchoTarget _target = {};
  float _feedbackDelayBarsSamples = {};
  std::array<bool, FFGEchoTapCount> _tapOn = {};
  std::array<float, FFGEchoTapCount> _tapDelayBarsSamples = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};
  float _graphStVarFilterFreqMultiplier = {};

  FFGEchoDelayState _feedbackDelayState = {};
  std::array<FFGEchoDelayState, FFGEchoTapCount> _tapDelayStates = {};

  void ProcessTaps(
    FBModuleProcState& state,
    bool processAudioOrExchangeState);

  void ProcessFeedback(
    FBModuleProcState& state,
    bool processAudioOrExchangeState);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoProcessor);

  int Process(FBModuleProcState& state);
  void BeginBlock(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);
  void ReleaseOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state);
  void AllocOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state, bool graph, float sampleRate);
};