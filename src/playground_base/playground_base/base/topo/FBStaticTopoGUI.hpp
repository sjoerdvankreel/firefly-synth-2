#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>
#include <functional>

class FBPlugGUI;
class FBHostGUIContext;

struct FBRuntimeTopo;

typedef std::function<std::unique_ptr<FBPlugGUI>(
  FBRuntimeTopo const* topo, FBHostGUIContext* context)>
FBPlugGUIFactory;

struct FBStaticTopoGUI final
{
  int plugWidth = {};
  float minUserScale = {};
  float maxUserScale = {};
  int aspectRatioWidth = {};
  int aspectRatioHeight = {};
  FBPlugGUIFactory factory = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopoGUI);
};