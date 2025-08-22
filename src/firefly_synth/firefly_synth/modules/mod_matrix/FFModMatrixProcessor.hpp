#pragma once

#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixProcessor.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <map>
#include <array>
#include <vector>

struct FBRuntimeTopo;
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
  std::array<FFModulationOpType, SlotCount> _opType = {};

  // voice only
  std::array<float, SlotCount> _scaleOnNoteValues = {};
  std::array<float, SlotCount> _sourceOnNoteValues = {};

  // mind the bookkeeping
  // source index * source slot, map was slow, 0/1 = bool
  std::vector<std::vector<int>> _modSourceIsReady = {};
  std::array<bool, SlotCount> _slotHasBeenProcessed = {};
  std::array<bool, SlotCount> _allModSourcesAreReadyForSlot = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixProcessor);

  void BeginModulationBlock();
  void EndModulationBlock(FBModuleProcState& state);

  void InitBuffers(FBRuntimeTopo const* topo);
  void BeginVoiceOrBlock(FBModuleProcState& state);
  void ApplyModulation(FBModuleProcState& state, FBTopoIndices const& currentModule);
};