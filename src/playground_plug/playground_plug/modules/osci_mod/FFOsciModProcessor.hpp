#pragma once

#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FBModuleProcState;

struct FFOsciModVoiceState final
{
  bool expoFM = {};
  std::array<bool, FFOsciModSlotCount> fmOn = {};
  std::array<FFOsciModAMMode, FFOsciModSlotCount> amMode = {};
};

class FFOsciModProcessor final
{
  FFOsciModVoiceState _voiceState = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};