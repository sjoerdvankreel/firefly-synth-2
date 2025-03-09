#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/dsp/shared/FBPhase.hpp>
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
  FBPhase _phase = {};
  FFOsciVoiceState _voiceState = {};

  void ProcessBasic(
    FBModuleProcState& state, 
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock& audioOut);

  void ProcessDSF(
    FBModuleProcState& state,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock& audioOut);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);
};