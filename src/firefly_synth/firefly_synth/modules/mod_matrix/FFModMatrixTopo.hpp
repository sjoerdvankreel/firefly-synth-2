#pragma once

#include <firefly_synth/modules/mod_matrix/FFModMatrixSource.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <string>
#include <memory>
#include <vector>
#include <utility>

struct FBStaticTopo;

inline int constexpr FFModMatrixGlobalSlotCount = 14;
inline int constexpr FFModMatrixVoiceSlotCount = 2 * FFModMatrixGlobalSlotCount;

struct FFStaticTopo;
struct FBStaticModule;
enum class FFModMatrixOpType { Off, Add, Mul, Stack, BPAdd, BPStack };
enum class FFModMatrixParam { OpType, Source, Scale, Target, Amount, Count };

std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool global, FFStaticTopo const* topo);
std::vector<FFModMatrixSource> FFModMatrixMakeSources(bool global, FBStaticTopo const* topo);
std::vector<FBParamTopoIndices> FFModMatrixMakeTargets(bool global, FBStaticTopo const* topo);
