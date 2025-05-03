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

  bool _basicSinOn = {};
  bool _basicSawOn = {};
  bool _basicTriOn = {};
  bool _basicSqrOn = {};
  float _dsfDistance = {};
  float _dsfOvertones = {};
  FFOsciDSFMode _dsfMode = {};
  float _dsfBandwidthPlain = {};
  bool _fmExp = {};
  float _fmRatioRatio12 = {};
  float _fmRatioRatio23 = {};
  FFOsciFMRatioMode _fmRatioMode = {};

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

  FBSIMDArray<float, FFOsciUniMaxCount> _prevUniFMOutput = {};
  FBMDArray2<FFOsciFMPhaseGenerator, FFOsciUniMaxCount / FBSIMDFloatCount, FFOsciFMOperatorCount> _uniFMPhaseGens = {};

  template <bool ExpoFM>
  void ProcessFM(
    FBModuleProcState& state, int oversamplingTimes, float oversampledRate);
  template <bool ExpoFM>
  xsimd::batch<float, FBXSIMDBatchType> CalcOneSampleForFM(
    float oversampledRate, int subUniBlock, int op,
    float* uniPitchesForFM, float* uniIncrsForFM,
    xsimd::batch<float, FBXSIMDBatchType> fmToOp,
    xsimd::batch<float, FBXSIMDBatchType> externalFMModulatorsForFMBatch);

#if 0 //todo


  void ProcessModMatrixAMModulators(
    int moduleSlot,
    int oversamplingTimes,
    std::array<FBFixedFloatArray, FFOsciModSlotCount> const& outputAMMix,
    std::array<FFOsciDSPState, FFOsciCount>& allOsciDSPStates);

#endif

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};