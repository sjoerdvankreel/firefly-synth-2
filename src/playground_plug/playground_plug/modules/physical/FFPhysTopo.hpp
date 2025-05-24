#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_plug/modules/osci_base/FFOsciTopoBase.hpp>

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakePhysTopo();

inline int constexpr FFPhysMaxPoles = 8;
inline int constexpr FFPhysMaxSeed = 255;
inline int constexpr FFPhysGraphRounds = 5;

enum class FFPhysType { Off, Uni, Norm };
enum class FFPhysParam {
  Type, Gain, Coarse, Fine,
  UniCount, UniDetune, UniSpread, UniBlend,
  Seed, Poles, X, Y, Color, Excite, LP, HP,
  Damp, DampScale, Feedback, FeedbackScale, Center, Range, Count };