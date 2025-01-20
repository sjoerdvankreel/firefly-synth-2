#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;

class FBParamLabel:
public juce::Label,
public IFBHorizontalAutoSize
{
public:
  FBParamLabel(FBRuntimeParam const* param);
  int FixedWidth(int height) const override;
};