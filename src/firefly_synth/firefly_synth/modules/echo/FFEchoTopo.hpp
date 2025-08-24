#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <memory>

struct FBStaticModule;
inline int constexpr FFEchoTapCount = 8;
inline int constexpr FFEchoMaxSeconds = 10;
std::unique_ptr<FBStaticModule> FFMakeEchoTopo(bool global);

enum class FFEchoGUIParam { 
  TapSelect, Count };
enum class FFGEchoTarget { 
  Off, Voice, Out, 
  FX1In, FX1Out, FX2In, FX2Out, 
  FX3In, FX3Out, FX4In, FX4Out };

enum class FFEchoModule {
  Taps, Feedback, Reverb, Count };
enum class FFEchoOrder {
  TapsToFeedbackToReverb, TapsToReverbToFeedback,
  FeedbackToTapsToReverb, FeedbackToReverbToTaps,
  ReverbToTapsToFeedback, ReverbToFeedbackToTaps };

enum class FFEchoParam {
  VOnOrGTarget, VOrderOrGOrder, Gain, Sync,
  DelaySmoothTime, DelaySmoothBars,
  ReverbOn = FFEchoParam::Count, ReverbMix, ReverbXOver,
  ReverbSize, ReverbDamp, ReverbAPF,
  ReverbLPFreq, ReverbLPRes, ReverbHPFreq, ReverbHPRes,
  FeedbackOn, FeedbackMix, FeedbackAmount, FeedbackXOver,
  FeedbackDelayTime, FeedbackDelayBars,
  FeedbackLPFreq, FeedbackLPRes, FeedbackHPFreq, FeedbackHPRes,
  TapsOn, TapsMix, TapOn, TapBalance, 
  TapLevel, TapXOver, TapDelayTime, TapDelayBars,
  TapLPFreq, TapLPRes, TapHPFreq, TapHPRes, Count };

inline int 
FFEchoGetProcessingOrder(FFEchoOrder order, FFEchoModule module)
{
  switch (order)
  {
  case FFEchoOrder::TapsToFeedbackToReverb:
    switch (module)
    {
    case FFEchoModule::Taps: return 0;
    case FFEchoModule::Feedback: return 1;
    case FFEchoModule::Reverb: return 2;
    default: FB_ASSERT(false); return -1;
    }
  case FFEchoOrder::TapsToReverbToFeedback:
    switch (module)
    {
    case FFEchoModule::Taps: return 0;
    case FFEchoModule::Feedback: return 2;
    case FFEchoModule::Reverb: return 1;
    default: FB_ASSERT(false); return -1;
    }
  case FFEchoOrder::FeedbackToTapsToReverb:
    switch (module)
    {
    case FFEchoModule::Taps: return 1;
    case FFEchoModule::Feedback: return 0;
    case FFEchoModule::Reverb: return 2;
    default: FB_ASSERT(false); return -1;
    }
  case FFEchoOrder::FeedbackToReverbToTaps:
    switch (module)
    {
    case FFEchoModule::Taps: return 2;
    case FFEchoModule::Feedback: return 0;
    case FFEchoModule::Reverb: return 1;
    default: FB_ASSERT(false); return -1;
    }
  case FFEchoOrder::ReverbToTapsToFeedback:
    switch (module)
    {
    case FFEchoModule::Taps: return 1;
    case FFEchoModule::Feedback: return 2;
    case FFEchoModule::Reverb: return 0;
    default: FB_ASSERT(false); return -1;
    }
  case FFEchoOrder::ReverbToFeedbackToTaps:
    switch (module)
    {
    case FFEchoModule::Taps: return 2;
    case FFEchoModule::Feedback: return 1;
    case FFEchoModule::Reverb: return 0;
    default: FB_ASSERT(false); return -1;
    }
  default: FB_ASSERT(false); return -1;
  }
}