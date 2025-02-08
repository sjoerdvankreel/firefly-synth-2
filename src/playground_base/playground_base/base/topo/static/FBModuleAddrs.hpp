#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <functional>

struct FBModuleProcExchangeState;

typedef std::function<FBModuleProcExchangeState* (
  int moduleSlot, void* state)>
FBGlobalModuleExchangeAddrSelector;
typedef std::function<FBModuleProcExchangeState* (
  int voice, int moduleSlot, void* state)>
FBVoiceModuleExchangeAddrSelector;

struct FBModuleAddrSelectors final
{
  FB_COPY_MOVE_DEFCTOR(FBModuleAddrSelectors);
  FBVoiceModuleExchangeAddrSelector voiceModuleExchange = {};
  FBGlobalModuleExchangeAddrSelector globalModuleExchange = {};
};