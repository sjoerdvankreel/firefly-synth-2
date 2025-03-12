#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/dsp/shared/FBTrackingPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FBModuleProcState;

struct FFOsciVoiceState final
{
  int note = {};
  float key = {}; // TODO floating key
  FFOsciType type = {};
  int unisonCount = {};
  float unisonOffset = {};
  bool basicSinOn = {};
  bool basicSawOn = {};
  bool basicTriOn = {};
  bool basicSqrOn = {};
  FFOsciDSFMode dsfMode = {};
  int dsfDistance = {};
  int dsfOvertones = {};
  float dsfBandwidth = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FFOsciVoiceState _voiceState = {};
  std::array<FBPhase, FFOsciUnisonMaxCount> _phases = {};

  void ProcessBasicUnisonVoice(
    FBFixedFloatBlock& audioOut,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock const& sinGainPlain,
    FBFixedFloatBlock const& sawGainPlain,
    FBFixedFloatBlock const& triGainPlain,
    FBFixedFloatBlock const& sqrGainPlain,
    FBFixedFloatBlock const& sqrPWPlain);

  void ProcessDSFUnisonVoice(
    float sampleRate,
    FBFixedFloatBlock& audioOut,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock const& decayPlain);

  void ProcessTypeUnisonVoice(
    float sampleRate,
    FBFixedFloatBlock& audioOut,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock const& basicSinGainPlain,
    FBFixedFloatBlock const& basicSawGainPlain,
    FBFixedFloatBlock const& basicTriGainPlain,
    FBFixedFloatBlock const& basicSqrGainPlain,
    FBFixedFloatBlock const& basicSqrPWPlain,
    FBFixedFloatBlock const& dsfDecayPlain);

  void ProcessUnisonVoice(
    float sampleRate, int unisonVoice, float pos,
    FBFixedFloatBlock& audioOut,
    FBFixedFloatBlock const& basePitch,
    FBFixedFloatBlock const& detunePlain,
    FBFixedFloatBlock const& basicSinGainPlain,
    FBFixedFloatBlock const& basicSawGainPlain,
    FBFixedFloatBlock const& basicTriGainPlain,
    FBFixedFloatBlock const& basicSqrGainPlain,
    FBFixedFloatBlock const& basicSqrPWPlain,
    FBFixedFloatBlock const& dsfDecayPlain);

  void ProcessUnison(
    FBModuleProcState& state,
    FBFixedFloatAudioBlock& audioOut,
    FBFixedFloatBlock const& basePitch);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);
};