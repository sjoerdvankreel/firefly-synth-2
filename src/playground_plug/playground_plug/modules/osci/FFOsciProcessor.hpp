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

inline int constexpr FFOsciSyncCrossOverSamples = 8;

template <class T>
using FFOsciUnisonOperatorArrayForFM =
std::array<std::array<T, FFOsciUnisonMaxCount / FBSIMDFloatCount>, FFOsciFMOperatorCount>;

struct FFOsciVoiceState final
{
  float key = {};
  FFOsciType type = {};

  bool syncOn = {};

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
  FFOsciOversampledUnisonArray _uniSyncCrossOverWeights = {};
  std::array<int, FFOsciUnisonMaxCount> _uniSyncCrossOverSamples = {};
  FFOsciOversampledUnisonArray _unisonOutputMaybeOversampledUnsynced = {};
  std::array<FFOsciMasterPhaseGenerator, FFOsciUnisonMaxCount> _uniPhaseGensMaster = {};
  std::array<FFOsciSlavePhaseGenerator, FFOsciUnisonMaxCount> _uniPhaseGensSlaveSynced = {};
  std::array<FFOsciSlavePhaseGenerator, FFOsciUnisonMaxCount> _uniPhaseGensSlaveUnsynced = {};

  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniFreqsMaster = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniIncrsMaster = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPhasesMaster = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPitchesMaster = {};

  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniFreqsSlave = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniIncrsSlave = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPitchesSlave = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPhasesSlaveSynced = {};
  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _uniPhasesSlaveUnsynced = {};

  alignas(FBSIMDAlign) FFOsciOversampledUnisonArray _modMatrixFMModulators = {};

  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<FFOsciFMPhasesGenerator> _uniPhaseGensForFM = {}; // todo hsync fm
  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<xsimd::batch<float, FBXSIMDBatchType>> _prevUniOutputForFM = {};
  alignas(FBSIMDAlign) FBMDArray3<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes> _externalFMModulatorsForFM = {};
  alignas(FBSIMDAlign) FBMDArray4<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes, FFOsciFMOperatorCount> _uniIncrsForFM = {};
  alignas(FBSIMDAlign) FBMDArray4<float, FFOsciUnisonMaxCount, FBFixedBlockSamples, FFOsciOverSamplingTimes, FFOsciFMOperatorCount> _uniPitchesForFM = {};

  void ProcessBasic(
    FBModuleProcState& state, 
    int oversamplingTimes,
    FFOsciOversampledUnisonArray const& uniPhases);
  void ProcessDSF(
    FBModuleProcState& state, 
    int oversamplingTimes,
    FFOsciOversampledUnisonArray const& uniPhases);

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

  void ProcessBasePitchFreqAndSync(
    FBStaticModule const& topo, float sampleRate,
    FBAccParamState const& coarseNorm, FBAccParamState const& fineNorm,
    FBAccParamState const& syncNotesNorm, FBFixedFloatArray& basePitch,
    FBFixedFloatArray& baseFreq, FBFixedFloatArray& syncNotesPlain);

  void ProcessUniPitches(
    int oversamplingTimes,
    FBFixedFloatArray const& basePitch,
    FBFixedFloatArray const& syncNotesPlain,
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

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};