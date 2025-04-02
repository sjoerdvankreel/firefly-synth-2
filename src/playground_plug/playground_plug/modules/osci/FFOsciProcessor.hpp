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
  std::array<FFOsciPhase, FFOsciUnisonMaxCount> _unisonPhases = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};