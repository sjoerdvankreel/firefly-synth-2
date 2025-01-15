#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBSpecialParams.hpp>

#include <functional>

struct FBStaticTopo;
typedef std::function<FBSpecialParams (
  FBStaticTopo const& topo, void* state)>
FBSpecialParamsSelector;

struct FBStaticTopoState final
{
  void* (*allocRawProcState)() = {};
  void* (*allocRawScalarState)() = {};
  void (*freeRawProcState)(void* state) = {};
  void (*freeRawScalarState)(void* state) = {};
  FBSpecialParamsSelector specialSelector = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopoState);
};