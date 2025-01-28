#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <string>
#include <vector>
#include <functional>

struct FBModuleGraphComponentData;

typedef std::function<void(FBModuleGraphComponentData* data)>
FBModuleGraphRenderer;

struct FBStaticModule final
{
  bool voice = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  FBModuleGraphRenderer renderGraph = {};
  std::vector<FBStaticParam> params = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};