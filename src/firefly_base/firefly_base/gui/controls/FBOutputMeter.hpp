#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeParam;

class FBOutputParamMeter final:
public juce::Component,
public juce::TooltipClient,
public FBParamControl
{
  double _valueNormalized = {};

public:
  FBOutputParamMeter(
    FBPlugGUI* plugGUI,
    FBRuntimeParam const* param);

  juce::String getTooltip() override;
  void paint(juce::Graphics& g) override;
  void parentHierarchyChanged() override;
  void SetValueNormalizedFromHost(double normalized) override;
};