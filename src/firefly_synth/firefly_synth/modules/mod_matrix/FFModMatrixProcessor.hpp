#pragma once

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
struct FFModMatrixSourceState
{
  static inline int constexpr SlotCount = FFModMatrixTraits<Global>::SlotCount;

  bool isReady = {};
  std::vector<int> slotsWithThisSource = {};
};

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

  // mind the bookkeeping
  std::array<bool, SlotCount> _slotHasBeenProcessed = {};
  std::array<bool, SlotCount> _allModSourcesAreReadyForSlot = {};
  // (mod matrix slot * mod matrix slot)
  std::vector<std::vector<int>> _slotsWithSameTarget = {};
  // (source module index * source module slot), map was slow 
  std::vector<std::vector<FFModMatrixSourceState<Global>>> _modSourceState = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixProcessor);

  void BeginVoiceOrBlock(FBModuleProcState& state);
  void InitializeBuffers(FBRuntimeTopo const* topo);
  void EndModulationBlock(FBModuleProcState& state);
  void BeginModulationBlock(FBModuleProcState& state);
  void ApplyModulation(FBModuleProcState& state, FBTopoIndices const& currentModule);
};