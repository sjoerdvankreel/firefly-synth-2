#pragma once

#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFNoiseProcessor final
{
  float _key = {};
  bool _on = {};
  int _uniCount = {};
  int _seed = {};
  int _q = {};
  FBParkMillerPRNG _prng = {};

public:
  FFNoiseProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFNoiseProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};