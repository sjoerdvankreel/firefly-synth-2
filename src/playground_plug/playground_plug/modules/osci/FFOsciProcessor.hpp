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

typedef FBStaticArray<
  float, FFOsciUnisonMaxCount, FBFixedBlockAlign>
  FFOsciUnisonArray;
typedef std::array<
  FFOsciUnisonArray, FBFixedBlockSamples>
  FFOsciFixedUnisonArray;
typedef std::array<
  FFOsciFixedUnisonArray, FFOsciOverSamplingTimes>
  FFOsciOversampledUnisonArray;

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
  std::array<std::array<FFOsciPhase, FFOsciFMOperatorCount>, FFOsciUnisonMaxCount> _unisonPhases = {}; // todo
  std::array<std::array<float, FFOsciFMOperatorCount>, FFOsciUnisonMaxCount> _prevUnisonOutputForFM = {};

  void ProcessBasic(
    FBModuleProcState& state,
    int oversamplingTimes,
    FFOsciFixedUnisonArray const& uniIncrs,
    FFOsciOversampledUnisonArray const& uniPhases);

  void ProcessDSF(
    FBModuleProcState& state,
    int oversamplingTimes,
    float oversampledRate,
    FFOsciFixedUnisonArray const& uniFreqs,
    FFOsciFixedUnisonArray const& uniIncrs,
    FFOsciOversampledUnisonArray const& uniPhases);

  void ProcessFM(
    FBModuleProcState& state,
    int oversamplingTimes,
    float oversampledRate,
    FFOsciFixedUnisonArray const& uniFreqs,
    FFOsciFixedUnisonArray const& uniIncrs,
    FFOsciOversampledUnisonArray const& fmModulators);

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};