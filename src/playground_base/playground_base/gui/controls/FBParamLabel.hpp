#pragma once

#include <playground_base/gui/shared/FBParamComponent.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBParamLabel:
public juce::Label,
public FBParamComponent,
public IFBHorizontalAutoSize
{
  int const _textWidth;
public:
  void parentHierarchyChanged() override;
  int FixedWidth(int height) const override;
  FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param);
};