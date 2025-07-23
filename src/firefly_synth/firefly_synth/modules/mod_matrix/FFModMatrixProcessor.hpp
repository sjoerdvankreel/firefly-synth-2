#pragma once

#include <firefly_synth/modules/mod_matrix/FFModMatrixTopo.hpp>
#include <firefly_synth/modules/mod_matrix/FFModMatrixProcessor.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>

struct FBTopoIndices;
struct FBModuleProcState;

template <bool Global>
class FFModMatrixProcessor final
{
  static inline int constexpr SlotCount = Global ? FFModMatrixGlobalSlotCount : FFModMatrixVoiceSlotCount;

  std::array<int, SlotCount> _scale = {};
  std::array<int, SlotCount> _source = {};
  std::array<int, SlotCount> _target = {};
  std::array<FFModMatrixOpType, SlotCount> _opType = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFModMatrixProcessor);

  void ClearModulation(FBModuleProcState& state);
  void BeginVoiceOrBlock(FBModuleProcState& state);
  void ApplyModulation(FBModuleProcState& state, FBTopoIndices const& currentModule);
};