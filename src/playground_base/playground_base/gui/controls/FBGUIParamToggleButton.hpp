#pragma once

#include <playground_base/gui/shared/FBGUIParamControl.hpp>
#include <playground_base/gui/controls/FBAutoSizeToggleButton.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeGUIParam;

class FBGUIParamToggleButton final:
public FBAutoSizeToggleButton,
public FBGUIParamControl
{
  bool _isOn = {};

public:
  juce::String getTooltip() override;
  void buttonStateChanged() override;
  void parentHierarchyChanged() override;

  void SetValueNormalizedFromPlug(float normalized) override;
  FBGUIParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
};