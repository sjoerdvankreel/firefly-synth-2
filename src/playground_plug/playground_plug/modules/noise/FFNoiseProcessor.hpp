#pragma once

#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBMarsagliaPRNG.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFNoiseProcessor final:
public FFOsciProcessorBase
{
  bool _on = {};
  int _seed = {};
  int _poles = {};

  float _baseFreq = 0.0f; // todo remove
  int _totalPosition = 0;
  int _bufferPosition = 0;
  FBMarsagliaPRNG _prng = {};
  FBSIMDArray<float, FFNoiseMaxPoles> _w = {};
  FBSIMDArray<float, FFNoiseMaxPoles> _x = {};

public:
  FFNoiseProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFNoiseProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};