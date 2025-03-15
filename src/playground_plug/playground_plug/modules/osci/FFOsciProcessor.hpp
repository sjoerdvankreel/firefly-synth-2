#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/dsp/shared/FBTrackingPhase.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

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
  float dsfBandwidth = {};
  FFOsciDSFMode dsfMode = {};

  int unisonCount = {};
  float unisonOffset = {};
  float unisonOffsetRandom = {};

  std::array<bool, FFOsciCount> amSourceOn = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FBParkMillerPRNG _prng = {};
  FFOsciVoiceState _voiceState = {};
  std::array<FBPhase, FFOsciUnisonMaxCount> _phases = {};

  void ProcessBasicUnisonVoice(
    FBFixedFloatBlock& unisonAudioOut,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock const& sinGainPlain,
    FBFixedFloatBlock const& sawGainPlain,
    FBFixedFloatBlock const& triGainPlain,
    FBFixedFloatBlock const& sqrGainPlain,
    FBFixedFloatBlock const& sqrPWPlain);

  void ProcessDSFUnisonVoice(
    float sampleRate,
    FBFixedFloatBlock& unisonAudioOut,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock const& decayPlain);

  void ProcessTypeUnisonVoice(
    float sampleRate,
    FBFixedFloatBlock& unisonAudioOut,
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
    FBModuleProcState const& state,
    int unisonVoice, float pos,
    FBFixedFloatBlock& unisonAudioOut,
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
    std::array<FBFixedFloatBlock, FFOsciUnisonMaxCount>& unisonAudioOut,
    FBFixedFloatBlock const& basePitch);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);
};