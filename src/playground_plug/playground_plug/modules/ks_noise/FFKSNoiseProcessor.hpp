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

struct FFKSNoiseUniVoiceState final
{
  float lastDraw = 0.0f;
  float prevDelayVal = 0.f;
  float phaseTowardsX = 0.0f;
  int colorFilterPosition = 0;

  FBDelayLine delayLine = {};
  FBBasicHPFilter dcFilter = {};
  FBCytomicFilter<1> lpFilter = {};
  FBCytomicFilter<1> hpFilter = {};
  FFKSNoisePhaseGenerator phaseGen = {};
  FBSArray<float, FFKSNoiseMaxPoles> colorFilterBuffer = {};
};

class FFKSNoiseProcessor final:
public FFOsciProcessorBase
{
  int _seed = {};
  int _poles = {};
  int _graphPosition = {};
  FFKSNoiseType _type = {};

  FBMarsagliaPRNG _normalPrng = {};
  FBParkMillerPRNG _uniformPrng = {};
  std::array<FFKSNoiseUniVoiceState, FFOsciBaseUniMaxCount> _uniState = {};

  float Draw();
  float Next(
    FBStaticModule const& topo, int uniVoice,
    float sampleRate, float uniFreq, 
    float excite, float colorNorm, 
    float xNorm, float yNorm,
    float lpFreqHz, float hpFreqHz);

public:
  FFKSNoiseProcessor();
  int Process(FBModuleProcState& state);
  void Initialize(float sampleRate);
  void BeginVoice(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFKSNoiseProcessor);
};