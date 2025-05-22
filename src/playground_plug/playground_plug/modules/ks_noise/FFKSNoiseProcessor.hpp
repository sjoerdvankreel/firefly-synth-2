#pragma once

#include <playground_plug/modules/ks_noise/FFKSNoiseTopo.hpp>
#include <playground_plug/modules/ks_noise/FFKSNoisePhaseGenerator.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBDelayLine.hpp>
#include <playground_base/dsp/shared/FBCytomicFilter.hpp>
#include <playground_base/dsp/shared/FBBasicHPFilter.hpp>
#include <playground_base/dsp/shared/FBMarsagliaPRNG.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

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

  float _lastDraw = 0.0f;
  float _prevDelayVal = 0.f;
  float _phaseTowardsX = 0.0f;
  int _graphPosition = 0;
  int _colorFilterPosition = 0;

  FBDelayLine _delayLine = {};
  FBBasicHPFilter _dcFilter = {};
  FBCytomicFilter<1> _lpFilter = {};
  FBCytomicFilter<1> _hpFilter = {};
  FBMarsagliaPRNG _normalPrng = {};
  FBParkMillerPRNG _uniformPrng = {};
  FFKSNoisePhaseGenerator _phaseGen = {};
  FBSArray<float, FFKSNoiseMaxPoles> _colorFilterBuffer = {};

  float Draw();
  float Filter(
    float sampleRate, float lpHz, 
    float hpHz, float val);
  float Next(
    FBStaticModule const& topo,
    float sampleRate, float baseFreq, 
    float excite, float colorNorm, 
    float xNorm, float yNorm);

public:
  FFKSNoiseProcessor();
  int Process(FBModuleProcState& state);
  void Initialize(float sampleRate);
  void BeginVoice(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFKSNoiseProcessor);
};