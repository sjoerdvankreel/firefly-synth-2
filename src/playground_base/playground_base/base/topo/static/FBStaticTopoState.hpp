#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/static/FBSpecialParams.hpp>

#include <functional>

struct FBStaticTopo;
typedef std::function<FBSpecialParams (
  FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;
typedef std::function<std::array<FBVoiceInfo, FBMaxVoices>* (
  void* state)>
FBVoiceStateExchangeAddrSelector;

struct FBStaticTopoState final
{
  void* (*allocRawProcState)() = {};
  void* (*allocRawScalarState)() = {};
  void* (*allocRawExchangeState)() = {};
  void (*freeRawProcState)(void* state) = {};
  void (*freeRawScalarState)(void* state) = {};
  void (*freeRawExchangeState)(void* state) = {};

  int exchangeStateSize = {};
  int exchangeStateAlignment = {};
  FBSpecialParamsSelector specialSelector = {};
  FBVoiceStateExchangeAddrSelector voiceStateExchangeAddr = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopoState);
};