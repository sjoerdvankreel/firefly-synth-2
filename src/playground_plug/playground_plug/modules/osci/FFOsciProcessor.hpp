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

struct FFOsciVoiceState final
{
  int note = {};
  float key = {}; // TODO floating key
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

  bool oversampling = false;
  std::array<int, FFOsciCount - 1> modSourceUnisonCount = {};
  std::array<FFOsciModAMMode, FFOsciCount - 1> modSourceAMMode = {};
  std::array<FFOsciModFMMode, FFOsciCount - 1> modSourceFMMode = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FBParkMillerPRNG _prng = {};
  FFOsciVoiceState _voiceState = {};
  juce::dsp::Oversampling<float> _oversampling;
  juce::dsp::AudioBlock<float> _oversampledBlock = {};
  std::array<FFOsciPhase, FFOsciUnisonMaxCount> _unisonPhases = {};
  alignas(FBSIMDAlign) std::array<std::array<FFOsciFMPhases, 
    FFOsciUnisonMaxCount / FBSIMDFloatCount>, FFOsciFMOperatorCount> _unisonPhasesForFM = {};
  alignas(FBSIMDAlign) std::array<std::array<xsimd::batch<float, FBXSIMDBatchType>, 
    FFOsciUnisonMaxCount / FBSIMDFloatCount>, FFOsciFMOperatorCount> _prevUnisonOutputForFM = {};

  void ProcessBasic(
    FBModuleProcState& state,
    int oversamplingTimes,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniIncrs,
    std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> const& uniPhases);

  void ProcessDSF(
    FBModuleProcState& state,
    int oversamplingTimes,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniFreqs,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniIncrs,
    std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> const& uniPhases);

  void ProcessFM(
    FBModuleProcState& state,
    int oversamplingTimes,
    float oversampledRate,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniFreqs,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniIncrs,
    std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> const& fmModulators);

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};