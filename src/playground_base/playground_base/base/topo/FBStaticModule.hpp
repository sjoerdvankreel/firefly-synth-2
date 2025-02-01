#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <array>
#include <string>
#include <vector>
#include <functional>

struct FBModuleGraphComponentData;

typedef std::function<void(FBModuleGraphComponentData* graphData)>
FBModuleGraphRenderer;

struct FBStaticModule final
{
  bool voice = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::vector<FBStaticParam> params = {};
  FBModuleGraphRenderer renderGraph = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};