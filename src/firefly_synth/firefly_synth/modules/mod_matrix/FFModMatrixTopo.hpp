#pragma once

#include <string>
#include <memory>
#include <utility>

inline int constexpr FFModMatrixSlotCount = 32;

struct FBStaticModule;
enum class FFModMatrixOpType { Add, Mul, Stack };
enum class FFModMatrixParam { On, Source, Target, OpType, Bipolar, Amount };

std::unique_ptr<FBStaticModule> FFMakeModMatrixTopo();