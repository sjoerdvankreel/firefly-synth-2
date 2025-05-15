#pragma once

#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBMarsagliaPRNG.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

inline int constexpr FFNoiseCorrectionBufferSize = 16384;

class FFNoiseProcessor final:
public FFOsciProcessorBase
{
  bool _on = {};
  int _seed = {};
  int _poles = {};

  float _baseFreq = 0.0f; // todo remove
  float _correctionTotal = 0.0f;
  int _totalPosition = 0;
  int _historyPosition = 0;
  int _correctionPosition = 0;
  FBMarsagliaPRNG _prng = {};
  FBSIMDArray<float, FFNoiseMaxPoles> _historyBuffer = {};
  FBSIMDArray<float, FFNoiseCorrectionBufferSize> _correctionBuffer = {};

public:
  FFNoiseProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFNoiseProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};