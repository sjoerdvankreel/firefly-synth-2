#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFGEchoTapCount = 8;
inline int constexpr FFGEchoMaxSeconds = 10;
std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();

enum class FFGEchoGUIParam { TapSelect, Count };
enum class FFGEchoTarget { Off, Voice, FX1, FX2, FX3, FX4, Out };
enum class FFGEchoReverbPlacement { Off, BeforeTaps, AfterTaps };

enum class FFGEchoParam {
  Target, Mix, Sync, 
  ReverbPlacement, ReverbMix, ReverbAPF, 
  ReverbSpread, ReverbSize, ReverbDamp,
  ReverbLPOn, ReverbLPFreq, ReverbLPRes,
  ReverbHPOn, ReverbHPFreq, ReverbHPRes,
  TapOn, TapFeedback, TapBalance, TapLevel, TapXOver, TapFBXOver,
  TapDelayTime, TapDelayBars, TapLengthTime, TapLengthBars,
  TapLPOn, TapLPFreq, TapLPRes, TapFBLPOn, TapFBLPFreq, TapFBLPRes,
  TapHPOn, TapHPFreq, TapHPRes, TapFBHPOn, TapFBHPFreq, TapFBHPRes, Count };