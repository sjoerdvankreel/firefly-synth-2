#pragma once

#include <playground_plug/modules/noise/FFNoiseTopo.hpp>
#include <playground_plug/modules/noise/FFNoisePhaseGenerator.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBMarsagliaPRNG.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>

#include <array>
#include <vector>

class FBAccParamState;
struct FBModuleProcState;

class FFNoiseProcessor final:
public FFOsciProcessorBase
{
  int _seed = {};
  int _poles = {};
  FFNoiseType _type = {};

  int _graphPosition = 0;
  float _lastDraw = 0.0f;
  float _phaseTowardsX = 0.0f;
  
  FBMarsagliaPRNG _normalPrng = {};
  FBParkMillerPRNG _uniformPrng = {};
  FFNoisePhaseGenerator _phaseGen = {};

  int _waveTableSize = 0;
  int _colorFilterPosition = 0;
  std::vector<float> _waveTableBuffer = {};
  FBSIMDArray<float, FFNoiseMaxPoles> _colorFilterBuffer = {};

  float Draw();
  float Next(
    FBStaticModule const& topo,
    float sampleRate, float baseFreq,
    float colorNorm, float xNorm, float yNorm);

public:
  FFNoiseProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFNoiseProcessor);

  int Process(FBModuleProcState& state);
  void AllocateBuffers(float sampleRate);
  void BeginVoice(FBModuleProcState& state);
};