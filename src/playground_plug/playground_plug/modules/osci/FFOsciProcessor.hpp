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
  int _dsfDistance = {};
  int _dsfOvertones = {};
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

  FBParkMillerPRNG _prng = {};
  FBTrackingPhaseGenerator _phaseGen = {};
  juce::dsp::Oversampling<float> _oversampler;
  juce::dsp::AudioBlock<float> _oversampledBlock = {};
  FBSIMDArray<float, FFOsciUniMaxCount> _uniPosMHalfToHalf = {};
  FBSIMDArray<float, FFOsciUniMaxCount> _uniPosAbsHalfToHalf = {};
  std::array<FFOsciPhaseGenerator, FFOsciUniMaxCount> _uniPhaseGens = {};
  
  // todo hope to drop all this stuff
#if 0
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniFreqs = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniIncrs = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPhases = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPitches = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _modMatrixFMModulators = {};

  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<FFOsciFMPhasesGenerator> _uniPhaseGensForFM = {};
  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<xsimd::batch<float, FBXSIMDBatchType>> _prevUniOutputForFM = {};
  alignas(FBSIMDAlign) FBMDArray3<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes> _externalFMModulatorsForFM = {};
  alignas(FBSIMDAlign) FBMDArray4<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes, FFOsciFMOperatorCount> _uniIncrsForFM = {};
  alignas(FBSIMDAlign) FBMDArray4<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes, FFOsciFMOperatorCount> _uniPitchesForFM = {};

#endif

  void ProcessBasic(FBModuleProcState& state, int oversamplingTimes);
  void ProcessDSF(FBModuleProcState& state, int oversamplingTimes);

  template <bool ExpoFM>
  void ProcessFM(
    FBModuleProcState& state, int oversamplingTimes, float oversampledRate);
  template <bool ExpoFM>
  xsimd::batch<float, FBXSIMDBatchType> CalcOneSampleForFM(
    float oversampledRate, int subUniBlock, int op,
    float* uniPitchesForFM, float* uniIncrsForFM,
    xsimd::batch<float, FBXSIMDBatchType> fmToOp,
    xsimd::batch<float, FBXSIMDBatchType> externalFMModulatorsForFMBatch);

  void ProcessUniPhasesNonFM(int oversamplingTimes);
  void ProcessUniFreqAndDelta(int oversamplingTimes, float oversampledRate);

#if 0 //todo
  void ProcessDownSampling(
    int oversamplingTimes,
    FFOsciOversampledUnisonArray const& uniOutputMaybeOversampled,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount>& uniOutputNonOversampled);

  void ProcessUniBlendToVoices(
    int oversamplingTimes,
    FBFixedFloatArray const& uniBlendPlain,
    std::array<float, FFOsciUnisonMaxCount> const& uniPositionsAbsHalfToHalf,
    FFOsciOversampledUnisonArray& unisonOutputMaybeOversampled);

  void ProcessUniSpreadToStereo(
    FBFixedFloatArray const& uniSpreadPlain,
    std::array<float, FFOsciUnisonMaxCount> const& uniPositionsMHalfToHalf,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniOutputNonOversampled,
    FBFixedFloatAudioArray& output);

  void ProcessBasePitchAndFreq(
    FBStaticModule const& topo, float sampleRate,
    FBAccParamState const& coarseNorm, FBAccParamState const& fineNorm,
    FBFixedFloatArray& basePitch, FBFixedFloatArray& baseFreq);

  void ProcessUniPitches(
    int oversamplingTimes,
    FBFixedFloatArray const& basePitch,
    FBFixedFloatArray const& uniDetunePlain,
    std::array<float, FFOsciUnisonMaxCount> const& uniPositionsMHalfToHalf);

  void ProcessModMatrixAMModulators(
    int moduleSlot,
    int oversamplingTimes,
    std::array<FBFixedFloatArray, FFOsciModSlotCount> const& outputAMMix,
    std::array<FFOsciDSPState, FFOsciCount>& allOsciDSPStates);

  void ProcessModMatrixFMModulators(
    int moduleSlot,
    int oversamplingTimes,
    std::array<FFOsciDSPState, FFOsciCount> const& allOsciDSPStates,
    std::array<FBFixedFloatArray, FFOsciModSlotCount> const& outputFMIndex);

  void ProcessUniBlendDetuneSpreadAndPos(
    FBStaticModule const& topo,
    FBAccParamState const& uniBlendNorm,
    FBAccParamState const& uniDetuneNorm,
    FBAccParamState const& uniSpreadNorm,
    FBFixedFloatArray& uniBlendPlain,
    FBFixedFloatArray& uniDetunePlain,
    FBFixedFloatArray& uniSpreadPlain,
    std::array<float, FFOsciUnisonMaxCount>& uniPositionsMHalfToHalf,
    std::array<float, FFOsciUnisonMaxCount>& uniPositionsAbsHalfToHalf);

#endif

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};