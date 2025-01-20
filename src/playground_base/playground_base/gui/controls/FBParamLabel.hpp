#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;

class FBParamLabel:
public juce::Label,
public IFBHorizontalAutoSize
{
public:
  int FixedWidth() const override;
  FBParamLabel(FBRuntimeParam const* param);
};