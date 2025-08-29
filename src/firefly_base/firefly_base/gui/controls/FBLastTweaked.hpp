#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBLastTweakedTextBox:
public juce::TextEditor,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize,
public FBParamListener
{
  int _paramIndex = -1;
  int const _fixedWidth;
  FBPlugGUI* const _plugGUI;

public:
  ~FBLastTweakedTextBox();
  FBLastTweakedTextBox(FBPlugGUI* plugGUI, int fixedWidth);

  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  void AudioParamChangedFromUI(int index, double normalized) override;
};
