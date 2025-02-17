#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIParamComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

FBGUIParamComponent::
FBGUIParamComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBParamsDependent(
  plugGUI, param->topoIndices.module, 
  param->topoIndices.param.slot, param->static_.dependencies),
_param(param) {}