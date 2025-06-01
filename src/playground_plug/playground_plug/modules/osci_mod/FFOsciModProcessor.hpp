#pragma once

#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>

struct FBModuleProcState;

class FFOsciModProcessor final
{
  bool _oversample = {};
  std::array<bool, FFOsciModSlotCount> _fmOn = {};
  std::array<FFOsciModAMMode, FFOsciModSlotCount> _amMode = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciModProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(bool graph, FBModuleProcState& state);
};