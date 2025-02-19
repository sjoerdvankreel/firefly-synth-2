#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamsDependent(plugGUI, param->topoIndices.module, param->static_.dependencies),
_param(param) {}