#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

class FBParamDisplayLabel final:
public juce::Label,
public FBParamControl,
public IFBHorizontalAutoSize
{
  int const _maxTextWidth;

public:
  FBParamDisplayLabel(
    FBPlugGUI* plugGUI,
    FBRuntimeParam const* param,
    std::string const& maxWidthText);

  void parentHierarchyChanged() override;
  int FixedWidth(int height) const override;
  void SetValueNormalizedFromHost(double normalized) override;
};

class FBParamDisplayMeter final:
public juce::Component,
public FBParamControl
{
  int _fillCount = 0;

public:
  FBParamDisplayMeter(
    FBPlugGUI* plugGUI,
    FBRuntimeParam const* param);

  void paint(juce::Graphics& g) override;
  void parentHierarchyChanged() override;
  void SetValueNormalizedFromHost(double normalized) override;
};
