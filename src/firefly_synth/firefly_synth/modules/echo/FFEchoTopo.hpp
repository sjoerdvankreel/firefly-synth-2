#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFGEchoTapCount = 8;
inline int constexpr FFGEchoMaxSeconds = 10;
std::unique_ptr<FBStaticModule> FFMakeGEchoTopo();

enum class FFGEchoModule {
  Taps, Feedback, Reverb, Count };

enum class FFGEchoGUIParam { 
  TapSelect, Count };
enum class FFGEchoTarget { 
  Off, Voice, Out, 
  FX1In, FX1Out, FX2In, FX2Out, 
  FX3In, FX3Out, FX4In, FX4Out };
enum class FFGEchoOrder {
  TapsToFeedbackToReverb, TapsToReverbToFeedback,
  FeedbackToTapsToReverb, FeedbackToReverbToTaps,
  ReverbToTapsToFeedback, ReverbToFeedbackToTaps };

enum class FFGEchoParam {
  Target, Order, Gain, Sync,
  DelaySmoothTime, DelaySmoothBars,
  ReverbOn, ReverbMix, ReverbXOver, 
  ReverbSize, ReverbDamp, ReverbAPF,
  ReverbLPFreq, ReverbLPRes, ReverbHPFreq, ReverbHPRes,
  FeedbackOn, FeedbackMix, FeedbackAmount, FeedbackXOver,
  FeedbackDelayTime, FeedbackDelayBars,
  FeedbackLPFreq, FeedbackLPRes, FeedbackHPFreq, FeedbackHPRes,
  TapsOn, TapsMix,
  TapOn, TapBalance, TapLevel, TapXOver,
  TapDelayTime, TapDelayBars,
  TapLPFreq, TapLPRes, TapHPFreq, TapHPRes, Count };

inline int 
FFGEchoGetProcessingOrder(FFGEchoOrder order, FFGEchoModule module)
{
  switch (order)
  {
  case FFGEchoOrder::TapsToFeedbackToReverb:
    switch (module)
    {
    case FFGEchoModule::Taps: return 0;
    case FFGEchoModule::Feedback: return 1;
    case FFGEchoModule::Reverb: return 2;
    default: FB_ASSERT(false); return -1;
    }
  case FFGEchoOrder::TapsToReverbToFeedback:
    switch (module)
    {
    case FFGEchoModule::Taps: return 0;
    case FFGEchoModule::Feedback: return 2;
    case FFGEchoModule::Reverb: return 1;
    default: FB_ASSERT(false); return -1;
    }
  case FFGEchoOrder::FeedbackToTapsToReverb:
    switch (module)
    {
    case FFGEchoModule::Taps: return 1;
    case FFGEchoModule::Feedback: return 0;
    case FFGEchoModule::Reverb: return 2;
    default: FB_ASSERT(false); return -1;
    }
  case FFGEchoOrder::FeedbackToReverbToTaps:
    switch (module)
    {
    case FFGEchoModule::Taps: return 2;
    case FFGEchoModule::Feedback: return 0;
    case FFGEchoModule::Reverb: return 1;
    default: FB_ASSERT(false); return -1;
    }
  case FFGEchoOrder::ReverbToTapsToFeedback:
    switch (module)
    {
    case FFGEchoModule::Taps: return 1;
    case FFGEchoModule::Feedback: return 2;
    case FFGEchoModule::Reverb: return 0;
    default: FB_ASSERT(false); return -1;
    }
  case FFGEchoOrder::ReverbToFeedbackToTaps:
    switch (module)
    {
    case FFGEchoModule::Taps: return 2;
    case FFGEchoModule::Feedback: return 1;
    case FFGEchoModule::Reverb: return 0;
    default: FB_ASSERT(false); return -1;
    }
  default: FB_ASSERT(false); return -1;
  }
}