#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFGEchoTapCount = 6;
inline int constexpr FFGEchoMaxSeconds = 10;
std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();

enum class FFGEchoTarget { Voice, FX1, FX2, FX3, FX4, Out };

// todo taps->reverb/reverb->taps
// per-tap spread/lr-interaction
enum class FFGEchoParam {
  On, Target, Mix, Sync, 
  TapOn, TapFeedback, TapBalance, 
  TapDelaySeconds, TapDelayBars, TapLengthSeconds, TapLengthBars,
  TapLPOn, TapLPFreq, TapLPRes, TapFBLPOn, TapFBLPFreq, TapFBLPRes,
  TapHPOn, TapHPFreq, TapHPRes, TapFBHPOn, TapFBHPFreq, TapFBHPRes, Count };