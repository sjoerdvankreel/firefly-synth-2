#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBPlugVersion.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

#include <vector>

struct FBStaticTopo final
{
  FBPlugVersion version = {};
  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);

  void* (*allocProcState)() = {};
  void (*freeProcState)(void* state) = {};
  void* (*allocScalarState)() = {};
  void (*freeScalarState)(void* state) = {};
};