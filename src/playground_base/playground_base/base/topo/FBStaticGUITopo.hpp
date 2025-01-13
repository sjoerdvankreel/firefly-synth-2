#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>
#include <functional>

class IFBPlugGUI;
class IFBHostGUIContext;
struct FBRuntimeTopo;

typedef std::function<std::unique_ptr<IFBPlugGUI>(
  FBRuntimeTopo const* topo, IFBHostGUIContext* context)>
FBPlugGUIFactory;

struct FBStaticGUITopo final
{
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticGUITopo);
  int width = {};
  int aspectRatioWidth = {};
  int aspectRatioHeight = {};
  FBPlugGUIFactory factory = {};
};