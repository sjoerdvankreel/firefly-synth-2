#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBSpecialParams.hpp>

#include <functional>

struct FBStaticTopo;
typedef std::function<FBSpecialParams (
  FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;

struct FBStaticStateTopo final
{
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticStateTopo);

  void* (*allocRawProcState)() = {};
  void* (*allocRawScalarState)() = {};
  void (*freeRawProcState)(void* state) = {};
  void (*freeRawScalarState)(void* state) = {};
  FBSpecialParamsSelector specialSelector = {};
};