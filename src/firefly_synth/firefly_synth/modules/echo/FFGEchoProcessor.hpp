#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

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
  FFStateVariableFilter<2> _feedbackLPFilter = {};
  FFStateVariableFilter<2> _feedbackHPFilter = {};
  std::array<FFDelayLine, 2> _feedbackDelayLine = {};

  std::array<FBBasicLPFilter, FFGEchoTapCount> _tapDelayTimeSmoothers = {};
  std::array<FFStateVariableFilter<2>, FFGEchoTapCount> _tapLPFilters = {};
  std::array<FFStateVariableFilter<2>, FFGEchoTapCount> _tapHPFilters = {};
  std::array<std::array<FFDelayLine, 2>, FFGEchoTapCount> _tapDelayLines = {};

  void ProcessTaps(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout);
  void ProcessFeedback(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoProcessor);

  void BeginBlock(FBModuleProcState& state);
  void Process(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout);
  void ReleaseOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state);
  void AllocOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state, float sampleRate);
};