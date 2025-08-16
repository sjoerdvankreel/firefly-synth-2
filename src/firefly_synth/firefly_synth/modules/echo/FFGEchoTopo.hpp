#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFGEchoTapCount = 6;
inline int constexpr FFGEchoMaxSeconds = 10;
std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();

enum class FFGEchoTarget { Voice, FX1, FX2, FX3, FX4, Out };
enum class FFGEchoParam {
  On, Target, Mix, Sync, TapOn,
  TapDelaySeconds, TapDelayBars, TapLengthSeconds, TapLengthBars,
  TapFeedback, TapBalance, TapLPFreq, TapLPRes, TapHPFreq, TapHPRes, Count };