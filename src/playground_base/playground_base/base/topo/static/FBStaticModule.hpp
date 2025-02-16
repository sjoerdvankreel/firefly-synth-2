#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBModuleAddrs.hpp>
#include <playground_base/base/topo/static/FBStaticAudioParam.hpp>

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
  FBModuleGraphRenderer renderer = {};
  FBModuleAddrSelectors addrSelectors = {};
  std::vector<FBStaticAudioParam> audioParams = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};