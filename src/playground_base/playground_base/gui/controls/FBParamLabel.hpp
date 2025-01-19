#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct FBRuntimeParam;

class FBParamLabel:
public juce::Label
{
public:
  FBParamLabel(FBRuntimeParam const* param);
};