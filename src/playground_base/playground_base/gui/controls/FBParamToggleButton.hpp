#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamToggleButton final:
public juce::ToggleButton,
public FBParamControl,
public IFBHorizontalAutoSize
{
  bool _isOn = {};

public:
  juce::String getTooltip() override;
  void buttonStateChanged() override;

  int FixedWidth(int height) const override;
  void SetValueNormalizedFromHost(float normalized) override;
  FBParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};