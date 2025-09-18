#pragma once

#include <firefly_synth/modules/osci_mod/FFOsciModTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

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
  void BeginVoice(FBModuleProcState& state, bool graph);
};