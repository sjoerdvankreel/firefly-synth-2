#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFGEchoTapCount = 8;
inline int constexpr FFGEchoMaxSeconds = 10;
std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();

enum class FFGEchoGUIParam { TapSelect, Count };
enum class FFGEchoTarget { Off, Voice, FX1, FX2, FX3, FX4, Out };

// todo taps->reverb/reverb->taps
// per-tap spread/lr-interaction
enum class FFGEchoParam {
  Target, Mix, Sync, 
  TapOn, TapFeedback, TapBalance, TapLevel, TapXOver, TapFBXOver,
  TapDelayTime, TapDelayBars, TapLengthTime, TapLengthBars,
  TapLPOn, TapLPFreq, TapLPRes, TapFBLPOn, TapFBLPFreq, TapFBLPRes,
  TapHPOn, TapHPFreq, TapHPRes, TapFBHPOn, TapFBHPFreq, TapFBHPRes, Count };