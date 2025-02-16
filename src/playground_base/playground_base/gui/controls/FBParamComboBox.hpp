#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/controls/FBAutoSizeComboBox.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeAudioParam;

class FBParamComboBox final:
public FBAutoSizeComboBox,
public FBParamControl
{
  int _maxTextWidth = 0;

public:
  juce::String getTooltip() override;
  void parentHierarchyChanged() override;
  void valueChanged(juce::Value& value) override;
  void SetValueNormalizedFromHost(float normalized) override;
  FBParamComboBox(FBPlugGUI* plugGUI, FBRuntimeAudioParam const* param);
};