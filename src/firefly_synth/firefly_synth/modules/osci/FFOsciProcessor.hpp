#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/dsp/shared/FFBasicHPFilter.hpp>
#include <firefly_synth/dsp/shared/FFMarsagliaPRNG.hpp>
#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>
#include <firefly_synth/dsp/shared/FFStateVariableFilter.hpp>
#include <firefly_synth/dsp/shared/FFTrackingPhaseGenerator.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciPhaseGenerator.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModTopo.hpp>
#include <firefly_synth/modules/shared/FFOsciProcessorBase.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

struct FFOsciStringUniVoiceState final
{
  float lastDraw = 0.0f;
  float prevDelayVal = 0.f;
  float phaseTowardsX = 0.0f;
  int colorFilterPosition = 0;

  FFDelayLine delayLine = {};
  FFBasicHPFilter dcFilter = {};
  FFOsciStringPhaseGenerator phaseGen = {};
  FBSArray<float, FFOsciStringMaxPoles> colorFilterBuffer = {};
};

class FFOsciProcessor final:
public FFOsciProcessorBase
{
  FFOsciType _type = {};
  int _oversampleTimes = {};
  float _uniOffsetPlain = {};
  float _uniRandomPlain = {};

  int _stringSeed = {};
  int _stringPoles = {};
  bool _stringLPOn = {};
  bool _stringHPOn = {};
  FFOsciStringMode _stringMode = {};

  FFOsciFMMode _fmMode = {};
  float _fmRatioRatio12 = {};
  float _fmRatioRatio23 = {};
  FFOsciFMRatioMode _fmRatioMode = {};

  float _waveDSFOver = {};
  float _waveDSFBWPlain = {};
  float _waveDSFDistance = {};
  FFOsciWaveDSFMode _waveDSFMode = {};
  FFOsciWaveHSMode _waveHSMode = {};
  std::array<FFOsciWavePWMode, FFOsciWavePWCount> _wavePWMode = {};
  std::array<FFOsciWaveBasicMode, FFOsciWaveBasicCount> _waveBasicMode = {};

  int _graphPosition = {};
  float _graphStVarFilterFreqMultiplier = {};

  bool _modMatrixExpoFM = false;
  std::array<bool, FFOsciCount - 1> _modSourceFMOn = {};
  std::array<int, FFOsciCount - 1> _modSourceUniCount = {};
  std::array<FFOsciModAMMode, FFOsciCount - 1> _modSourceAMMode = {};

  juce::dsp::Oversampling<float> _oversampler;
  juce::dsp::AudioBlock<float> _oversampledBlock = {};
  juce::dsp::AudioBlock<float> _downsampledBlock = {};
  std::array<float*, FFOsciBaseUniMaxCount> _downsampledChannelPtrs = {};

  FFParkMillerPRNG _uniformPrng = {};
  FFMarsagliaPRNG _stringNormalPrng = {};
  FFStateVariableFilter<FFOsciBaseUniMaxCount> _stringLPFilter = {};
  FFStateVariableFilter<FFOsciBaseUniMaxCount> _stringHPFilter = {};
  std::array<FFOsciStringUniVoiceState, FFOsciBaseUniMaxCount> _stringUniState = {};

  FFTrackingPhaseGenerator _phaseGen = {};
  std::array<FFOsciWavePhaseGenerator, FFOsciBaseUniMaxCount> _uniWavePhaseGens = {};
  FBSArray2<float, FFOsciBaseUniMaxCount, FFOsciFMOperatorCount> _prevUniFMOutput = {};
  std::array<std::array<FFOsciFMPhaseGenerator, FFOsciBaseUniMaxCount / FBSIMDFloatCount>, FFOsciFMOperatorCount> _uniFMPhaseGens = {};

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(bool graph, FBModuleProcState& state);
};