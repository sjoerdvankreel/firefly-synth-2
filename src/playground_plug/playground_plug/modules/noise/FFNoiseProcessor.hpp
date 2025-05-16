#pragma once

#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_plug/modules/noise/FFNoisePhaseGenerator.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBMarsagliaPRNG.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

inline int constexpr FFNoiseCorrectionBufferSize = 16384;

class FFNoiseProcessor final:
public FFOsciProcessorBase
{
  int _seed = {};
  int _poles = {};
  FFNoiseType _type = {};

  int _graphPosition = 0;
  int _historyPosition = 0;
  int _correctionPosition = 0;
  float _lastDraw = 0.0f;
  float _correctionMax = 1.0f;
  float _correctionTotal = 0.0f;
  float _phaseIncremented = 0.0f;
  FBMarsagliaPRNG _normalPrng = {};
  FBParkMillerPRNG _uniformPrng = {};
  FFNoisePhaseGenerator _phaseGen = {};
  FBSIMDArray<float, FFNoiseMaxPoles> _historyBuffer = {};
  FBSIMDArray<float, FFNoiseCorrectionBufferSize> _correctionBuffer = {};

  float Draw();

public:
  FFNoiseProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFNoiseProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};

inline float
FFNoiseProcessor::Draw()
{
  if (_type == FFNoiseType::Norm)
    return _normalPrng.NextScalar();
  else
    return _uniformPrng.NextScalar();
  assert(false);
  return 0.0f;
}