#pragma once

#include <playground_base/gui/controls/FBAutoSizeLabel.hpp>
#include <playground_base/gui/shared/FBGUIParamComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeGUIParam;

class FBGUIParamLabel final:
public FBAutoSizeLabel,
public FBGUIParamComponent
{
public:
  FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
};