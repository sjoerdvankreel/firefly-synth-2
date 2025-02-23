#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FBModuleProcState;

struct FFOsciVoiceState final
{
  int note = {};
  float key = {}; // TODO floating key
  bool basicSinOn = {};
  bool basicSawOn = {};
  bool basicTriOn = {};
  bool basicSqrOn = {};
  FFOsciType type = {};
};

class FFOsciProcessor final
{
  FBPhase _phase = {};
  FFOsciVoiceState _voiceState = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);
};