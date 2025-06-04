#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_synth/modules/osci_base/FFOsciTopoBase.hpp>

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeStringOsciTopo();

inline int constexpr FFStringOsciMaxPoles = 8;
inline int constexpr FFStringOsciMaxSeed = 255;
inline int constexpr FFStringOsciGraphRounds = 5;

enum class FFStringOsciType { Off, Uni, Norm };
enum class FFStringOsciParam {
  Type, Gain, Coarse, Fine,
  UniCount, UniDetune, UniSpread, UniBlend,
  Seed, Poles, X, Y, Color, Excite, LPFreq, LPRes, LPKTrk, HPFreq, HPRes, HPKTrk,
  Damp, DampKTrk, Feedback, FeedbackKTrk, TrackingKey, TrackingRange, Count };