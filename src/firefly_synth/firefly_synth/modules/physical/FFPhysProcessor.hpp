#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/dsp/shared/FFBasicHPFilter.hpp>
#include <firefly_synth/dsp/shared/FFMarsagliaPRNG.hpp>
#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>
#include <firefly_synth/dsp/shared/FFStateVariableFilter.hpp>
#include <firefly_synth/modules/physical/FFPhysTopo.hpp>
#include <firefly_synth/modules/physical/FFPhysPhaseGenerator.hpp>
#include <firefly_synth/modules/osci_base/FFOsciProcessorBase.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
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
  FFStateVariableFilter<FFOsciBaseUniMaxCount> _lpFilter = {};
  FFStateVariableFilter<FFOsciBaseUniMaxCount> _hpFilter = {};
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
  void InitializeBuffers(bool graph, float sampleRate);
  void BeginVoice(bool graph, FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPhysProcessor);
};