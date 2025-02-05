#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <functional>

typedef std::function<float* (
  int moduleSlot, void* state)>
FBGlobalExchangeActiveAddrSelector;
typedef std::function<float* (
  int voice, int moduleSlot, void* state)>
FBVoiceExchangeActiveAddrSelector;

struct FBModuleAddrSelectors final
{
  FB_COPY_MOVE_DEFCTOR(FBModuleAddrSelectors);
  FBVoiceExchangeActiveAddrSelector voiceExchangeActive = {};
  FBGlobalExchangeActiveAddrSelector globalExchangeActive = {};
};