#pragma once

#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeAudioParam;

class FBOutputParamLabel final:
public juce::Label,
public FBParamControl,
public IFBHorizontalAutoSize
{
  int const _maxTextWidth;

public:
  FBOutputParamLabel(
    FBPlugGUI* plugGUI,
    FBRuntimeAudioParam const* param,
    std::string const& defaultText,
    std::string const& maxWidthText);

  juce::String getTooltip() override;
  void parentHierarchyChanged() override;
  int FixedWidth(int height) const override;
  void SetValueNormalizedFromHost(float normalized) override;
};