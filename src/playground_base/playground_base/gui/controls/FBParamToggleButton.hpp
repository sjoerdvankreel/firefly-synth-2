#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/controls/FBAutoSizeToggleButton.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamToggleButton final:
public FBAutoSizeToggleButton,
public FBParamControl
{
  bool _isOn = {};

public:
  juce::String getTooltip() override;
  void buttonStateChanged() override;
  void parentHierarchyChanged() override;

  void SetValueNormalizedFromHost(double normalized) override;
  FBParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};