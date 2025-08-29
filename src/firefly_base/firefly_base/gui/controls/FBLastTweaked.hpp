#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBLastTweakedLabel:
public juce::Label,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize,
public IFBParamListener
{
  int _maxWidth = {};
  FBPlugGUI* const _plugGUI;

public:
  ~FBLastTweakedLabel();
  FBLastTweakedLabel(FBPlugGUI* plugGUI);

  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  void AudioParamChangedFromUI(int index, double normalized) override;
};

class FBLastTweakedTextBox:
public juce::TextEditor,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize,
public IFBParamListener
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
