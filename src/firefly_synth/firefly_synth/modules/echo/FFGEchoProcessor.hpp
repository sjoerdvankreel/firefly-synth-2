#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

struct FFGEchoDelayState
{
  FFStateVariableFilter<2> lpFilter = {};
  FFStateVariableFilter<2> hpFilter = {};
  std::array<FFDelayLine, 2> delayLine = {};
};

class FFGEchoProcessor final
{
  bool _sync = {};
  FFGEchoOrder _order = {}; // todo
  FFGEchoTarget _target = {};
  FFGEchoFeedbackType _feedbackType = {};
  float _feedbackDelayBarsSamples = {};
  std::array<bool, FFGEchoTapCount> _tapOn = {};
  std::array<float, FFGEchoTapCount> _tapDelayBarsSamples = {};

  FBBasicLPFilter _feedbackDelayTimeSmoother = {};
  FFGEchoDelayState _feedbackDelayGlobalState = {};
  std::array<FFGEchoDelayState, FFGEchoTapCount> _feedbackDelayPerTapStates = {};
  std::array<FFGEchoDelayState, FFGEchoTapCount> _tapDelayStates = {};
  std::array<FBBasicLPFilter, FFGEchoTapCount> _tapDelayTimeSmoothers = {};

  void ProcessTaps(
    FBModuleProcState& state,
    bool reverbAfterFeedback,
    bool processAudioOrExchangeState,
    FBSArray2<float, FBFixedBlockSamples, 2>& inout);

  void ProcessFeedback(
    FBModuleProcState& state,
    FFGEchoDelayState& delayState,
    bool mixInDry,
    bool processAudioOrExchangeState,
    FBSArray2<float, FBFixedBlockSamples, 2>& inout);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoProcessor);

  void BeginBlock(FBModuleProcState& state);
  void Process(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout);
  void ReleaseOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state);
  void AllocOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state, float sampleRate);
};