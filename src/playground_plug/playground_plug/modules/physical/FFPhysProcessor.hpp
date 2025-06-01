#pragma once

#include <playground_plug/dsp/shared/FFDelayLine.hpp>
#include <playground_plug/dsp/shared/FFCytomicFilter.hpp>
#include <playground_plug/dsp/shared/FFBasicHPFilter.hpp>
#include <playground_plug/dsp/shared/FFMarsagliaPRNG.hpp>
#include <playground_plug/dsp/shared/FFParkMillerPRNG.hpp>
#include <playground_plug/modules/physical/FFPhysTopo.hpp>
#include <playground_plug/modules/physical/FFPhysPhaseGenerator.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <array>
#include <vector>

class FBAccParamState;
struct FBModuleProcState;

struct FFPhysUniVoiceState final
{
  float lastDraw = 0.0f;
  float prevDelayVal = 0.f;
  float phaseTowardsX = 0.0f;
  int colorFilterPosition = 0;

  FFDelayLine delayLine = {};
  FFBasicHPFilter dcFilter = {};
  FFPhysPhaseGenerator phaseGen = {};
  FBSArray<float, FFPhysMaxPoles> colorFilterBuffer = {};
};

class FFPhysProcessor final:
public FFOsciProcessorBase
{
  int _seed = {};
  int _poles = {};
  int _graphPosition = {};

  FFPhysType _type = {};
  FFMarsagliaPRNG _normalPrng = {};
  FFParkMillerPRNG _uniformPrng = {};
  FBCytomicFilter<FFOsciBaseUniMaxCount> _lpFilter = {};
  FBCytomicFilter<FFOsciBaseUniMaxCount> _hpFilter = {};
  std::array<FFPhysUniVoiceState, FFOsciBaseUniMaxCount> _uniState = {};

  float Draw();
  float Next(
    FBStaticModule const& topo, int uniVoice,
    float sampleRate, float uniFreq, 
    float excite, float colorNorm, 
    float xNorm, float yNorm);

public:
  FFPhysProcessor();
  int Process(FBModuleProcState& state);
  void Initialize(bool graph, float sampleRate);
  void BeginVoice(bool graph, FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPhysProcessor);
};