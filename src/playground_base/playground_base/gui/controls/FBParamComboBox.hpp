#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamComboBox final:
public juce::ComboBox,
public FBParamControl,
public IFBHorizontalAutoSize
{
  int _maxTextWidth = 0;

public:
  juce::String getTooltip() override;
  void parentHierarchyChanged() override;
  int FixedWidth(int height) const override;
  void valueChanged(juce::Value& value) override;
  void SetValueNormalizedFromHost(float normalized) override;
  FBParamComboBox(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};