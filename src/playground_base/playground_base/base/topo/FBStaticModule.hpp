#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <string>
#include <vector>
#include <functional>

// TODO move to own hpp all graph stuff
class FBPlugGUI;
struct FBModuleGraphComponentData;

typedef std::function<void(
  FBPlugGUI const* gui,
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