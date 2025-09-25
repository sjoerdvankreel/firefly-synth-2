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
  static inline int constexpr MaxSlotCount = FFModMatrixTraits<Global>::MaxSlotCount;

  // user selectable, this was a perf opt
  int _activeSlotCount = {};

  std::array<int, MaxSlotCount> _scale = {};
  std::array<int, MaxSlotCount> _source = {};
  std::array<int, MaxSlotCount> _target = {};
  std::array<FFModulationOpType, MaxSlotCount> _opType = {};

  // voice only
  std::array<bool, MaxSlotCount> _onNoteWasSnapshotted = {};
  std::array<float, MaxSlotCount> _scaleOnNoteSnapshot = {};
  std::array<float, MaxSlotCount> _sourceOnNoteSnapshot = {};

  // mind the bookkeeping
  // source index * source slot, map was slow, 0/1 = bool
  std::vector<std::vector<int>> _modSourceIsReady = {};
  std::array<bool, MaxSlotCount> _slotHasBeenProcessed = {};
  std::array<bool, MaxSlotCount> _ownModSourceIsReadyForSlot = {};
  std::array<bool, MaxSlotCount> _allModSourcesAreReadyForSlot = {};

  // Whenever one or more sources clear, re-evaluate all targets
  // which have a source that depends on those.
  void UpdateCandidateSlots(FBModuleProcState& state);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixProcessor);

  void BeginModulationBlock();
  void EndModulationBlock(FBModuleProcState& state);

  void InitBuffers(FBRuntimeTopo const* topo);
  void BeginVoiceOrBlock(FBModuleProcState& state);

  // perf opt: by having separate cleared/apply functions,
  // we can do "all global mod sources cleared" at once for voice matrix
  void ApplyModulation(FBModuleProcState& state);
  void ModSourceCleared(FBModuleProcState& state, FBTopoIndices const& currentModule);
};