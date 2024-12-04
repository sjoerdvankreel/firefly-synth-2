#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBPlugVersion.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

#include <vector>

struct FBStaticTopo final
{
  FBPlugVersion version = {};
  void* (*allocScalarState)() = {};
  void (*freeScalarState)(void* state) = {};
  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
};