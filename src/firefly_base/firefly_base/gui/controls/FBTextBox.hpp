#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBAutoSizeTextBox:
public juce::TextEditor,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int const _fixedWidth;

public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBAutoSizeTextBox(int fixedWidth);
};

class FBLastTweakedTextBox:
public FBAutoSizeTextBox,
public FBParamListener
{
  int _paramIndex = -1;
  FBPlugGUI* const _plugGUI;

public:
  ~FBLastTweakedTextBox();
  FBLastTweakedTextBox(FBPlugGUI* plugGUI, int fixedWidth);
  void AudioParamChangedFromUI(int index, double normalized) override;
};
