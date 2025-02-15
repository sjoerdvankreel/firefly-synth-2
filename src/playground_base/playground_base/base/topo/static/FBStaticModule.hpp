#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBModuleAddrs.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/static/FBStaticModuleGraph.hpp>

#include <array>
#include <string>
#include <vector>

struct FBStaticModule final
{
  bool voice = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  FBStaticModuleGraph graph = {};
  std::vector<FBStaticParam> params = {};
  FBModuleAddrSelectors addrSelectors = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};