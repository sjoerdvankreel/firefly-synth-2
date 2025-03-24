#pragma once

#include <playground_plug/modules/osci_am/FFOsciAMTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FBModuleProcState;

struct FFOsciAMVoiceState final
{
  std::array<bool, FFOsciModSlotCount> on = {};
};

class FFOsciAMProcessor final
{
  FFOsciAMVoiceState _voiceState = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAMProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};