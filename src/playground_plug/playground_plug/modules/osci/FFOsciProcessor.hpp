#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciPhase.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/dsp/shared/FBTrackingPhase.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <juce_dsp/juce_dsp.h>
#include <array>

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
  bool unisonDetuneHQ = {};
  float unisonOffsetPlain = {};
  float unisonRandomPlain = {};

  bool expoFM = false;
  bool oversampling = false;
  std::array<bool, FFOsciCount - 1> modSourceFMOn = {};
  std::array<int, FFOsciCount - 1> modSourceUnisonCount = {};
  std::array<FFOsciModAMMode, FFOsciCount - 1> modSourceAMMode = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FBParkMillerPRNG _prng = {};
  FFOsciVoiceState _voiceState = {};
  juce::dsp::Oversampling<float> _oversampling;
  juce::dsp::AudioBlock<float> _oversampledBlock = {};
  std::array<FFOsciPhase, FFOsciUnisonMaxCount> _unisonPhases = {};
  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<FFOsciFMPhases> _unisonPhasesForFM = {};
  alignas(FBSIMDAlign) FFOsciUnisonOperatorArrayForFM<xsimd::batch<float, FBXSIMDBatchType>> _prevUnisonOutputForFM = {};

  void ProcessBasic(
    FBModuleProcState& state,
    int oversamplingTimes,
    FFOsciOversampledUnisonArray const& uniIncrs,
    FFOsciOversampledUnisonArray const& uniPhases);

  void ProcessDSF(
    FBModuleProcState& state,
    int oversamplingTimes,
    FFOsciOversampledUnisonArray const& uniFreqs,
    FFOsciOversampledUnisonArray const& uniIncrs,
    FFOsciOversampledUnisonArray const& uniPhases);

  void ProcessFM(
    FBModuleProcState& state,
    int oversamplingTimes,
    float oversampledRate,
    FFOsciOversampledUnisonArray const& uniPitchs,
    FFOsciOversampledUnisonArray const& fmModulators);

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};