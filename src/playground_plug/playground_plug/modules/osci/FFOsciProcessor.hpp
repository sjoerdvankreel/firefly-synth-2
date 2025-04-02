#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciPhase.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
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
  FFOsciDSFMode dsfMode = {};
  float dsfBandwidthPlain = {};

  int unisonCount = {};
  bool unisonDetuneHQ = {};
  float unisonOffsetPlain = {};
  float unisonOffsetRandomPlain = {};

  std::array<bool, FFOsciCount - 1> modSourceOn = {};
  std::array<int, FFOsciCount - 1> modSourceUnisonCount = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FBParkMillerPRNG _prng = {};
  FFOsciVoiceState _voiceState = {};
  std::array<FFOsciPhase, FFOsciUnisonMaxCount> _phases = {};

  void ProcessBasicUnisonVoiceFast(
    FBFixedFloatArray& unisonAudioOut,
    FBFixedFloatArray const& phasePlusFm,
    FBFixedFloatArray const& incr,
    FBFixedFloatArray const& sinGainPlain,
    FBFixedFloatArray const& sawGainPlain,
    FBFixedFloatArray const& triGainPlain,
    FBFixedFloatArray const& sqrGainPlain,
    FBFixedFloatArray const& sqrPWPlain);

  void ProcessBasicUnisonVoiceSlow(
    float& unisonAudioOut,
    float const phasePlusFm,
    float const incr,
    float const sinGainPlain,
    float const sawGainPlain,
    float const triGainPlain,
    float const sqrGainPlain,
    float const sqrPWPlain);

  void ProcessDSFUnisonVoiceFast(
    float sampleRate,
    FBFixedFloatArray& unisonAudioOut,
    FBFixedFloatArray const& phasePlusFm,
    FBFixedFloatArray const& freq,
    FBFixedFloatArray const& decayPlain);

  void ProcessDSFUnisonVoiceSlow(
    float sampleRate,
    float& unisonAudioOut,
    float const phasePlusFm,
    float const freq,
    float const decayPlain);

  void ProcessTypeUnisonVoiceFast(
    float sampleRate,
    FBFixedFloatArray& unisonAudioOut,
    FBFixedFloatArray const& phasePlusFm,
    FBFixedFloatArray const& freq,
    FBFixedFloatArray const& incr,
    FBFixedFloatArray const& basicSinGainPlain,
    FBFixedFloatArray const& basicSawGainPlain,
    FBFixedFloatArray const& basicTriGainPlain,
    FBFixedFloatArray const& basicSqrGainPlain,
    FBFixedFloatArray const& basicSqrPWPlain,
    FBFixedFloatArray const& dsfDecayPlain);

  void ProcessTypeUnisonVoiceSlow(
    float sampleRate,
    float& unisonAudioOut,
    float const phasePlusFm,
    float const freq,
    float const incr,
    float const basicSinGainPlain,
    float const basicSawGainPlain,
    float const basicTriGainPlain,
    float const basicSqrGainPlain,
    float const basicSqrPWPlain,
    float const dsfDecayPlain);

  void ProcessUnisonVoice(
    FBModuleProcState const& state,
    int unisonVoice, 
    FBFixedFloatArray& unisonAudioOut,
    FBFixedFloatArray const& unisonPos,
    FBFixedFloatArray const& basePitch,
    FBFixedFloatArray const& detunePlain,
    FBFixedFloatArray const& basicSinGainPlain,
    FBFixedFloatArray const& basicSawGainPlain,
    FBFixedFloatArray const& basicTriGainPlain,
    FBFixedFloatArray const& basicSqrGainPlain,
    FBFixedFloatArray const& basicSqrPWPlain,
    FBFixedFloatArray const& dsfDecayPlain);

  void ProcessUnison(
    FBModuleProcState& state,
    FBFixedFloatAudioArray& audioOut,
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount>& unisonAudioOut,
    FBFixedFloatArray const& basePitch);

  void ShiftPrevUnisonOutputForAllOscis(
    FBModuleProcState& state);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  int Process2(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};