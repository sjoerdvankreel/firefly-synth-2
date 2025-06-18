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
#include <firefly_synth/modules/osci_mod/FFOsciModState.hpp>
#include <firefly_synth/modules/shared/FFOsciProcessorBase.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <juce_dsp/juce_dsp.h>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

inline float constexpr FFOsciStringMinFreq = 20.0f;

inline int
OsciModStartSlot(int osciSlot)
{
  switch (osciSlot)
  {
  case 0: return -1;
  case 1: return 0;
  case 2: return 1;
  case 3: return 3;
  default: FB_ASSERT(false); return -1;
  }
}

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

class FFOsciProcessor final
{
  float _key = {};

  FFOsciType _type = {};
  int _uniCount = {};
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

  int _stringGraphPosition = {};
  float _stringGraphStVarFilterFreqMultiplier = {};

  bool _modMatrixExpoFM = false;
  std::array<bool, FFOsciCount - 1> _modSourceFMOn = {};
  std::array<int, FFOsciCount - 1> _modSourceUniCount = {};
  std::array<FFOsciModAMMode, FFOsciCount - 1> _modSourceAMMode = {};

  juce::dsp::Oversampling<float> _oversampler;
  juce::dsp::AudioBlock<float> _oversampledBlock = {};
  juce::dsp::AudioBlock<float> _downsampledBlock = {};
  std::array<float*, FFOsciUniMaxCount> _downsampledChannelPtrs = {};

  FFParkMillerPRNG _uniformPrng = {};
  FFMarsagliaPRNG _stringNormalPrng = {};
  FFStateVariableFilter<FFOsciUniMaxCount> _stringLPFilter = {};
  FFStateVariableFilter<FFOsciUniMaxCount> _stringHPFilter = {};
  std::array<FFOsciStringUniVoiceState, FFOsciUniMaxCount> _stringUniState = {};

  FFTrackingPhaseGenerator _phaseGen = {};
  std::array<FFOsciWavePhaseGenerator, FFOsciUniMaxCount> _uniWavePhaseGens = {};
  FBSArray2<float, FFOsciUniMaxCount, FFOsciFMOperatorCount> _prevUniFMOutput = {};
  std::array<std::array<FFOsciFMPhaseGenerator, FFOsciUniMaxCount / FBSIMDFloatCount>, FFOsciFMOperatorCount> _uniFMPhaseGens = {};

  FBSArray<float, FFOsciUniMaxCount> _uniPosMHalfToHalf = {};
  FBSArray<float, FFOsciUniMaxCount> _uniPosAbsHalfToHalf = {};
  FBSArray2<float, FBFixedBlockSamples, FFOsciUniMaxCount> _uniOutput = {};

  float 
  StringDraw();
  float 
  StringNext(
    int uniVoice,
    float sampleRate, float uniFreq,
    float excite, float colorNorm,
    float xNorm, float yNorm);

  void ProcessFM(
    FBModuleProcState& state,
    FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
    FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain);
  void ProcessWave(
    FBModuleProcState& state,
    FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
    FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain);
  void ProcessString(
    FBModuleProcState& state,
    FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
    FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain);

  void BeginVoiceString(bool graph, FBModuleProcState& state);
  void BeginVoiceFM(FBModuleProcState& state, FBSArray<float, FFOsciUniMaxCount> const& uniPhaseInit);
  void BeginVoiceWave(FBModuleProcState& state, FBSArray<float, FFOsciUniMaxCount> const& uniPhaseInit);

public:
  FFOsciProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void InitializeBuffers(bool graph, float sampleRate);
  void BeginVoice(bool graph, FBModuleProcState& state);
};