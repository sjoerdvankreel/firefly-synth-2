#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <string>
#include <vector>
#include <functional>

struct FBRuntimeTopo;
struct FBGraphComponentData;

typedef std::function<void(
  FBRuntimeTopo const* topo, 
  int moduleSlot, 
  FBGraphComponentData* data)>
FBModuleGraphRenderer;

struct FBStaticModule final
{
  bool voice = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::vector<FBStaticParam> params = {};
  FBModuleGraphRenderer graphRenderer = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};