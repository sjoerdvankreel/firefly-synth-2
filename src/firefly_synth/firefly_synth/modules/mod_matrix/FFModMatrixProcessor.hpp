#pragma once

#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixProcessor.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <map>
#include <array>
#include <vector>

struct FBTopoIndices;
struct FBModuleProcState;
class FBProcStateContainer;

template <bool Global>
class FFModMatrixProcessor final
{
  static inline int constexpr SlotCount = FFModMatrixTraits<Global>::SlotCount;
  std::array<int, SlotCount> _scale = {};
  std::array<int, SlotCount> _source = {};
  std::array<int, SlotCount> _target = {};
  std::array<FFModMatrixOpType, SlotCount> _opType = {};

  // voice only
  std::array<float, SlotCount> _scaleOnNoteValues = {};
  std::array<float, SlotCount> _sourceOnNoteValues = {};

  // mind the bookkeeping - also don't clear it because alloc, just set to false
  std::map<FBTopoIndices, bool> _modSourceIsReady = {};
  std::array<bool, SlotCount> _slotHasBeenProcessed = {};
  std::array<bool, SlotCount> _allModSourcesAreReadyForSlot = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixProcessor);

  void BeginModulationBlock();
  void EndModulationBlock(FBModuleProcState& state);
  void BeginVoiceOrBlock(FBModuleProcState& state);
  void ApplyModulation(FBModuleProcState& state, FBTopoIndices const& currentModule);
};