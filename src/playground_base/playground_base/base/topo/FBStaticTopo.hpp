#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBPlugVersion.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBStaticGUITopo.hpp>
#include <playground_base/base/topo/FBStaticStateTopo.hpp>

#include <vector>

struct FBStaticTopo final
{
  FBStaticGUITopo gui = {};
  FBPlugVersion version = {};
  FBStaticStateTopo state = {};
  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
};