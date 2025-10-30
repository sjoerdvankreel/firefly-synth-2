#pragma once

#include <firefly_synth/modules/mod_matrix/FFModMatrixSource.hpp>

#include <memory>
#include <vector>

struct FBStaticTopo;

inline int constexpr FFModMatrixVoiceMaxSlotCount = 25;
inline int constexpr FFModMatrixGlobalMaxSlotCount = 12;

template <bool Global>
struct FFModMatrixTraits
{
  static inline int constexpr MaxSlotCount = Global ? FFModMatrixGlobalMaxSlotCount : FFModMatrixVoiceMaxSlotCount;
};

struct FFStaticTopo;
struct FBStaticModule;
enum class FFModMatrixParam { 
  Slots, OpType, 
  Source, SourceInv, SourceLow, SourceHigh,
  Scale, ScaleMin, ScaleMax,
  Target, TargetAmt, Count };

std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool global, FFStaticTopo const* topo);
std::vector<FFModMatrixSource> FFModMatrixMakeSources(bool global, FBStaticTopo const* topo);
std::vector<FBParamTopoIndices> FFModMatrixMakeTargets(bool global, FBStaticTopo const* topo);
