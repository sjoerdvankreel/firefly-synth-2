#pragma once

#include <string>
#include <memory>
#include <utility>

inline int constexpr FFModMatrixVoiceSlotCount = 32;
inline int constexpr FFModMatrixGlobalSlotCount = 16;

struct FBStaticTopo;
struct FBStaticModule;
enum class FFModMatrixOpType { Off, Add, Mul, Stack, BPAdd, BPStack };
enum class FFModMatrixParam { OpType, Source, Target, Amount, Count };
std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool global, FBStaticTopo const* topo);