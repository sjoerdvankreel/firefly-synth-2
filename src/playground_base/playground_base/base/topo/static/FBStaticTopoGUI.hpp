#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <memory>
#include <functional>

class FBPlugGUI;
class FBHostGUIContext;

typedef std::function<std::unique_ptr<FBPlugGUI>(FBHostGUIContext* context)>
FBPlugGUIFactory;

struct FBStaticTopoGUI final
{
  int plugWidth = {};
  int aspectRatioWidth = {};
  int aspectRatioHeight = {};
  FBPlugGUIFactory factory = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticTopoGUI);
};