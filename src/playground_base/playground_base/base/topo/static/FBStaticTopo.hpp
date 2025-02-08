#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>
#include <playground_base/base/topo/static/FBStaticTopoGUI.hpp>
#include <playground_base/base/topo/static/FBStaticTopoMeta.hpp>
#include <playground_base/base/topo/static/FBStaticTopoState.hpp>

#include <vector>

struct FBStaticTopo final
{
  FBStaticTopoGUI gui = {};
  FBStaticTopoMeta meta = {};
  FBStaticTopoState state = {};
  std::vector<FBStaticModule> modules = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopo);
};