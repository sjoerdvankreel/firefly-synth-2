#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_plug/modules/osci_base/FFOsciTopoBase.hpp>

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeKSNoiseTopo();

inline int constexpr FFKSNoiseMaxPoles = 8;
inline int constexpr FFKSNoiseMaxSeed = 255;
inline int constexpr FFKSNoiseGraphRounds = 10;

enum class FFKSNoiseType { Off, Uni, Norm };
enum class FFKSNoiseParam {
  Type, Gain, Coarse, Fine,
  UniCount, UniDetune, UniSpread, UniBlend,
  Seed, Poles, Color, X, Y, Damp, DampScale, Feedback, FeedbackScale, Center, Range, Count };