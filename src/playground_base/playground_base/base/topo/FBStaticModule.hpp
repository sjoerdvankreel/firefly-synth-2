#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticParam.hpp>

#include <string>
#include <vector>

struct FBStaticModule final
{
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::vector<FBStaticParam> params = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};