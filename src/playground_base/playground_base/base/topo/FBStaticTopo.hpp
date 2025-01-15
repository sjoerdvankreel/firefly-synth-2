#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/base/topo/FBStaticTopoGUI.hpp>
#include <playground_base/base/topo/FBStaticTopoMeta.hpp>
#include <playground_base/base/topo/FBStaticTopoState.hpp>

#include <vector>

struct FBStaticTopo final
{
  FBStaticTopoGUI gui = {};
  FBStaticTopoMeta meta = {};
  FBStaticTopoState state = {};
  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
};