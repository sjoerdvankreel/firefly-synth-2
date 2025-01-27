#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <string>
#include <vector>
#include <functional>

// TODO move to own hpp all graph stuff
struct FBRuntimeTopo;
struct FBScalarStateContainer;
struct FBModuleGraphComponentData;

typedef std::function<void(
  FBRuntimeTopo const* topo, 
  FBScalarStateContainer const* scalar,
  int moduleSlot, 
  FBModuleGraphComponentData* data)>
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