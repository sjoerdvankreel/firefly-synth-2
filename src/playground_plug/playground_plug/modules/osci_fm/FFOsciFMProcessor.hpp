#pragma once

#include <playground_plug/modules/osci_fm/FFOsciFMTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FBModuleProcState;

struct FFOsciFMVoiceState final
{
  std::array<bool, FFOsciModSlotCount> on = {};
  std::array<bool, FFOsciModSlotCount> throughZero = {};
};

class FFOsciFMProcessor final
{
  FFOsciFMVoiceState _voiceState = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciFMProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);
};