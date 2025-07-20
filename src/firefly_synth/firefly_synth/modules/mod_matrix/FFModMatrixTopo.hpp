#pragma once

#include <string>
#include <memory>
#include <utility>

inline int constexpr FFModMatrixGlobalSlotCount = 14;
inline int constexpr FFModMatrixVoiceSlotCount = 2 * FFModMatrixGlobalSlotCount;

struct FBStaticTopo;
struct FBStaticModule;
enum class FFModMatrixOpType { Off, Add, Mul, Stack, BPAdd, BPStack };
enum class FFModMatrixParam { OpType, Source, Target, Amount, Count };
std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool global, FBStaticTopo const* topo);