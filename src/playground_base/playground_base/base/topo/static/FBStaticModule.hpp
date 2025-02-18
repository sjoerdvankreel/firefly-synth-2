#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBModuleAddrs.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/static/FBStaticGUIParam.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <functional>

class FBPlugGUI;
struct FBModuleGraphComponentData;

typedef std::function<void(FBModuleGraphComponentData* graphData)>
FBModuleGraphRenderer;
typedef std::function<std::unique_ptr<juce::Component>(FBPlugGUI* plugGUI, int moduleSlot)>
FBModuleGraphControlFactory;

struct FBStaticModule final
{
  bool voice = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  FBModuleAddrSelectors addrSelectors = {};
  FBModuleGraphRenderer graphRenderer = {};
  FBModuleGraphControlFactory graphControlFactory = {};
  std::vector<FBStaticParam> params = {};
  std::vector<FBStaticGUIParam> guiParams = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};