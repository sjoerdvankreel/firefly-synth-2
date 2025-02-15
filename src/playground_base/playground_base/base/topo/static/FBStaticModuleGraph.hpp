#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>

#include <string>
#include <functional>

struct FBModuleGraphComponentData;
typedef std::function<void(FBModuleGraphComponentData* graphData)>
FBModuleGraphRenderer;

struct FBStaticModuleGraph final
{
  bool enabled = false;
  bool hasControl = false;
  FBLinearParam controlParam;
  std::string controlUnit = {};
  std::string controlName = {};
  std::string controlTooltip = {};
  FBModuleGraphRenderer renderer = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModuleGraph);
};