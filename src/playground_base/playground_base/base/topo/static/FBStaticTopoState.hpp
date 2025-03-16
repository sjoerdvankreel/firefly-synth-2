#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/base/topo/static/FBSpecialParams.hpp>
#include <playground_base/base/topo/static/FBSpecialGUIParams.hpp>

#include <functional>

struct FBStaticTopo;
struct FBHostExchangeState;

typedef std::function<FBSpecialParams (
  FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;
typedef std::function<FBSpecialGUIParams(
  FBStaticTopo const& topo, void* state)>
FBSpecialGUIParamsSelector;
typedef std::function<FBHostExchangeState* (
  void* state)>
FBHostExchangeAddrSelector;
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
  FBHostExchangeAddrSelector hostExchangeAddr = {};
  FBSpecialGUIParamsSelector specialGUISelector = {};
  FBVoicesExchangeAddrSelector voicesExchangeAddr = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopoState);
};