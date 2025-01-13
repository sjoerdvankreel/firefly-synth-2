#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>
#include <functional>

struct FBRuntimeTopo;
class FBPlugGUIContext;
class IFBHostGUIContext;

typedef std::function<std::unique_ptr<FBPlugGUIContext>(
  FBRuntimeTopo const* topo, IFBHostGUIContext* context)>
FBPlugGUIFactory;

struct FBStaticGUITopo final
{
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticGUITopo);
  int plugWidth = {};
  int aspectRatioWidth = {};
  int aspectRatioHeight = {};
  FBPlugGUIFactory factory = {};
};