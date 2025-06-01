#pragma once

#include <playground_plug/dsp/shared/FFParkMillerPRNG.hpp>
#include <playground_plug/dsp/shared/FFTrackingPhaseGenerator.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciPhaseGenerator.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>
#include <playground_plug/modules/osci_base/FFOsciProcessorBase.hpp>

#include <playground_base/base/shared/FBUtility.hpp>
#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFOsciProcessor final:
public FFOsciProcessorBase
{
  FFOsciType _type = {};
  int _oversampleTimes = {};
  float _uniOffsetPlain = {};
  float _uniRandomPlain = {};

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

  bool _modMatrixExpoFM = false;
  std::array<bool, FFOsciCount - 1> _modSourceFMOn = {};
  std::array<int, FFOsciCount - 1> _modSourceUniCount = {};
  std::array<FFOsciModAMMode, FFOsciCount - 1> _modSourceAMMode = {};

  juce::dsp::Oversampling<float> _oversampler;
  juce::dsp::AudioBlock<float> _oversampledBlock = {};
  juce::dsp::AudioBlock<float> _downsampledBlock = {};
  std::array<float*, FFOsciBaseUniMaxCount> _downsampledChannelPtrs = {};

  FFParkMillerPRNG _prng = {};
  FFTrackingPhaseGenerator _phaseGen = {};
  std::array<FFOsciPhaseGenerator, FFOsciBaseUniMaxCount> _uniPhaseGens = {};
  FBSArray2<float, FFOsciBaseUniMaxCount, FFOsciFMOperatorCount> _prevUniFMOutput = {};
  std::array<std::array<FFOsciFMPhaseGenerator, FFOsciBaseUniMaxCount / FBSIMDFloatCount>, FFOsciFMOperatorCount> _uniFMPhaseGens = {};

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(bool graph, FBModuleProcState& state);
};