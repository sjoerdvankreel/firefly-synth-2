#pragma once

#include <playground_plug/modules/ks_noise/FFKSNoiseTopo.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoisePhaseGenerator.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBMarsagliaPRNG.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>

#include <array>
#include <vector>

class FBAccParamState;
struct FBModuleProcState;

class FFKSNoiseProcessor final:
public FFOsciProcessorBase
{
  int _seed = {};
  int _poles = {};
  FFKSNoiseType _type = {};

  int _graphPosition = 0;
  float _lastDraw = 0.0f;
  float _phaseTowardsX = 0.0f;
  
  FBMarsagliaPRNG _normalPrng = {};
  FBParkMillerPRNG _uniformPrng = {};
  FFKSNoisePhaseGenerator _phaseGen = {};

  int _waveTableSize = 0;
  int _prevWaveTablePos1 = 0;
  int _colorFilterPosition = 0;
  std::vector<float> _waveTableBuffer = {};
  FBSIMDArray<float, FFKSNoiseMaxPoles> _colorFilterBuffer = {};

  float Draw();
  float Next(
    FBStaticModule const& topo,
    float sampleRate, float baseFreq,
    float colorNorm, float xNorm, float yNorm);

public:
  FFKSNoiseProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFKSNoiseProcessor);

  int Process(FBModuleProcState& state);
  void AllocateBuffers(float sampleRate);
  void BeginVoice(FBModuleProcState& state);
};