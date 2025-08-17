#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFGEchoProcessor final
{
  bool _sync = {};
  bool _reverbLPOn = {};
  bool _reverbHPOn = {};
  FFGEchoReverbPlacement _reverbPlacement = {};
  std::array<bool, FFGEchoTapCount> _tapOn = {};
  std::array<bool, FFGEchoTapCount> _tapLPOn = {};
  std::array<bool, FFGEchoTapCount> _tapHPOn = {};
  std::array<bool, FFGEchoTapCount> _tapFBLPOn = {};
  std::array<bool, FFGEchoTapCount> _tapFBHPOn = {};
  std::array<int, FFGEchoTapCount> _tapDelayBarsSamples = {};
  std::array<int, FFGEchoTapCount> _tapLengthBarsSamples = {};
  std::array<std::array<FFDelayLine, 2>, FFGEchoTapCount> _delayLines = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGEchoProcessor);
  void InitializeBuffers(float sampleRate);

  void BeginBlock(FBModuleProcState& state);
  void Process(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout);
};