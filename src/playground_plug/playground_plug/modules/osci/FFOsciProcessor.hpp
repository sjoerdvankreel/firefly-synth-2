#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciPhaseGenerator.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>

#include <playground_base/base/shared/FBMDArray.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>
#include <playground_base/dsp/shared/FBTrackingPhaseGenerator.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFOsciProcessor final
{
  float _key = {};
  FFOsciType _type = {};
  int _uniCount = {};
  float _uniOffsetPlain = {};
  float _uniRandomPlain = {};
  int _oversamplingTimes = {};

  float _dsfDistance = {};
  float _dsfOvertones = {};
  FFOsciDSFMode _dsfMode = {};
  float _dsfBandwidthPlain = {};
  bool _fmExp = {};
  float _fmRatioRatio12 = {};
  float _fmRatioRatio23 = {};
  FFOsciFMRatioMode _fmRatioMode = {};
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
  std::array<float*, FFOsciUniMaxCount> _downsampledChannelPtrs = {};

  FBParkMillerPRNG _prng = {};
  FBTrackingPhaseGenerator _phaseGen = {};
  FBSIMDArray<float, FFOsciUniMaxCount> _uniPosMHalfToHalf = {};
  FBSIMDArray<float, FFOsciUniMaxCount> _uniPosAbsHalfToHalf = {};
  std::array<FFOsciPhaseGenerator, FFOsciUniMaxCount> _uniPhaseGens = {};
  FBSIMDArray2<float, FBFixedBlockSamples, FFOsciUniMaxCount> _uniOutputDownsampled = {};

  FBSIMDArray2<float, FFOsciUniMaxCount, FFOsciFMOperatorCount> _prevUniFMOutput = {};
  FBMDArray2<FFOsciFMPhaseGenerator, FFOsciUniMaxCount / FBSIMDFloatCount, FFOsciFMOperatorCount> _uniFMPhaseGens = {};

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};