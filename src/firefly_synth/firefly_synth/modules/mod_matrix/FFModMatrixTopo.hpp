#pragma once

#include <string>
#include <memory>
#include <utility>

inline int constexpr FFModMatrixSlotCount = 18;

struct FBStaticTopo;
struct FBStaticModule;
enum class FFModMatrixOpType { Off, Add, Mul, Stack };
enum class FFModMatrixParam { OpType, Source, Target, Bipolar, Amount, Count };
std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo(bool global, FBStaticTopo const* topo);