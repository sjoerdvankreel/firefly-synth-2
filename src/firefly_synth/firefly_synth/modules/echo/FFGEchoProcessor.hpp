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
  bool _reverbLPOn = {};
  bool _reverbHPOn = {};
  FFGEchoTarget _target = {};
  FFGEchoReverbPlacement _reverbPlacement = {};
  std::array<bool, FFGEchoTapCount> _tapOn = {};
  std::array<bool, FFGEchoTapCount> _tapLPOn = {};
  std::array<bool, FFGEchoTapCount> _tapHPOn = {};
  std::array<bool, FFGEchoTapCount> _tapFBLPOn = {};
  std::array<bool, FFGEchoTapCount> _tapFBHPOn = {};
  std::array<int, FFGEchoTapCount> _tapDelaySamples = {};
  std::array<float, FFGEchoTapCount> _tapLengthBarsSamples = {};

  // https://forum.juce.com/t/delay-line-artifacts/46781/4
  int _preDelayBufferPosition = {};
  FBBasicLPFilter _delayTimeSmoother = {};
  std::array<std::vector<float>, 2> _preDelayBuffer = {};
  std::array<std::array<FFDelayLine, 2>, FFGEchoTapCount> _delayLines = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoProcessor);
  void InitializeBuffers(float sampleRate);

  void BeginBlock(FBModuleProcState& state);
  void Process(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout);
};