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

template <class T>
using FFOsciUnisonOperatorArrayForFM =
std::array<std::array<T, FFOsciUnisonMaxCount / FBSIMDFloatCount>, FFOsciFMOperatorCount>;

struct FFOsciVoiceState final
{
  float key = {};
  FFOsciType type = {};

  bool basicSinOn = {};
  bool basicSawOn = {};
  bool basicTriOn = {};
  bool basicSqrOn = {};

  int dsfDistance = {};
  int dsfOvertones = {};
  FFOsciDSFMode dsfMode = {};
  float dsfBandwidthPlain = {};

  bool fmExp = {};
  float fmRatioRatio12 = {};
  float fmRatioRatio23 = {};
  FFOsciFMRatioMode fmRatioMode = {};

  int unisonCount = {};
  float unisonOffsetPlain = {};
  float unisonRandomPlain = {};

  bool oversampling = false;
  bool externalFMExp = false;
  std::array<bool, FFOsciCount - 1> modSourceFMOn = {};
  std::array<int, FFOsciCount - 1> modSourceUnisonCount = {};
  std::array<FFOsciModAMMode, FFOsciCount - 1> modSourceAMMode = {};
};

class FFOsciProcessor final
{
  FBParkMillerPRNG _prng = {};
  FFOsciVoiceState _voiceState = {};
  FBTrackingPhaseGenerator _phaseGen = {};
  juce::dsp::Oversampling<float> _oversampling;
  juce::dsp::AudioBlock<float> _oversampledBlock = {};
  std::array<FFOsciPhaseGenerator, FFOsciUnisonMaxCount> _uniPhaseGens = {};
  
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniFreqs = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniIncrs = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPhases = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPitches = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _modMatrixFMModulators = {};

  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<FFOsciFMPhasesGenerator> _uniPhaseGensForFM = {};
  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<xsimd::batch<float, FBXSIMDBatchType>> _prevUniOutputForFM = {};
  alignas(FBSIMDAlign) FBMDArray3<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes> _externalFMModulatorsForFM = {};
  alignas(FBSIMDAlign) FBMDArray4<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes, FFOsciFMOperatorCount> _uniPitchsForFM = {};

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

  void ProcessDownSampling(
    int oversamplingTimes,
    FFOsciOversampledUnisonArray const& uniOutputMaybeOversampled,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount>& uniOutputNonOversampled);

  void ProcessUniSpreadToStereo(
    FBFixedFloatArray const& uniSpreadPlain,
    std::array<float, FFOsciUnisonMaxCount> const& uniPositions,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniOutputNonOversampled,
    FBFixedFloatAudioArray& output);

  void ProcessBasePitchAndFreq(
    FBStaticModule const& topo, float sampleRate,
    FBAccParamState const& coarseNorm, FBAccParamState const& fineNorm,
    FBFixedFloatArray& basePitch, FBFixedFloatArray& baseFreq);

  void ProcessUniDetuneSpreadAndPos(
    FBStaticModule const& topo,
    FBAccParamState const& uniDetuneNorm, FBAccParamState const& uniSpreadNorm,
    FBFixedFloatArray& uniDetunePlain, FBFixedFloatArray& uniSpreadPlain,
    std::array<float, FFOsciUnisonMaxCount>& uniPositions);

  void ProcessUniPitches(
    int oversamplingTimes,
    FBFixedFloatArray const& basePitch,
    FBFixedFloatArray const& uniDetunePlain,
    std::array<float, FFOsciUnisonMaxCount> const& uniPositions);

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

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};