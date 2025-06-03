#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;
struct FBRuntimeGUIParam;

class FBAutoSizeToggleButton:
public juce::ToggleButton,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};

class FBGUIParamToggleButton final:
public FBAutoSizeToggleButton,
public FBGUIParamControl
{
  bool _isOn = {};

public:
  juce::String getTooltip() override;
  void buttonStateChanged() override;
  void parentHierarchyChanged() override;

  void SetValueNormalizedFromPlug(double normalized) override;
  FBGUIParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param);
};

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
