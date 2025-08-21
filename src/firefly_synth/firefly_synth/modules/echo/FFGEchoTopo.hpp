#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFGEchoTapCount = 8;
inline int constexpr FFGEchoMaxSeconds = 10;
std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();

enum class FFGEchoGUIParam { 
  TapSelect, Count };
enum class FFGEchoReverbType {
  Off, Main, Taps };
enum class FFGEchoFeedbackType {
  Off, Main, Taps };
enum class FFGEchoTarget { 
  Off, Voice, Out, 
  FX1In, FX1Out, FX2In, FX2Out, 
  FX3In, FX3Out, FX4In, FX4Out };

enum class FFGEchoParam {
  Target, TapsMix, Sync, 
  ReverbType, ReverbMix, ReverbXOver, 
  ReverbSize, ReverbDamp, ReverbAPF,
  ReverbLPFreq, ReverbLPRes, ReverbHPFreq, ReverbHPRes,
  FeedbackType, FeedbackMix, FeedbackAmount, FeedbackXOver,
  FeedbackDelayTime, FeedbackDelayBars, FeedbackDelaySmoothTime, FeedbackDelaySmoothBars,
  FeedbackLPFreq, FeedbackLPRes, FeedbackHPFreq, FeedbackHPRes,
  TapOn, TapBalance, TapLevel, TapXOver, 
  TapDelayTime, TapDelayBars, TapDelaySmoothTime, TapDelaySmoothBars,
  TapLPFreq, TapLPRes, TapHPFreq, TapHPRes, Count };