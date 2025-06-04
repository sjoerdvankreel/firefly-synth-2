#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/dsp/shared/FFBasicHPFilter.hpp>
#include <firefly_synth/dsp/shared/FFMarsagliaPRNG.hpp>
#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>
#include <firefly_synth/dsp/shared/FFStateVariableFilter.hpp>
#include <firefly_synth/modules/osci_base/FFOsciProcessorBase.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciTopo.hpp>
#include <firefly_synth/modules/string_osci/FFStringOsciPhaseGenerator.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>
#include <vector>

class FBAccParamState;
struct FBModuleProcState;

struct FFStringOsciUniVoiceState final
{
  float lastDraw = 0.0f;
  float prevDelayVal = 0.f;
  float phaseTowardsX = 0.0f;
  int colorFilterPosition = 0;

  FFDelayLine delayLine = {};
  FFBasicHPFilter dcFilter = {};
  FFStringOsciPhaseGenerator phaseGen = {};
  FBSArray<float, FFStringOsciMaxPoles> colorFilterBuffer = {};
};

class FFStringOsciProcessor final:
public FFOsciProcessorBase
{
  int _seed = {};
  int _poles = {};
  int _graphPosition = {};
  float _graphStVarFilterFreqMultiplier = {};

  FFStringOsciType _type = {};
  FFMarsagliaPRNG _normalPrng = {};
  FFParkMillerPRNG _uniformPrng = {};
  FFStateVariableFilter<FFOsciBaseUniMaxCount> _lpFilter = {};
  FFStateVariableFilter<FFOsciBaseUniMaxCount> _hpFilter = {};
  std::array<FFStringOsciUniVoiceState, FFOsciBaseUniMaxCount> _uniState = {};

  float Draw();
  float Next(
    FBStaticModule const& topo, int uniVoice,
    float sampleRate, float uniFreq, 
    float excite, float colorNorm, 
    float xNorm, float yNorm);

public:
  FFStringOsciProcessor();
  int Process(FBModuleProcState& state);
  void InitializeBuffers(bool graph, float sampleRate);
  void BeginVoice(bool graph, FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFStringOsciProcessor);
};