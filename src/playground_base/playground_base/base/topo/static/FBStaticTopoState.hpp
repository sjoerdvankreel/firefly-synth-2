#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/static/FBSpecialParams.hpp>

#include <functional>

struct FBStaticTopo;
struct FBProcExchangeState;

// TODO drop selector in favor of just-point-at-smooth-param
typedef std::function<FBSpecialParams (
  FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;
typedef std::function<FBProcExchangeState* (
  void* state)>
FBProcExchangeAddrSelector;
typedef std::function<std::array<FBVoiceInfo, FBMaxVoices>* (
  void* state)>
FBVoicesExchangeAddrSelector;

struct FBStaticTopoState final
{
  void* (*allocRawGUIState)() = {};
  void* (*allocRawProcState)() = {};
  void* (*allocRawScalarState)() = {};
  void* (*allocRawExchangeState)() = {};
  void (*freeRawGUIState)(void* state) = {};
  void (*freeRawProcState)(void* state) = {};
  void (*freeRawScalarState)(void* state) = {};
  void (*freeRawExchangeState)(void* state) = {};

  int exchangeStateSize = {};
  int exchangeStateAlignment = {};
  FBSpecialParamsSelector specialSelector = {};
  FBProcExchangeAddrSelector procExchangeAddr = {};
  FBVoicesExchangeAddrSelector voicesExchangeAddr = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopoState);
};