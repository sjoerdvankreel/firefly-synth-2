#pragma once

#include <playground_base/gui/shared/FBGUIParamControl.hpp>
#include <playground_base/gui/controls/FBAutoSizeComboBox.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeGUIParam;

class FBGUIParamComboBox final:
public FBAutoSizeComboBox,
public FBGUIParamControl
{
  int _maxTextWidth = 0;

public:
  juce::String getTooltip() override;
  void parentHierarchyChanged() override;
  void valueChanged(juce::Value& value) override;

  void SetValueNormalizedFromPlug(float normalized) override;
  FBGUIParamComboBox(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
};