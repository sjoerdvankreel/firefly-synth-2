#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>

#include <juce_dsp/juce_dsp.h>

#include <array>
#include <vector>

class FBAccParamState;
struct FBModuleProcState;

enum class FFEchoVoiceExtensionStage
{
  NotStarted,
  Extending,
  Fading,
  Finished
};

inline int constexpr FFEchoReverbCombCount = 8;
inline int constexpr FFEchoReverbAllPassCount = 4;
inline float constexpr FFEchoPlotLengthSeconds = 5.0f;

struct FFEchoDelayState
{
  void Reset();
  FBBasicLPFilter smoother = {};
  FFStateVariableFilter<2> lpFilter = {};
  FFStateVariableFilter<2> hpFilter = {};
};

// https://github.com/sinshu/freeverb
// Would be better to use FFCombFilter here but i had a working reverb 
// implementation from FF1 and I don't feel like rewriting it. 
// Also i'd have to figure out if Freeverb uses feedback or feedforward
// combs and also figure out if the allpass filter below can somehow
// be mapped to the Cytomic state variable filter in allpass mode.
struct FFEchoReverbState
{
  void Reset();
  FFStateVariableFilter<2> lpFilter = {};
  FFStateVariableFilter<2> hpFilter = {};
  std::array<std::array<int, FFEchoReverbCombCount>, 2> combPosition = {};
  std::array<std::array<float, FFEchoReverbCombCount>, 2> combFilter = {};
  std::array<std::array<std::vector<float>, FFEchoReverbCombCount>, 2> combState = {};
  std::array<std::array<std::int32_t, FFEchoReverbAllPassCount>, 2> allPassPosition = {};
  std::array<std::array<std::vector<float>, FFEchoReverbAllPassCount>, 2> allPassState = {};
};

template <bool Global>
class FFEchoProcessor final
{
  bool _on = {};
  bool _sync = {};
  bool _tapsOn = {};
  bool _feedbackOn = {};
  bool _reverbOn = {};
  FFEchoOrder _order = {};
  int _voiceFadeSamples = {};
  int _voiceExtendSamples = {};
  float _feedbackDelayBarsSamples = {};
  std::array<bool, FFEchoTapCount> _tapOn = {};
  std::array<float, FFEchoTapCount> _tapDelayBarsSamples = {};

  bool _graph = {};
  int _graphSampleCount = {};
  int _graphSamplesProcessed = {};
  float _graphStVarFilterFreqMultiplier = {};

  int _voiceFadeSamplesProcessed = {};
  int _voiceExtendSamplesProcessed = {};
  FFEchoVoiceExtensionStage _voiceExtensionStage = {};

  FFEchoReverbState _reverbState = {};
  FFEchoDelayState _feedbackDelayState = {};
  std::array<FFDelayLine<1>, 2> _feedbackDelayLine = {};
  std::array<FFDelayLine<FFEchoTapCount>, 2> _tapsDelayLine = {};
  std::array<FFEchoDelayState, FFEchoTapCount> _tapDelayStates = {};

  void ProcessTaps(
    FBModuleProcState& state,
    bool processAudioOrExchangeState);

  void ProcessFeedback(
    FBModuleProcState& state,
    bool processAudioOrExchangeState);

  void ProcessReverb(
    FBModuleProcState& state,
    bool processAudioOrExchangeState);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEchoProcessor);

  int Process(FBModuleProcState& state, int ampEnvFinishedAt);
  void BeginVoiceOrBlock(bool graph, int graphIndex, int graphSampleCount, FBModuleProcState& state);
  void ReleaseOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state);
  void AllocOnDemandBuffers(FBRuntimeTopo const* topo, FBProcStateContainer* state, bool graph, float sampleRate);
};